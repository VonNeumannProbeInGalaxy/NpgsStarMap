#include <cmath>
#include <cstdio>

#include <algorithm>
#include <vector>
#include <random>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#define NOMINMAX

#include "object/Objects.h"
#include "Vec3/Vec3.h"
#include "UI/Button.h"
#include "UI/InputBox.h"
#include "UI/HoverMessage.h"
#include "TooolfuncForStarMap.h"

namespace fs = std::filesystem;
//using namespace std;
const double PI = 3.141592653589793238;

using json = nlohmann::json;



#pragma region StarMap
//————————————————————————————————————————————————————————————————————————————————————————————————————
class StarMap {//屏幕各项数据，声明与初始化可能有遗漏
public:
	StarMap(int width, int height);

	void run(int number);
	void menu();
	int width, height;
private:
	long double timeingame;
	double timerate;
	SDL_Window* window;
	SDL_Renderer* renderer;

	bool running;
	bool ifexit;
	bool ifsave;

	std::vector<Star> stars;
	std::vector<Route> routes;

	std::vector<Opoint> opoints;
	std::unordered_map<int, std::string> star_messages;

	UI::Button menubutton;
	UI::Button coordinate;
	UI::Button exitbutton;
	UI::Button addtimerate;
	UI::Button subtimerate;
	UI::Button stop;
	UI::Button ShipAndRKKV;

	UI::HoverMessage hovermessage;

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

	void openold(int n);

	void generate_stars(double n);
	void generate_nebula();
	bool read(int number);
	void generate_opoints();
	void handle_events();
	void update();
	void render();
	bool save(int number);
	json savej();
	void cleanup();


	void handle_mouse_wheel(SDL_Event& event);
	void handle_mouse_button_down(SDL_Event& event);
	void handle_mouse_button_up(SDL_Event& event);
	void handle_mouse_motion(SDL_Event& event);

	void update_camera();
	void sortsatrsbydistance();
	void update_stars();
	void update_opoints();
	void updata_ship();

	void add_ship_into_route(int number1, int number2, int numberofship, int cat, bool dir, double v, double m1, double m2, double m3);

	void renderTextureWithColor(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Color color, SDL_Rect destRect);
	void draw_stars();
	void draw_ships();
	void draw_opoints();
	void draw_info_panel();
	void draw_time(double atime);


	SDL_Texture* load_texture(const std::string& path);
};

