#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <vector>
#include <random>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <SDL_ttf.h>
namespace fs = std::filesystem;
//using namespace std;
const double PI = 3.1415926535;



struct Vec3 {
    double x, y, z;
    Vec3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
};

struct Star {//星星所有数据
    int name;
    Vec3 absolute_pos;
    Vec3 relative_pos;
    double distance;
    double depth;
    SDL_Point screen_pos;
    double temperature;//星云色相存储于此，使用不同于恒星函数将其转换为颜色
    double power;
    int dysondensity;
    double radius;
    std::string teamname;
    bool needtoshowpos;
    SDL_Point zpoint;
    double zpdep;
    int type;//0恒星，1星云
};

struct opoint {//其他标记点
    Vec3 absolute_pos;
    Vec3 relative_pos;
    double distance;
    double depth;
    SDL_Point screen_pos;
    
};

Vec3 add_vectors(const Vec3& v1, const Vec3& v2) {
    return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vec3 subtract_vectors(const Vec3& v1, const Vec3& v2) {
    return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vec3 cross_product(const Vec3& v1, const Vec3& v2) {
    return Vec3(
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    );
}

double dot_product(const Vec3& v1, const Vec3& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3 normalize_vector(const Vec3& v) {
    double length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0) {
        throw std::runtime_error("Cannot normalize zero vector");
    }
    return Vec3(v.x / length, v.y / length, v.z / length);
}

double vector_length(const Vec3& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

SDL_Color ScaleSDLColor(double x, const SDL_Color& color) {//rgb数乘,带过曝处理
    SDL_Color newColor;

    newColor.r = static_cast<Uint8>(std::min(color.r * x, 255.0));  // 缩放并限制在 [0, 255]
    newColor.g = static_cast<Uint8>(std::min(color.g * x, 255.0));  // 缩放并限制在 [0, 255]
    newColor.b = static_cast<Uint8>(std::min(color.b * x, 255.0));  // 缩放并限制在 [0, 255]
    newColor.a = color.a;  // 保持 alpha 不变

    return newColor;
}

SDL_Color kelvin_to_rgb(double temperature) {
    temperature /= 100;

    int red, green, blue;

    if (temperature <= 66) {
        red = 255;
        green = static_cast<int>((-0.60884257109 - 0.00174890002 * (temperature - 2) + 0.40977318429 * std::log(temperature - 10)) * 256);
        blue = static_cast<int>((-0.99909549742 + 0.00324474355 * (temperature - 10) + 0.45364683926 * std::log(temperature - 10)) * 256);
    } else {
        red = static_cast<int>((1.38030159086 + 0.00044786845 * (temperature - 55) - 0.15785750233 * std::log(temperature - 55)) * 256);
        green = static_cast<int>((1.27627220616 + 0.0003115081 * (temperature - 50) - 0.11013841706 * std::log(temperature - 50)) * 256);
        blue = 255;
    }

    red = std::max(0, std::min(255, red));
    green = std::max(0, std::min(255, green));
    blue = std::max(0, std::min(255, blue));

    return {static_cast<Uint8>(red), static_cast<Uint8>(green), static_cast<Uint8>(blue), 255};
}

void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y) {
        SDL_RenderDrawLine(renderer, centerX + x, centerY + y, centerX - x, centerY + y);
        SDL_RenderDrawLine(renderer, centerX + y, centerY + x, centerX - y, centerY + x);
        SDL_RenderDrawLine(renderer, centerX - x, centerY - y, centerX + x, centerY - y);
        SDL_RenderDrawLine(renderer, centerX - y, centerY - x, centerX + y, centerY - x);

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0) {
            x--;
            dx += 2;
            err += (-radius << 1) + dx;
        }
    }
}

double variable_threshold001(double variable) {
    return (variable > 0.01) ? variable : 0.01;
}

double variable_threshold00(double variable) {
    return (variable > 0.00000000001) ? variable : 0.00000000001;
}

double variable_threshold1(double variable) {
    return (variable < 1) ? variable : 1;
}

Vec3 scalar_multiply(double scalar, const Vec3& v) {
    return Vec3(scalar * v.x, scalar * v.y, scalar * v.z);
}

