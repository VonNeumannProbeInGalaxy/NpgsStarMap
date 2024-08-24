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
#include "Utils/Vec3.h"
#include "UI/Button.h"
#include "UI/InputBox.h"
#include "UI/HoverMessage.h"
#include <UI/HoverText.h>
#include "Utils/TooolfuncForStarMap.h"

namespace fs = std::filesystem;
//using namespace std;

using json = nlohmann::json;

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
	int month;
	int lastmonth;
	SDL_Window* window;
	SDL_Renderer* renderer;

	int mousestorey;

	bool running;
	bool ifexit;
	bool ifsave;

	std::vector<StarVe> stars;
	std::vector<Route> routes;

	std::vector<Opoint> opoints;
	std::vector<Parameterofship> shipwaittotakeof;
	std::unordered_map<int, std::string> star_messages;

	UI::Button menubutton;
	UI::Button coordinate;
	UI::Button exitbutton;
	UI::Button addtimerate;
	UI::Button subtimerate;
	UI::Button stop;
	UI::Button ShipAndRKKV;
	UI::Button orbit;
	UI::Button control;
	UI::Button sendrkkv;
	UI::Button sendship;
	UI::Button sendlaser;

	UI::HoverMessage hovermessage;
	UI::HoverText showtimeingame;
	UI::HoverText showtimerate;
	UI::HoverText detailed;

	Vec3 cenposcam;
	Vec3 reposcam;
	Vec3 poscam;
	Vec3 vecx, vecy, vecz;
    double offset_x;
	double offset_y;
	double posx, posy;
	double mposx, mposy;
	double theta, phi;
	double zvectheta, zvecphi;
	double cirtheta, cirphi;
	double r, rtarget;

	Vec3 circen;

	StarVe targetstar;
	bool totar;
	StarVe controlstar;
	bool tocontrol;

	StarVe rightclickstar;
	int lastrightname;
	PlaneT rightclickplanet;
	bool planetmessage;

	std::string text;//确定



	Vec3 targetplanet;
	Vec3 lasttargetplanet;
	int followplanet;
	int lastfollowplanet;
	bool toplanet;
	bool onplanet;

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
	bool showorbit;

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
	void update_and_draw_stars();
	void update_opoints();
	void shiptakeof();

	void add_ship_into_route(int number1, int number2, int numberofship, int cat, bool dir, double v, double m1, double m2, double m3,int pnum=-1);

	void renderTextureWithColor(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Color color, SDL_Rect destRect);
	void updata_and_draw_ships();
	void draw_opoints();
	void draw_info_panel();
	void draw_time(double atime);


	SDL_Texture* load_texture(const std::string& path);
};