std::string random_name() {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(6);

    for (int i = 0; i < 6; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}
//————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
class Button {//Button abutton(0, 0, 100, 100, true, { 100,100,100 }, "aa");
public:
    Button(){
        text_rect = { 9,9,8,8 };
        back_rect = { 9,9,8,8 };
        button_rect = { 10,10,10,10 };
        toshow = true;
        color = { 100,100,100 };
        text = "btext";
        lastpress = 0;
        press = 0;
        state = 0;
        mouson = 0;
    }
    Button(int x, int y, int w, int h, bool show, SDL_Color bcolor, std::string btext){
        text_rect = { int(x + 0.1 * w),int(y + 0.1 * h),int(0.8 * w),int(0.8 * h) };
        back_rect = { int(x + 0.05 * w),int(y + 0.05 * h),int(0.9 * w),int(0.9 * h) };
        button_rect = { x,y,w,h };
        toshow = show;
        color = bcolor;
        text = btext;
        lastpress = 0;
        press = 0;
        state = 0;
        mouson = 0;
    }
    bool buttoncal(int x, int y, int w, int h, bool show);
    bool buttoncal(int x, int y, int w, int h);
    bool buttoncal(bool show);
    bool buttoncal();
    void drawbutton(SDL_Renderer* renderer) const;

    bool mouson;
    bool state;
    bool toshow;
    SDL_Rect button_rect;
    SDL_Rect back_rect;
    SDL_Rect text_rect;
    SDL_Color color;
    std::string text;


    bool lastpress;
    bool press;
    
};

bool Button::buttoncal(bool show) {
    
    toshow = show;
    if (toshow == true) {
        int mx, my;
        const Uint32 moubuts = SDL_GetMouseState(&mx, &my);
        SDL_Point mp = { mx,my };
        mouson = SDL_PointInRect(&mp, &button_rect);

        press = 0;
        if (mouson == true && (moubuts & SDL_BUTTON(SDL_BUTTON_LEFT))) {
            press = true;
        }
        if (!press && lastpress && mouson) {
            state = !state;

        }
        lastpress = press;

    }
    return state;
}

bool Button::buttoncal(int x, int y, int w, int h, bool show) {
    text_rect = { int(x + 0.1 * w),int(y + 0.1 * h),int(0.8 * w),int(0.8 * h) };
    back_rect = { int(x + 0.05 * w),int(y + 0.05 * h),int(0.9 * w),int(0.9 * h) };
    button_rect = { x,y,w,h };
    toshow = show;
    if(toshow==true){
        int mx, my;
        const Uint32 moubuts = SDL_GetMouseState(&mx, &my);
        SDL_Point mp = { mx,my };
        mouson = SDL_PointInRect(&mp, &button_rect);

        press = 0;
        if (mouson == true && (moubuts & SDL_BUTTON(SDL_BUTTON_LEFT))) {
            press = true;
        }
        if (!press && lastpress && mouson) {
            state = !state;

        }
        lastpress = press;

    }
    return state;
}

bool Button::buttoncal(int x, int y, int w, int h) {
    text_rect = { int(x + 0.1 * w),int(y + 0.1 * h),int(0.8 * w),int(0.8 * h) };
    back_rect = { int(x + 0.05 * w),int(y + 0.05 * h),int(0.9 * w),int(0.9 * h) };
    button_rect = { x,y,w,h };

    if (toshow == true) {
        int mx, my;
        const Uint32 moubuts = SDL_GetMouseState(&mx, &my);
        SDL_Point mp = { mx,my };
        mouson = SDL_PointInRect(&mp, &button_rect);

        press = 0;
        if (mouson == true && (moubuts & SDL_BUTTON(SDL_BUTTON_LEFT))) {
            press = true;
        }
        if (!press && lastpress && mouson) {
            state = !state;

        }
        lastpress = press;

    }
    return state;
}

bool Button::buttoncal() {
    if (toshow == true) {
        int mx, my;
        const Uint32 moubuts = SDL_GetMouseState(&mx, &my);
        SDL_Point mp = { mx,my };
        mouson = SDL_PointInRect(&mp, &button_rect);

        press = 0;
        if (mouson == true && (moubuts & SDL_BUTTON(SDL_BUTTON_LEFT))) {
            press = true;
        }
        if (!press && lastpress && mouson) {
            state = !state;

        }
        lastpress = press;

    }
    return state;
}

void Button::drawbutton(SDL_Renderer* renderer) const {
    if(toshow==true){
        if (state == true) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            if (mouson == true) {
                SDL_SetRenderDrawColor(renderer, 0, 200, 200, 255);
            }
            SDL_RenderFillRect(renderer, &button_rect);
        }
        else if (state == false) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            if (mouson == true) {
                SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
            }
            SDL_RenderFillRect(renderer, &button_rect);
        }
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &back_rect);
        TTF_Font* font = TTF_OpenFont("TCM.TTF", 100);
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), { 0,0,0 });
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        int text_width = surface->w;
        int text_height = surface->h;
        SDL_FreeSurface(surface);
        TTF_CloseFont(font);
        SDL_RenderCopy(renderer, texture, nullptr, &text_rect);
        SDL_DestroyTexture(texture);
    }
}
//————————————————————————————————————————————————————————————————————————————————————————————————————
class StarMap {//屏幕各项数据，声明与初始化可能有遗漏
public:
    StarMap(int width, int height);
    
    void run(int number);
    void menu();
    int width, height;
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    bool running;
    bool ifexit;
    bool ifsave;

    std::vector<Star> stars;
   
    std::vector<opoint> opoints;
    std::unordered_map<int, std::string> star_messages;
    
    Button menubutton;
    Button coordinate;
    Button exitbutton;

    Vec3 cenposcam;//对应11111
    Vec3 reposcam;
    Vec3 poscam;
    Vec3 vecx, vecy, vecz;
    Vec3 target;
    Vec3 circen;
    
    double offset_x;
    double offset_y;
    double posx, posy;
    double mposx, mposy;
    double theta, phi;
    double r, rtarget;//确定
    
    int targetname;
    int lasttargetname;
    SDL_Color targetcolor;
    int targetcloud;
    bool totar;
    std::string text;//确定
    
    double scroll_y;
    double targetscroll_y;
    double messagex;
    double messagextarget;
    
    double messagewidthrate;
    double messageheightrate;
    double menutargetscolly;
    double menuscolly;

    Uint32 last_time;
    double deltatime;
    
    Uint32 click_time;
    double deltaclicktime;
    bool doubleclick;
    bool showmessage;
    bool dragging;

    bool showcircle;
    bool showvertical;

    SDL_Texture* menuback;
    
    SDL_Texture* tieimg[4];
    SDL_Texture* mesimg[5];
    
    void init_SDL();
    void load_textures();
    void generate_stars();
    void generate_nebula();
    void read_stars(int number);
    void generate_opoints();
    void handle_events();
    void update();
    void render();
    bool save(int number);
    void cleanup();
    

    void handle_mouse_wheel(SDL_Event& event);
    void handle_mouse_button_down(SDL_Event& event);
    void handle_mouse_button_up(SDL_Event& event);
    void handle_mouse_motion(SDL_Event& event);
    
    void update_camera();
    void sortsatrsbydistance();
    void update_stars();
    void update_opoints();


    void renderTextureWithColor(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Color color, SDL_Rect destRect);
    void draw_stars();
    void draw_opoints();
    void draw_info_panel();

    std::vector<std::string> split_string(const std::string& s, char delimiter);
    
    SDL_Texture* load_texture(const std::string& path);
};


StarMap::StarMap(int w, int h) : width(w), height(h), running(true), menubutton(0, 0, 100, 100, 1, { 100,100,100 ,255 }, "menu"), coordinate(0, 0, 100, 100, 0, { 100,100,100 ,255 }, "coordinate"), exitbutton(0, 0, 100, 100, 0, { 100,100,100 ,255 }, "exit&save") {//屏幕初始化
    init_SDL();
    TTF_Init();
    load_textures();
    generate_opoints();

    ifexit = 0;
    ifsave = 0;

    cenposcam = Vec3(0, 0, 0);//对应11111
    reposcam = Vec3(0, 0, 0);
    poscam = Vec3(0, 0, 0);
    vecx = Vec3(0, 0, 0);
    vecy = Vec3(0, 0, 0);
    vecz = Vec3(0, 0, 0);
    target = Vec3(0, 0, 0);
    circen = Vec3(0, 0, 0);

    offset_x = 0;
    offset_y = 0;
    posx = 0;
    posy = height;
    mposx = 0;
    mposy = 0;
    theta = 0;
    phi = 0;
    r = 20;
    rtarget = 20;//确定
    
    targetname = -1;
    lasttargetname = -1;
    targetcolor = {0, 0, 0, 255};
    targetcloud = 0;
    totar = false;
    text = "";//确定
    
    scroll_y = 0;
    targetscroll_y = 0;
    messagex = 1.1*width;
    messagextarget = 1.1*width;
    
    messagewidthrate = 0.25;
    messageheightrate = 0.33;

    menutargetscolly = 0;
    menuscolly = 0;

    last_time = SDL_GetTicks();
    deltatime = 0;
    
    click_time = 0;
    deltaclicktime = 1;
    dragging = false;
    doubleclick = false;
    showmessage = false;

    showcircle = 0;
    showvertical = 0;
}


void StarMap::menu() {


    Uint32 current_time = SDL_GetTicks();
    Uint32 last_time = current_time;
    double deltatime = (current_time - last_time) / 1000.0;
    double startscroll_y=0;
    double menuy = 0;
    std::vector<Button> buttonofsaves;
    std::vector<int> savenumber;
    int i = 0;
    for (const auto& entry : fs::directory_iterator("save")) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();

            if (filename.size() > 13 && filename.substr(filename.size() - 13) == "star_data.txt") {
                std::string xValue = filename.substr(0, filename.size() - 13); 
                Button abutton;
                abutton=Button((0.2+0.4*(i%2) )* width, (0.4 +0.2*(i/2))* height, 0.2 * width, 0.1 * height, 0, { 255,127,0,255 }, "StarSave"+xValue);
                buttonofsaves.push_back(abutton);
                savenumber.push_back(std::stoi(xValue));
                i++;

            }
        }
    }
    Button startnew = Button(0.4 * width, 0.4 * height, 0.2 * width, 0.1 * height, 1, { 255,127,0,255 }, "NewGame");
    Button startold = Button(0.4 * width, 0.6 * height, 0.2 * width, 0.1 * height, 1, { 255,127,0,255 }, "OpenSave");
    Button returntostart= Button(0, 0, 0.1 * width, 0.1 * height, 0, { 255,127,0,255 }, "Return");
    while (running) {
        current_time = SDL_GetTicks();
        deltatime = (current_time - last_time) / 1000.0;
        last_time = current_time;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    width = event.window.data1;
                    height = event.window.data2;
                    SDL_RenderSetLogicalSize(renderer, width, height);
                    startnew.buttoncal(0.4 * width, 0.4 * height, 0.2 * width, 0.1 * height);
                    startold.buttoncal(0.4 * width, 0.6 * height, 0.2 * width, 0.1 * height);
                    returntostart.buttoncal(0, 0, 0.1 * width, 0.1 * height);
                }
            }
            else if (event.type == SDL_MOUSEWHEEL&&startold.state==1) {
                if (event.wheel.y > 0) {
                    if (startscroll_y < 0){
                        startscroll_y += static_cast<int>(height / 32.0);
                    }
                    else {
                        startscroll_y = 0;
                    }
                }
                else if (event.wheel.y < 0) {
                    startscroll_y -= static_cast<int>(height / 32.0);
                }
            }
        }
        menuy+= (startscroll_y - menuy) * variable_threshold1(18 * deltatime);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_Rect menu_rect = { 0,0 ,width, height };
        SDL_RenderCopy(renderer, menuback, nullptr, &menu_rect);


        if (startnew.buttoncal()) {
            run(0);
            startnew.state = 0;
            buttonofsaves.clear();
            savenumber.clear();
            i=0;
            for (const auto& entry : fs::directory_iterator("save")) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();

                    if (filename.size() > 13 && filename.substr(filename.size() - 13) == "star_data.txt") {
                        std::string xValue = filename.substr(0, filename.size() - 13);
                        Button abutton;
                        abutton = Button((0.2 + 0.4 * (i % 2)) * width, (0.4 + 0.2 * (i / 2)) * height, 0.2 * width, 0.1 * height, 0, { 255,127,0,255 }, "StarSave" + xValue);
                        buttonofsaves.push_back(abutton);
                        savenumber.push_back(std::stoi(xValue));
                        i++;
                    }
                }
            }
        }
        startold.buttoncal();
        if (startold.state==1) {
            startnew.buttoncal(0);
            startold.buttoncal(0);
            returntostart.buttoncal(1);
            //存档按钮显示，判断，开
            for (int j = 0; j < i; j++) {

                buttonofsaves[j].buttoncal((0.2 + 0.4 * (j % 2)) * width, (0.4 + 0.2 * (j / 2)) * height + menuy, 0.2 * width, 0.1 * height, 1);
                buttonofsaves[j].drawbutton(renderer);
                if (buttonofsaves[j].state) {
                    run(savenumber[j]);
                    buttonofsaves[j].state = 0;
                }
            }
            if (returntostart.buttoncal()) {
                startold.state = 0;
                returntostart.state = 0;
                startnew.buttoncal(1);
                startold.buttoncal(1);
                returntostart.buttoncal(0);
                //存档按钮不显示，状态归零
                for (int j = 0; j < i; j++) {
                    Button abutton = buttonofsaves[j];
                    abutton.buttoncal((0.2 + 0.4 * (j % 2)) * width, (0.4 + 0.2 * (j / 2)) * height + menuy, 0.2 * width, 0.1 * height, 0);

                }
            }
        }
        returntostart.drawbutton(renderer);
        startnew.drawbutton(renderer);
        startold.drawbutton(renderer);

        SDL_RenderPresent(renderer);
    }


}
  

void drawDashedLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, double dashLength) {//bug here

    double dx = x2 - x1;
    double dy = y2 - y1;
    double length = sqrt(dx * dx + dy * dy);

    // Calculate how many dashes are needed
    int numDashes = static_cast<int>(length / dashLength);
    if (numDashes >= 1000) {
        numDashes = 1000;
    }
    // Calculate dash components
    double dash_dx = dx / numDashes;
    double dash_dy = dy / numDashes;

    // Draw dashes
    for (int i = 0; i < numDashes; ++i) {

        // Calculate start and end points of each dash
        int dash_x1 = static_cast<int>(x1 + i * dash_dx);
        int dash_y1 = static_cast<int>(y1 + i * dash_dy);
        int dash_x2 = static_cast<int>(x1 + (i + 0.5) * dash_dx);
        int dash_y2 = static_cast<int>(y1 + (i + 0.5) * dash_dy);

        // Draw dash
        SDL_RenderDrawLine(renderer, dash_x1, dash_y1, dash_x2, dash_y2);
    }

    // Make sure the last dash reaches the endpoint of the line
   // SDL_RenderDrawLine(renderer, static_cast<int>(x1 + numDashes * dash_dx), static_cast<int>(y1 + numDashes * dash_dy), x2, y2);
}



void StarMap::init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
    }

    window = SDL_CreateWindow("Star Map", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);//创建窗口
    if (window == nullptr) {
        throw std::runtime_error("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));//renderer
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        throw std::runtime_error("Renderer could not be created! SDL_Error: " + std::string(SDL_GetError()));
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        throw std::runtime_error("SDL_image could not initialize! SDL_image Error: " + std::string(IMG_GetError()));
    }
}

void StarMap::load_textures() {
    menuback = load_texture("menuback.png");
    tieimg[0] = load_texture("bloom0.png");
    tieimg[1] = load_texture("bloom1.png");
    tieimg[2] = load_texture("bloom2.png");
    tieimg[3] = load_texture("bloom3.png");

    mesimg[0] = load_texture("stage0.png");
    mesimg[1] = load_texture("stage1.png");
    mesimg[2] = load_texture("stage2.png");
    mesimg[3] = load_texture("stage3.png");
    mesimg[4] = load_texture("stage4.png");
}

SDL_Texture* StarMap::load_texture(const std::string& path) {//图像加载，也许无误
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        throw std::runtime_error("Unable to load image " + path + "! SDL_image Error: " + IMG_GetError());
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (texture == nullptr) {
        throw std::runtime_error("Unable to create texture from " + path + "! SDL Error: " + SDL_GetError());
    }

    SDL_FreeSurface(loadedSurface);
    return texture;
}

bool StarMap::save(int number) {
    std::string filepath = "save/"+std::to_string(number) + "star_data.txt";
    if (number >= 1) {
        std::ofstream outFile(filepath, std::ios::out);
        if (!outFile) {
            std::cerr << "存档炸了" << std::endl;
        }
        for (auto& star : stars) {
            outFile << star.name << " " << star.absolute_pos.x << " " << star.absolute_pos.y << " " << star.absolute_pos.z << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " "
                << 0 << " " << 0 << " " << star.temperature << " " << star.power << " " << star.dysondensity << " " << star.radius << " " << star.teamname << " "
                << star.needtoshowpos << " " << 0 << " " << 0 << " " << 0 << " " << star.type << "\n";
        }
    }
    else if (number == 0) {
        bool aaaa = 1;
        number = 1;
        while (aaaa) {
            filepath = "save/" + std::to_string(number) + "star_data.txt";
            std::ifstream file(filepath);

            if (file.is_open()) {
                file.close();
                number += 1;

            }
            else if (!file.is_open()) {

                std::ofstream outFile(filepath, std::ios::out);
                if (!outFile) {
                    std::cerr << "存档炸了" << std::endl;
                }
                for (auto& star : stars) {
                    outFile << star.name << " " << star.absolute_pos.x << " " << star.absolute_pos.y << " " << star.absolute_pos.z << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " "
                        << 0 << " " << 0 << " " << star.temperature << " " << star.power << " " << star.dysondensity << " " << star.radius << " " << star.teamname << " "
                        << star.needtoshowpos << " " << 0 << " " << 0 << " " << 0 << " " << star.type << "\n";
                }
                aaaa = 0;
            }
        }
    }
    return 1;
}

void StarMap::generate_stars() {//生成，可能无误
    const int nstar0 = 3125;
    const double stardensity = 0.004;
    const double rmap0 = std::pow(nstar0 * 6 / (stardensity * PI), 1.0/3) / 2;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-rmap0, rmap0);
    std::uniform_real_distribution<> temp_dis(1000, 12000);
    std::uniform_real_distribution<> power_dis(0, 1);
    std::uniform_int_distribution<> type_dis(0, 4);
    std::uniform_real_distribution<> radius_dis(-3.3, -1);
    std::cout << std::ceil(nstar0 * 6 / PI) << std::endl;
    std::cout << rmap0 << std::endl;
    for (int i = 0; i < std::ceil(nstar0 * 6 / PI); ++i) {
        double xstar0 = dis(gen);
        double ystar0 = dis(gen);
        double zstar0 = dis(gen);
        bool aaa=false;
        if (xstar0*xstar0 + ystar0*ystar0 + zstar0*zstar0 < rmap0*rmap0) {
            std::string name = random_name();
            star_messages[i] = name + std::to_string(i) + "s introduction\ntestline, 114514w";
            if (power_dis(gen) > 0.9) {//测试垂线
                aaa = true;
            }
            Star astar = {
                i,
                Vec3(xstar0, ystar0, zstar0),
                Vec3(), 0, 0,{0, 0},
                temp_dis(gen),
                std::pow(2.71828, -6.9077 + 23 * power_dis(gen)),
                type_dis(gen),
                pow(10,radius_dis(gen)) * 0.000016,
                "fr",
                aaa,
                {0, 0},
                0,
                0
            };
            stars.push_back(astar);
        }
    }
    Star tstar = {
        114514,
        Vec3(0, 0, -10),
        Vec3(), 0,0, {0, 0},
        7000,
        std::pow(2.71828, -6.9077 + 23),
        0,
        0.01 * 0.000016,
        "fr",
        true,
        {0, 0},
        0,
        0
    };
    stars.push_back(tstar);
}

void StarMap::generate_nebula() {
    const int nstar0 = 2;
    const double rmap0 = 100;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-rmap0, rmap0);

    std::uniform_real_distribution<> radius_dis(0, 1);
    std::cout << std::ceil(nstar0 * 6 / PI) << std::endl;
    std::cout << rmap0 <<"a" << std::endl;
    for (int i = 0; i < std::ceil(nstar0 * 6 / PI); ++i) {
        double xstar0 = dis(gen);
        double ystar0 = dis(gen);
        double zstar0 = dis(gen);
        bool aaa = false;
        if (xstar0 * xstar0 + ystar0 * ystar0 + zstar0 * zstar0 < rmap0 * rmap0) {
            std::string name = random_name();
            star_messages[i] = name + std::to_string(i) + "s introduction\ntestline, 114514w";

            Star nebula = {
                1919810+i,
                Vec3(xstar0, ystar0, zstar0),
                Vec3(), 0, 0,{0, 0},
                0,
                0,
                -1,
                pow(10,radius_dis(gen)),
                "cl",
                0,
                {0, 0},
                0,
                1
            };
            stars.push_back(nebula);
        }
    }
}
void StarMap::generate_opoints() {
    for (double r = 1; r < 100; r *= 1.25892) {
        for (double the = 0; the < 2 * PI; the += 2 * PI / 100.0) {
            opoint apoint = {
                Vec3(r*std::cos(the), r * std::sin(the), 0),
                Vec3(), 0, 0,{0, 0}
            };
            opoints.push_back(apoint);
            
        }
    }
    for (double the = 0; the < 2 * PI; the += 2 * PI / 8) {
        for (int r = 0; r < 100; r += 1) {
            opoint apoint = {
                Vec3(r * std::cos(the), r * std::sin(the), 0),
                Vec3(), 0, 0,{0, 0}
            };
            opoints.push_back(apoint);
           
        }
    }

}
void StarMap::run(int number) {//主循环，可能完善
    if (number == 0) {
        generate_stars();
        generate_nebula();
    }
    else {
        read_stars(number);
    }
    while (running&& !ifsave) {

        

        handle_events();
        
        update();
        
        
        render();
        if (ifexit) {
            ifsave = save(number);
        }
    }
 
    cleanup();
}

void StarMap::read_stars(int number) {
    std::string filepath = "save/"+std::to_string(number) + "star_data.txt";
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filepath << std::endl;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Star star;

        iss >> star.name
            >> star.absolute_pos.x >> star.absolute_pos.y >> star.absolute_pos.z
            >> star.relative_pos.x >> star.relative_pos.y >> star.relative_pos.z
            >> star.distance >> star.depth
            >> star.screen_pos.x >> star.screen_pos.y
            >> star.temperature >> star.power
            >> star.dysondensity
            >> star.radius
            >> star.teamname
            >> star.needtoshowpos
            >> star.zpoint.x >> star.zpoint.y
            >> star.zpdep
            >> star.type;

        stars.push_back(star);
    }

    file.close();
}

void StarMap::handle_events() {//鼠标事件，可能完善

    bool aa = !menubutton.buttoncal(0, 0, 0.1 * width, 0.1 * height, 1) && !menubutton.mouson;

    doubleclick = false;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                width = event.window.data1;
                height = event.window.data2;
                SDL_RenderSetLogicalSize(renderer, width, height);
            }
        }
        else if (event.type == SDL_MOUSEWHEEL ) {
            handle_mouse_wheel(event);
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN && aa) {
            handle_mouse_button_down(event);
        }
        else if (event.type == SDL_MOUSEBUTTONUP && aa) {
            handle_mouse_button_up(event);
        }
        else if (event.type == SDL_MOUSEMOTION && !menubutton.buttoncal(0, 0, 0.1 * width, 0.1 * height, 1)) {
            handle_mouse_motion(event);
        }
    }

}

void StarMap::handle_mouse_wheel(SDL_Event& event) {//页面内摄影机距离和页面外侧边栏滚动

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if (!menubutton.buttoncal(0, 0, 0.1 * width, 0.1 * height, 1) && !menubutton.mouson) {
        if (!showmessage || (showmessage && double(mouseX) / width < (1 - messagewidthrate))) {
            if (event.wheel.y > 0) {
                rtarget /= 1.2;
            }
            else if (event.wheel.y < 0) {
                rtarget *= 1.2;
            }
        }
        else if (showmessage && double(mouseX) / width > (1 - messagewidthrate)) {
            if (event.wheel.y > 0) {
                if (targetscroll_y < 0) {
                    targetscroll_y += static_cast<int>(height / 32.0);
                }
                else {
                    targetscroll_y = 0;
                }
            }
            else if (event.wheel.y < 0) {
                targetscroll_y -= static_cast<int>(height / 32.0);
            }
        }
    }
    else if (menubutton.buttoncal(0, 0, 0.1 * width, 0.1 * height, 1)) {
        if (event.wheel.y > 0) {
            if (menutargetscolly < 0) {
                menutargetscolly += static_cast<int>(height / 32.0);
            }
            else {
                menutargetscolly = 0;
            }
        }
        else if (event.wheel.y < 0) {
            menutargetscolly -= static_cast<int>(height / 32.0);
        }
    }
}

void StarMap::handle_mouse_button_down(SDL_Event& event) {//左键点击
    if (event.button.button == SDL_BUTTON_LEFT) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        if (!showmessage || (showmessage && mouseX / static_cast<double>(width) < (1 - messagewidthrate))) {
            dragging = true;
            offset_x = mouseX - posx;
            offset_y = mouseY - posy;
            
            Uint32 current_time = SDL_GetTicks();
            deltaclicktime = (current_time - click_time) / 1000.0;
            click_time = current_time;
            doubleclick = false;
            if (deltaclicktime < 0.3){
                doubleclick = true;
            }
            //std::cout<<doubleclick<<std::endl;
        }
    }
}

void StarMap::handle_mouse_button_up(SDL_Event& event) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        dragging = false;
    }
}

void StarMap::handle_mouse_motion(SDL_Event& event) {
    if (dragging) {
        posx = event.motion.x - offset_x;
        posy = event.motion.y - offset_y;
    }
}

void StarMap::update() {//已完善
    Uint32 current_time = SDL_GetTicks();
    deltatime = (current_time - last_time) / 1000.0;
    last_time = current_time;
    std::cout << 1 / deltatime << std::endl;
    
    update_camera();
    update_stars();
    if (showcircle == 1) {
        update_opoints();
    }
    // 更新消息面板
    if (showmessage) {
        messagextarget = width * (1 - messagewidthrate);
    } else {
        messagextarget = 1.1*width;
    }
    messagex += (messagextarget - messagex) * variable_threshold1( 18 * deltatime);

    // 更新滚动
    scroll_y += (targetscroll_y - scroll_y) * variable_threshold1(18 * deltatime);
    if (menubutton.buttoncal(0, 0, 0.1 * width, 0.1 * height, 1)) {
        menuscolly += (menutargetscolly - menuscolly) * variable_threshold1(18 * deltatime);
    }

    // 更新相机距离
    r += (rtarget - r) * variable_threshold1(9.6 * deltatime);
    //std::cout << messagex<< std::endl;
}

void StarMap::update_camera() {
    // 更新相机位置和方向,可能完善
    if (posx > width) {
        posx -= width;
    }
    else if (posx < 0) {
        posx += width;
    }
    if (posy > height) {
        posy = height;
    }
    else if (posy < 0) {
        posy = 0;
    }
    theta = posx / width * 4 * PI;
    phi = (posy / height - 0.5) * PI;
    
    reposcam.x = r * std::cos(phi) * std::cos(theta);
    reposcam.y = r * std::cos(phi) * std::sin(theta);
    reposcam.z = r * std::sin(phi);
    poscam = add_vectors(cenposcam, reposcam);
    Vec3 vectoz = { 0,0,1 };
    vecx = normalize_vector(cross_product(vectoz, reposcam));
    vecy = normalize_vector(cross_product(reposcam, vecx));
    vecz = normalize_vector(reposcam);
    //std::cout << vecx.x << "-----" << vecx.y << "-----" << vecx.z << std::endl;
   
    if (totar == true) {

        cenposcam = add_vectors(cenposcam, scalar_multiply(variable_threshold1(0.05 * 60 * deltatime), (subtract_vectors(target, cenposcam))));
        circen = add_vectors(circen, scalar_multiply(variable_threshold1(0.05 * 120 * deltatime), (subtract_vectors(target, circen))));
        if (vector_length(subtract_vectors(target, cenposcam)) < pow(10, -14)) {
            totar = false;
        }
    }
}

void StarMap::sortsatrsbydistance() {//远近关系，可能无误
    std::sort(stars.begin(), stars.end(), [](const Star& a, const Star& b) {
        return a.distance > b.distance;
        });
}


void StarMap::update_stars() {//右键边栏目标确定，双击移动目标确定，可能完善
    sortsatrsbydistance();
    int nearstars = 0;
    int mouseX, mouseY;
    const Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);
    //double maxr = 0;
    for (auto& star : stars) {
        //maxr = std::max(maxr,std::sqrt(pow(star.absolute_pos.x,2)+ pow(star.absolute_pos.y, 2)));
        Vec3 relative_pos = subtract_vectors(star.absolute_pos, poscam);
        star.relative_pos = relative_pos;
        star.distance = vector_length(relative_pos);
        //std::cout << star.distance << std::endl;
        double projx = dot_product(relative_pos, vecx);
        double projy = dot_product(relative_pos, vecy);
        double projz = dot_product(relative_pos, vecz);
        double starrad = width / std::sqrt(variable_threshold001(pow((star.distance / star.radius), 2) - 1));
        double ll = 50 / pow(variable_threshold001(10000 * (star.distance)) / 10000, 0.8) * pow(star.power, 1 / 7.0);
        if (ll > 1024) {
            ll = 1024;
        }

        double screenx = width * (0.5 - 0.5 * projx / projz);
        double screeny = height * (0.5 - 0.5 * width / double(height) * projy / projz);
        star.screen_pos = { static_cast<int>(screenx), static_cast<int>(screeny) };
        star.depth = -projz;
        if (star.needtoshowpos == true) {
            Vec3 zpos;
            zpos.x = star.absolute_pos.x;
            zpos.y = star.absolute_pos.y;
            zpos.z = circen.z;
            Vec3 zrelative_pos = subtract_vectors(zpos, poscam);
            double zx = dot_product(zrelative_pos, vecx);
            double zy = dot_product(zrelative_pos, vecy);
            double zz = dot_product(zrelative_pos, vecz);
            double zscreenx = width * (0.5 - 0.5 * zx / zz);
            double zscreeny = height * (0.5 - 0.5 * width / double(height) * zy / zz);
            star.zpoint = { static_cast<int>(zscreenx), static_cast<int>(zscreeny) };
            star.zpdep = -zz;
        }

        if ((buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) && (mouseX / static_cast<double>(width) < (1 - messagewidthrate)) && star.type == 0&& !menubutton.buttoncal(0, 0, 0.1 * width, 0.1 * height, 1) && !menubutton.mouson) {
            if ((mouseX - screenx) * (mouseX - screenx) + (mouseY - screeny) * (mouseY - screeny) < pow((std::max(ll / 8, starrad)), 2)) {
                nearstars++;
                showmessage = true;
                targetname = star.name;
                targetcolor = kelvin_to_rgb(star.temperature);
                targetcloud = star.dysondensity;
            }
            if (nearstars == 0) {
                showmessage = false;
            }
        }
        if (doubleclick == true) {
            if ((mouseX - screenx) * (mouseX - screenx) + (mouseY - screeny) * (mouseY - screeny) < pow((std::max(ll / 8, starrad)), 2) && star.type == 0) {
                totar = true;
                target = star.absolute_pos;
            }
        }



    }

    

}

void StarMap::update_opoints() {
    for (auto& opoint : opoints) {
        Vec3 relative_pos = add_vectors(subtract_vectors(opoint.absolute_pos, poscam),circen);
        opoint.relative_pos = relative_pos;
        opoint.distance = vector_length(relative_pos);
        //std::cout << star.distance << std::endl;
        double projx = dot_product(relative_pos, vecx);
        double projy = dot_product(relative_pos, vecy);
        double projz = dot_product(relative_pos, vecz);

        double screenx = width * (0.5 - 0.5 * projx / projz);
        double screeny = height * (0.5 - 0.5 * width / double(height) * projy / projz);
        opoint.screen_pos = { static_cast<int>(screenx), static_cast<int>(screeny) };
        opoint.depth = -projz;
    }
}


void StarMap::render() {
    static bool lastopen;
    static bool open;
    open = menubutton.buttoncal(0, 0, 0.1 * width, 0.1 * height, 1);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);



    SDL_RenderClear(renderer);


    if (showcircle == 1) {
        draw_opoints();
    }
    draw_stars();
    draw_info_panel();


    if (open) {
        SDL_Rect backgroundrect = { 0,0,width,height };

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_RenderFillRect(renderer, &backgroundrect);
        showcircle = coordinate.buttoncal(0.2 * width, 0.3 * height + menuscolly, 0.2 * width, 0.07 * height, 1);
        ifexit = exitbutton.buttoncal(0.6 * width, 0.3 * height + menuscolly, 0.2 * width, 0.07 * height, 1);
        showvertical = showcircle;
    }
    if (!open) {
        coordinate.buttoncal(0.2 * width, 0.3 * height + menuscolly, 0.2 * width, 0.07 * height, 0);
        exitbutton.buttoncal(0.6 * width, 0.3 * height + menuscolly, 0.2 * width, 0.07 * height, 0);
    }
    menubutton.drawbutton(renderer);
    coordinate.drawbutton(renderer);
    exitbutton.drawbutton(renderer);
    lastopen = open;
    SDL_RenderPresent(renderer);
}

void  StarMap::renderTextureWithColor(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Color color, SDL_Rect destRect) {
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND); 
    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, color.a);
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
}



void StarMap::draw_stars() {
    for (auto& star : stars) {
        
        if (star.screen_pos.x >= 0 && star.screen_pos.x < width &&
            star.screen_pos.y >= 0 && star.screen_pos.y < height && star.depth>0&&star.type==0) {
            SDL_Color color = ScaleSDLColor(pow(star.power, 1.0 / 8.0) / 8.0 / (variable_threshold001(100 * (star.distance)) / 100), kelvin_to_rgb(star.temperature));
            SDL_Color color0 = kelvin_to_rgb(star.temperature);
            double  starrad = width / sqrt(variable_threshold001(pow((star.distance / star.radius), 2) - 1));
            double ll = 50 / pow(variable_threshold001(10000 * (star.distance)) / 10000, 0.8) * pow(star.power, 1 / 7.0);

            if (starrad >= 1) {
                drawFilledCircle(renderer, star.screen_pos.x, star.screen_pos.y, int(starrad), color0); //恒星本体，原色
            }
            else {
                drawFilledCircle(renderer, star.screen_pos.x, star.screen_pos.y, 1, ScaleSDLColor(variable_threshold1(ll), color0));//处理过远不显示的情况
            }
            SDL_Color AA = { 0, 0, 0, 255 };

            //星星
            //星芒
            SDL_Texture* modified_image = tieimg[3];

            if (ll > 1024) {
                ll = 1024;
            }
            if (ll > 128) {
                modified_image = tieimg[0];
            }
            else if (ll > 64) {
                modified_image = tieimg[1];
            }
            else if (ll > 32) {
                modified_image = tieimg[2];
            }
            else if (ll <= 32) {
                modified_image = tieimg[3];
            }

            AA.r = color0.r;
            AA.g = color0.g;
            AA.b = color0.b;
            AA.a = static_cast<uint8_t>(255 * 2 * (0.5 - 0.5 * tanh(0.05 * log1p(starrad))));


            if (ll > 1) {
                SDL_Rect dest_rect = { int(star.screen_pos.x - ll / 2 + 1), int(star.screen_pos.y - ll / 2 + 1), int(ll), int(ll) };
                renderTextureWithColor(renderer, modified_image, AA, dest_rect);



            }
        }
        if ((star.depth > 0 || star.zpdep > 0) && star.needtoshowpos == true && star.type == 0 && showvertical==1) {
            if (star.depth > 0 && star.zpdep < 0) {
                star.zpoint.x = star.screen_pos.x - (star.zpoint.x - star.screen_pos.x) * double(star.depth - star.zpdep) / variable_threshold00(double(star.depth));
                star.zpoint.y = star.screen_pos.y - (star.zpoint.y - star.screen_pos.y) * double(star.depth - star.zpdep) / variable_threshold00(double(star.depth));
            }
            if (star.depth < 0 && star.zpdep > 0) {
                star.screen_pos.x = star.zpoint.x - (star.screen_pos.x - star.zpoint.x) * double(star.zpdep - star.depth) / variable_threshold00((star.zpdep));
                star.screen_pos.y = star.zpoint.y - (star.screen_pos.y - star.zpoint.y) * double(star.zpdep - star.depth) / variable_threshold00((star.zpdep));
            }
            if ((circen.z - poscam.z) / (circen.z - star.absolute_pos.z) >= 0) {
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                SDL_RenderDrawLine(renderer, star.screen_pos.x, star.screen_pos.y, star.zpoint.x, star.zpoint.y);
            }
            else if ((circen.z - poscam.z) /(circen.z - star.absolute_pos.z) < 0) {
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                drawDashedLine(renderer, star.screen_pos.x, star.screen_pos.y, star.zpoint.x, star.zpoint.y, 10 );
            }
        }

        if (star.type == 1 && star.depth > 0) {
            double ll = std::min((width / sqrt(variable_threshold00(pow((star.distance / star.radius), 2) - 1))), double(2 * width));
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD );
            drawFilledCircle(renderer, star.screen_pos.x, star.screen_pos.y, ll, { 200,0,200,50 });
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


        }
        
    }
}

void StarMap::draw_opoints() {
    
    for (size_t i = 1; i < opoints.size(); i++) {
        opoint tpoint = opoints[i];
        opoint lpoint = opoints[i-1];
        
        if ((tpoint.screen_pos.x >= 0 && tpoint.screen_pos.x < width &&
            tpoint.screen_pos.y >= 0 && tpoint.screen_pos.y < height && tpoint.depth>0) && (lpoint.screen_pos.x >= 0 && lpoint.screen_pos.x < width &&
                lpoint.screen_pos.y >= 0 && lpoint.screen_pos.y < height && lpoint.depth>0)) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderDrawLine(renderer, lpoint.screen_pos.x, lpoint.screen_pos.y, tpoint.screen_pos.x, tpoint.screen_pos.y);
            
        }
    }
}

std::vector<std::string> StarMap::split_string(const std::string& s, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}
void StarMap::draw_info_panel() {//报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
    
    if ( messagex<=width) {
        // 绘制信息面板背景
        SDL_Rect message_rect = { static_cast<int>(messagex), 0,
                                 static_cast<int>(width * messagewidthrate), height };
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderFillRect(renderer, &message_rect);



        // 绘制星球图像

        int image_size = static_cast<int>(width * messagewidthrate );
        SDL_Rect image_rect = { static_cast<int>(messagex),
            0 + scroll_y, image_size, image_size*2/3.0 };
        renderTextureWithColor(renderer, mesimg[targetcloud], targetcolor, image_rect);

        SDL_RenderCopy(renderer, mesimg[targetcloud], nullptr, &image_rect);

        // 信息配置
        std::string aname = std::to_string(targetname);
        SDL_Color text_color = { 255, 255, 255, 255 };
        TTF_Font* font = TTF_OpenFont("file-deletion.ttf", height / 32.0);
        SDL_Surface* surface = TTF_RenderText_Blended(font, aname.c_str(), text_color);//函数未定义
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        int text_width = surface->w;//单行字尺寸
        int text_height = surface->h;
        SDL_FreeSurface(surface);
        
        SDL_Rect text_rect = { static_cast<int>(messagex + 10), image_size * 2 / 3.0 + scroll_y, text_width, text_height };
        SDL_RenderCopy(renderer, texture, nullptr, &text_rect);
        SDL_DestroyTexture(texture);

        // 绘制详细信息
        std::vector<std::string> info_lines = split_string(star_messages[targetname], '\n');
        int y_offset = text_height + 30 + image_size * 2 / 3.0 + scroll_y;
        for (const auto& line : info_lines) {
            
            surface = TTF_RenderUTF8_Blended(font, line.c_str(), text_color);
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            text_width = surface->w;
            text_height = surface->h;
            SDL_FreeSurface(surface);

            text_rect = { static_cast<int>(messagex + 10), y_offset, text_width, text_height };
            SDL_RenderCopy(renderer, texture, nullptr, &text_rect);
            SDL_DestroyTexture(texture);

            y_offset += text_height + 5;
        }
        TTF_CloseFont(font);
    }
    lasttargetname = targetname;
}

void StarMap::cleanup() {
    exitbutton.state = 0;
    menubutton.state = 0;
    ifexit = 0;
    ifsave = 0;
    stars.clear();
    stars.shrink_to_fit();
    star_messages.clear();

    cenposcam = Vec3(0, 0, 0);//对应11111
    reposcam = Vec3(0, 0, 0);
    poscam = Vec3(0, 0, 0);
    vecx = Vec3(0, 0, 0);
    vecy = Vec3(0, 0, 0);
    vecz = Vec3(0, 0, 0);
    target = Vec3(0, 0, 0);
    circen = Vec3(0, 0, 0);

    offset_x = 0;
    offset_y = 0;
    posx = 0;
    posy = height;
    mposx = 0;
    mposy = 0;
    theta = 0;
    phi = 0;
    r = 20;
    rtarget = 20;//确定

    targetname = -1;
    lasttargetname = -1;
    targetcolor = { 0, 0, 0, 255 };
    targetcloud = 0;
    totar = false;
    text = "";//确定

    scroll_y = 0;
    targetscroll_y = 0;
    messagex = 1.1 * width;
    messagextarget = 1.1 * width;

    messagewidthrate = 0.25;
    messageheightrate = 0.33;

    menutargetscolly = 0;
    menuscolly = 0;

    last_time = SDL_GetTicks();
    deltatime = 0;

    click_time = 0;
    deltaclicktime = 1;
    dragging = false;
    doubleclick = false;
    showmessage = false;

    showcircle = 0;
    showvertical = 0;
   /* for (auto& texture : tieimg) {
        SDL_DestroyTexture(texture);
    }
    for (auto& texture : mesimg) {
        SDL_DestroyTexture(texture);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();*/
}


int main(int argc, char* args[]) {
    try {
        StarMap star_map(1000, 600);
        star_map.menu();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}