#include "StarMap.h"

StarMap::StarMap(int w, int h) : width(w), height(h), running(true) {//屏幕初始化

	init_SDL();
	TTF_Init();
	load_textures();
	generate_opoints();

	hovermessage = UI::HoverMessage();
	menubutton = UI::Button(0, 0, 100, 100, 1, { 100,100,100 ,255 }, "",kTextFontFilename,"menu.png");
	coordinate = UI::Button(0, 0, 100, 100, 0, { 100,100,100 ,255 }, "", kTextFontFilename,"coordinate.png");
	exitbutton = UI::Button(0, 0, 100, 100, 0, { 100,100,100 ,255 }, "exit&save");
	addtimerate = UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "", kTextFontFilename, "speedup.png");
	subtimerate = UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "", kTextFontFilename, "slowdown.png");
	ShipAndRKKV = UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "", kTextFontFilename, "track.png");
	orbit = UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "", kTextFontFilename, "orbit.png");
	orbit._bPressState = 1;
	stop = UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "", kTextFontFilename,"pause.png");
	stop._bPressState = 1;
	sendrkkv= UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "", kTextFontFilename, "rkkv0.png");
	sendship = UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "", kTextFontFilename, "ship0.png");
	sendlaser = UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "", kTextFontFilename, "laser0.png");
	control = UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "", kTextFontFilename, "goto.png");
	timeingame = 0;
	timerate = 1;
	month = 0;
	lastmonth = 0;
	mousestorey = 0;

	ifexit = 0;
	ifsave = 0;

	cenposcam = Vec3(0, 0, 0);//对应11111
	reposcam = Vec3(0, 0, 0);
	poscam = Vec3(0, 0, 0);
	vecx = Vec3(0, 0, 0);
	vecy = Vec3(0, 0, 0);
	vecz = Vec3(0, 0, 0);
	circen = Vec3(0, 0, 0);

	offset_x = 0;
	offset_y = 0;
	posx = 0;
	posy = height;
	mposx = 0;
	mposy = 0;
	theta = 0;
	phi = 0;
	zvectheta = 0;
	zvecphi = 0;
	cirtheta = 1;
	cirphi = 1;
	r = 20;
	rtarget = 20;//确定


	lastrightname = -1;
	totar = false;
	tocontrol = false;
	text = "";//确定

	toplanet = 0;
	onplanet = 0;

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
	showorbit = true;

	showcircle = 0;
	showvertical = 0;
}


void StarMap::menu() {


	Uint32 current_time = SDL_GetTicks();
	Uint32 last_time = current_time;
	double deltatime = (current_time - last_time) / 1000.0;
	double startscroll_y = 0;//存档页面滚动目标
	double menuy = 0;//存档页面滚动逼近
	std::vector<UI::Button> buttonofsaves;
	std::vector<int> savenumber;
	int i = 0;
	for (const auto& entry : fs::directory_iterator("save")) {//识别存档加入按钮序列
		if (entry.is_regular_file()) {
			std::string filename = entry.path().filename().string();

			if (filename.size() > 14 && filename.substr(filename.size() - 14) == "star_data.json") {
				std::string xValue = filename.substr(0, filename.size() - 14);
				UI::Button abutton;
				abutton = UI::Button((0.2 + 0.4 * (i % 2)) * width, (0.4 + 0.2 * (i / 2)) * height, 0.2 * width, 0.1 * height, 0, { 255,127,0,255 }, "StarSave" + xValue);
				buttonofsaves.push_back(abutton);
				savenumber.push_back(std::stoi(xValue));
				i++;

			}
		}
	}
	UI::Button startnew = UI::Button(0.04 * width, 0.85 * height, 0.2 * width, 0.1 * height, 1, { 255,127,0,255 }, "NewUniverse");
	UI::Button startold = UI::Button(0.28 * width, 0.85 * height, 0.2 * width, 0.1 * height, 1, { 255,127,0,255 }, "OldUniverse");
	UI::Button setting = UI::Button(0.52 * width, 0.85 * height, 0.2 * width, 0.1 * height, 1, { 255,127,0,255 }, "Setting        ");
	UI::Button quitgame = UI::Button(0.76 * width, 0.85 * height, 0.2 * width, 0.1 * height, 1, { 255,127,0,255 }, "Exit             ");
	UI::Button returntostart = UI::Button(0, 0, 0.1 * width, 0.1 * height, 0, { 255,127,0,255 }, "Return");
	UI::Button createuni = UI::Button(0, 0, 0.1 * width, 0.1 * height, 0, { 255,127,0,255 }, "Create Universe");
	UI::InputBox starnum = UI::InputBox(0.4 * width, 0.7 * height + menuy, 0.2 * width, 0.1 * height, 0, "num of star:");
	starnum._InputText = "5000";
	while (running) {
		current_time = SDL_GetTicks();
		deltatime = (current_time - last_time) / 1000.0;
		last_time = current_time;
		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				running = false;
			} else if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {//更新按钮位置
					width = event.window.data1;
					height = event.window.data2;
					SDL_RenderSetLogicalSize(renderer, width, height);
					startnew.ProcessEvent(0.04 * width, 0.85 * height, 0.2 * width, 0.1 * height);
					startold.ProcessEvent(0.28 * width, 0.85 * height, 0.2 * width, 0.1 * height);
					quitgame.ProcessEvent(0.76 * width, 0.85 * height, 0.2 * width, 0.1 * height);
					setting.ProcessEvent(0.52 * width, 0.85 * height, 0.2 * width, 0.1 * height);
					returntostart.ProcessEvent(0, 0, 0.1 * width, 0.1 * height);
					starnum.ProcessEvent(0.4 * width, 0.7 * height + menuy, 0.2 * width, 0.1 * height);
				}
			} else if (event.type == SDL_MOUSEWHEEL && (startold._bPressState == 1 || startnew._bPressState == 1)) {//存档列滚动
				if (event.wheel.y > 0) {
					if (startscroll_y < 0) {
						startscroll_y += static_cast<int>(height / 32.0);
					} else {
						startscroll_y = 0;
					}
				} else if (event.wheel.y < 0) {
					startscroll_y -= static_cast<int>(height / 32.0);
				}
			} else if ((startold._bPressState == 0 && startnew._bPressState == 0)) {//存档列滚动
				startscroll_y = 0;
				menuy = 0;
			}
		}
		menuy += (startscroll_y - menuy) * variable_threshold1(18 * deltatime);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_Rect menu_rect = { 0,0 ,width, height };
		SDL_RenderCopy(renderer, menuback, nullptr, &menu_rect);

		if (!startnew._bToShow) {
			SDL_Rect backgroundrect = { 0,0,width,height };

			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
			SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

			SDL_RenderFillRect(renderer, &backgroundrect);
		}

		startnew.ProcessEvent();//new档
		if (startnew._bPressState == 1) {
			startnew._bMouseOnButton = 0;
			startnew.ProcessEvent(0);
			startold.ProcessEvent(0);
			quitgame.ProcessEvent(0);
			setting.ProcessEvent(0);
			returntostart.ProcessEvent(1);
			createuni.ProcessEvent(0.4 * width, 0.5 * height + menuy, 0.2 * width, 0.1 * height, 1);
			starnum.ProcessEvent(0.4 * width, 0.7 * height + menuy, 0.2 * width, 0.1 * height, 1);
			//存档按钮显示，判断，开
			if (createuni.ProcessEvent()) {
				createuni._bMouseOnButton = 0;
				generate_stars(std::stoi(starnum._InputText));
				run(0);
				createuni._bPressState = 0;
				startnew._bPressState = 0;
				returntostart._bPressState = 0;
				startnew.ProcessEvent(1);
				startold.ProcessEvent(1);
				setting.ProcessEvent(1);
				quitgame.ProcessEvent(1);
				returntostart.ProcessEvent(0);//退出存档后操作
				createuni.ProcessEvent(0);
				starnum.ProcessEvent(0);
				buttonofsaves.clear();
				savenumber.clear();
				i = 0;
				for (const auto& entry : fs::directory_iterator("save")) {
					if (entry.is_regular_file()) {
						std::string filename = entry.path().filename().string();

						if (filename.size() > 14 && filename.substr(filename.size() - 14) == "star_data.json") {
							std::string xValue = filename.substr(0, filename.size() - 14);
							UI::Button abutton;
							abutton = UI::Button((0.2 + 0.4 * (i % 2)) * width, (0.1 + 0.2 * (i / 2)) * height, 0.2 * width, 0.1 * height, 0, { 255,127,0,255 }, "StarSave" + xValue);
							buttonofsaves.push_back(abutton);
							savenumber.push_back(std::stoi(xValue));
							i++;
						}
					}
				}
			}
			if (returntostart.ProcessEvent()) {
				startnew._bPressState = 0;
				returntostart._bPressState = 0;
				startnew.ProcessEvent(1);
				startold.ProcessEvent(1);
				setting.ProcessEvent(1);
				quitgame.ProcessEvent(1);
				returntostart.ProcessEvent(0);
				createuni.ProcessEvent(0);
				starnum.ProcessEvent(0);


			}
		}

		startold.ProcessEvent();//旧档
		if (startold._bPressState == 1) {
			startold._bMouseOnButton = 0;
			startnew.ProcessEvent(0);
			startold.ProcessEvent(0);
			quitgame.ProcessEvent(0);
			setting.ProcessEvent(0);
			returntostart.ProcessEvent(1);
			//存档按钮显示，判断，开
			for (int j = 0; j < i; j++) {

				buttonofsaves[j].ProcessEvent((0.2 + 0.4 * (j % 2)) * width, (0.1 + 0.2 * (j / 2)) * height + menuy, 0.2 * width, 0.1 * height, 1);
				buttonofsaves[j].DrawButtonRelease_TextLeftAndLineDown(renderer);
				if (buttonofsaves[j]._bPressState) {
					openold(savenumber[j]);
					buttonofsaves[j]._bPressState = 0;
					buttonofsaves.clear();
					savenumber.clear();
					i = 0;
					for (const auto& entry : fs::directory_iterator("save")) {
						if (entry.is_regular_file()) {
							std::string filename = entry.path().filename().string();

							if (filename.size() > 14 && filename.substr(filename.size() - 14) == "star_data.json") {
								std::string xValue = filename.substr(0, filename.size() - 14);
								UI::Button abutton;
								abutton = UI::Button((0.2 + 0.4 * (i % 2)) * width, (0.1 + 0.2 * (i / 2)) * height, 0.2 * width, 0.1 * height, 0, { 255,127,0,255 }, "StarSave" + xValue);
								buttonofsaves.push_back(abutton);
								savenumber.push_back(std::stoi(xValue));
								i++;
							}
						}
					}
				}
			}
			if (returntostart.ProcessEvent()) {
				startold._bPressState = 0;
				returntostart._bPressState = 0;
				startnew.ProcessEvent(1);
				startold.ProcessEvent(1);
				setting.ProcessEvent(1);
				quitgame.ProcessEvent(1);
				returntostart.ProcessEvent(0);
				//存档按钮不显示，状态归零
				for (int j = 0; j < i; j++) {
					UI::Button abutton = buttonofsaves[j];
					abutton.ProcessEvent((0.2 + 0.4 * (j % 2)) * width, (0.1 + 0.2 * (j / 2)) * height + menuy, 0.2 * width, 0.1 * height, 0);

				}
			}
		}
		if (quitgame.ProcessEvent() == 1) {
			running = false;
		}
		returntostart.DrawButtonRelease_TextLeftAndLineDown(renderer);

		startnew.DrawButtonRelease_TextLeftAndLineDown(renderer);

		startold.DrawButtonRelease_TextLeftAndLineDown(renderer);
		setting.DrawButtonRelease_TextLeftAndLineDown(renderer);
		quitgame.DrawButtonRelease_TextLeftAndLineDown(renderer);
		createuni.DrawButtonRelease_TextLeftAndLineDown(renderer);
		starnum.Draw(renderer);
		if (startnew._bMouseOnButton) {
			hovermessage.ProcessEvent(convertChar8ArrayToString(u8"aa启动\nsdhusdhuudh\nsh\naaaaaaaaaaaaaaaaaaaaaaaaaaa"), height / 32.0, width, height);
		}
		hovermessage.Draw(renderer, height / 32.0);
		SDL_RenderPresent(renderer);
	}

}

void StarMap::openold(int n) {


	Uint32 current_time = SDL_GetTicks();
	Uint32 last_time = current_time;
	double deltatime = (current_time - last_time) / 1000.0;
	double startscroll_y = 0;//存档页面滚动目标
	double menuy = 0;//存档页面滚动逼近

	UI::Button start = UI::Button(0.4 * width, 0.5 * height + menuy, 0.2 * width, 0.1 * height, 1, { 255,127,0,255 }, "IntoUniverse");
	UI::Button returntoolds = UI::Button(0, 0, 0.1 * width, 0.1 * height, 1, { 255,127,0,255 }, "Return");
	UI::Button deletetheworld = UI::Button(0.4 * width, 0.7 * height + menuy, 0.2 * width, 0.1 * height, 1, { 255,127,0,255 }, "deletetheworld");
	UI::Button realdel(0.4 * width, 0.5 * height, 0.2 * width, 0.1 * height, 0, { 255,127,0,255 }, "Delete!");
	UI::Button dontdel(0.4 * width, 0.7 * height, 0.2 * width, 0.1 * height, 0, { 255,127,0,255 }, "return");
	while (!returntoolds._bPressState && !realdel._bPressState) {
		current_time = SDL_GetTicks();
		deltatime = (current_time - last_time) / 1000.0;
		last_time = current_time;
		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				running = false;
			} else if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {//更新按钮位置
					width = event.window.data1;
					height = event.window.data2;
					SDL_RenderSetLogicalSize(renderer, width, height);
					start.ProcessEvent(0.4 * width, 0.5 * height + menuy, 0.2 * width, 0.1 * height, 1);
					returntoolds.ProcessEvent(0, 0, 0.1 * width, 0.1 * height, 1);
					deletetheworld.ProcessEvent(0.4 * width, 0.7 * height + menuy, 0.2 * width, 0.1 * height, 1);
					realdel.ProcessEvent(0.4 * width, 0.5 * height, 0.2 * width, 0.1 * height);
					dontdel.ProcessEvent(0.4 * width, 0.7 * height, 0.2 * width, 0.1 * height);
				}
			} else if (event.type == SDL_MOUSEWHEEL) {//存档列滚动
				if (event.wheel.y > 0) {
					if (startscroll_y < 0) {
						startscroll_y += static_cast<int>(height / 32.0);
					} else {
						startscroll_y = 0;
					}
				} else if (event.wheel.y < 0) {
					startscroll_y -= static_cast<int>(height / 32.0);
				}
			}
		}
		menuy += (startscroll_y - menuy) * variable_threshold1(18 * deltatime);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_Rect menu_rect = { 0,0 ,width, height };
		SDL_RenderCopy(renderer, menuback, nullptr, &menu_rect);


		SDL_Rect backgroundrect = { 0,0,width,height };
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderFillRect(renderer, &backgroundrect);
		start.ProcessEvent(0.4 * width, 0.5 * height + menuy, 0.2 * width, 0.1 * height);
		returntoolds.ProcessEvent(0, 0, 0.1 * width, 0.1 * height);
		deletetheworld.ProcessEvent(0.4 * width, 0.7 * height + menuy, 0.2 * width, 0.1 * height);
		if (start._bPressState == 1) {
			run(n);
			returntoolds._bPressState = 1;
		}
		if (deletetheworld.ProcessEvent(0.4 * width, 0.7 * height + menuy, 0.2 * width, 0.1 * height)) {
			SDL_Rect backgroundrect = { 0,0,width,height };
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
			SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_RenderFillRect(renderer, &backgroundrect);
			start.ProcessEvent(0);
			returntoolds.ProcessEvent(0);
			deletetheworld.ProcessEvent(0);
			realdel.ProcessEvent(0.4 * width, 0.5 * height, 0.2 * width, 0.1 * height, 1);
			dontdel.ProcessEvent(0.4 * width, 0.7 * height, 0.2 * width, 0.1 * height, 1);
			if (realdel._bMouseOnButton) {
				hovermessage.ProcessEvent(convertChar8ArrayToString(u8"存档") + std::to_string(n) + convertChar8ArrayToString(u8"将被永久删除\n下一个庞加莱回归见。"), height / 32.0, width, height);
			}
			if (realdel.ProcessEvent(0.4 * width, 0.5 * height, 0.2 * width, 0.1 * height, 1)) {
				std::string filepath = "save/" + std::to_string(n) + "star_data.json";
				std::remove(filepath.c_str());
			}
			if (dontdel.ProcessEvent(0.4 * width, 0.7 * height, 0.2 * width, 0.1 * height, 1)) {
				deletetheworld._bPressState = 0;

				dontdel._bPressState = 0;
				realdel.ProcessEvent(0);
				dontdel.ProcessEvent(0);
				start.ProcessEvent(1);
				returntoolds.ProcessEvent(1);
				deletetheworld.ProcessEvent(1);
			}

		}
		returntoolds.DrawButtonRelease_TextLeftAndLineDown(renderer);
		start.DrawButtonRelease_TextLeftAndLineDown(renderer);
		deletetheworld.DrawButtonRelease_TextLeftAndLineDown(renderer);
		realdel.DrawButtonRelease_TextLeftAndLineDown(renderer);
		dontdel.DrawButtonRelease_TextLeftAndLineDown(renderer);

		hovermessage.Draw(renderer, height / 32.0);
		SDL_RenderPresent(renderer);
	}


}


void StarMap::init_SDL() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw std::runtime_error("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
	}

	int numDisplays = SDL_GetNumVideoDisplays();
	numDisplays = 1;
	//numDisplays = 1;//test!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	if (numDisplays < 1) {
		std::cerr << "No displays found" << std::endl;
		SDL_Quit();
	}

	SDL_Rect largestDisplayRect{};
	int largestDisplayArea = 0;

	for (int i = 0; i < numDisplays; ++i) {
		SDL_Rect displayRect;
		if (SDL_GetDisplayBounds(i, &displayRect) != 0) {
			std::cerr << "SDL_GetDisplayBounds Error: " << SDL_GetError() << std::endl;
			SDL_Quit();
		}

		int area = displayRect.w * displayRect.h;
		if (area > largestDisplayArea) {
			largestDisplayArea = area;
			largestDisplayRect = displayRect;
		}
	}

	SDL_Window* window = SDL_CreateWindow("VNPGS",
		largestDisplayRect.x,
		largestDisplayRect.y,
		largestDisplayRect.w,
		largestDisplayRect.h,
		SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS);
	width = largestDisplayRect.w;
	height = largestDisplayRect.h;

	if (window == nullptr) {
		throw std::runtime_error("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));//renderer
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		throw std::runtime_error("Renderer could not be created! SDL_Error: " + std::string(SDL_GetError()));
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		throw std::runtime_error("SDL_image could not initialize! SDL_image Error: " + std::string(SDL_GetError()));
	}
}

void StarMap::load_textures() {
	menuback = load_texture("Assets/Textures/menuback.png");
	tieimg[0] = load_texture("Assets/Textures/bloom0.png");
	tieimg[1] = load_texture("Assets/Textures/bloom1.png");
	tieimg[2] = load_texture("Assets/Textures/bloom2.png");
	tieimg[3] = load_texture("Assets/Textures/bloom3.png");

	mesimg[0] = load_texture("Assets/Textures/stage0.png");
	mesimg[1] = load_texture("Assets/Textures/stage1.png");
	mesimg[2] = load_texture("Assets/Textures/stage2.png");
	mesimg[3] = load_texture("Assets/Textures/stage3.png");
	mesimg[4] = load_texture("Assets/Textures/stage4.png");
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

json StarMap::savej() {
	json j;
	for (auto& star : stars) {
		json star_json;
		star_json["name"] = star.number;
		star_json["absolute_pos"]["x"] = star.absolute_pos.x;
		star_json["absolute_pos"]["y"] = star.absolute_pos.y;
		star_json["absolute_pos"]["z"] = star.absolute_pos.z;
		star_json["temperature"] = star.temperature;
		star_json["power"] = star.power;
		star_json["dysondensity"] = star.dysondensity;
		star_json["radius"] = star.radius;
		star_json["teamname"] = star.teamname;
		star_json["needtoshowpos"] = star.needtoshowpos;
		star_json["type"] = star.type;

		j["stars"].push_back(star_json);
	}
	for (auto& route : routes) {
		json route_json;

		route_json["show"] = route.show;

		route_json["origin"]["name"] = route.origin.number;
		route_json["origin"]["absolute_pos"]["x"] = route.origin.absolute_pos.x;
		route_json["origin"]["absolute_pos"]["y"] = route.origin.absolute_pos.y;
		route_json["origin"]["absolute_pos"]["z"] = route.origin.absolute_pos.z;
		route_json["origin"]["temperature"] = route.origin.temperature;
		route_json["origin"]["power"] = route.origin.power;
		route_json["origin"]["dysondensity"] = route.origin.dysondensity;
		route_json["origin"]["radius"] = route.origin.radius;
		route_json["origin"]["teamname"] = route.origin.teamname;
		route_json["origin"]["needtoshowpos"] = route.origin.needtoshowpos;
		route_json["origin"]["type"] = route.origin.type;

		// Write destin details
		route_json["destin"]["name"] = route.destin.number;
		route_json["destin"]["absolute_pos"]["x"] = route.destin.absolute_pos.x;
		route_json["destin"]["absolute_pos"]["y"] = route.destin.absolute_pos.y;
		route_json["destin"]["absolute_pos"]["z"] = route.destin.absolute_pos.z;
		route_json["destin"]["temperature"] = route.destin.temperature;
		route_json["destin"]["power"] = route.destin.power;
		route_json["destin"]["dysondensity"] = route.destin.dysondensity;
		route_json["destin"]["radius"] = route.destin.radius;
		route_json["destin"]["teamname"] = route.destin.teamname;
		route_json["destin"]["needtoshowpos"] = route.destin.needtoshowpos;
		route_json["destin"]["type"] = route.destin.type;
		for (auto& ship : route.ships) {
			json ship_json;
			ship_json["number"] = ship.number;
			ship_json["category"] = ship.category;
			ship_json["dir"] = ship.dir;
			ship_json["loadmess"] = ship.loadmess;
			ship_json["fuelmess"] = ship.fuelmess;
			ship_json["volatilesmess"] = ship.volatilesmess;
			ship_json["starttime"] = ship.starttime;
			ship_json["endtime"] = ship.endtime;
			ship_json["v"] = ship.v;
			route_json["ships"].push_back(ship_json);
		}

		j["routes"].push_back(route_json);
	}
	j["timeingame"] = timeingame;
	j["cenposcam"]["x"] = cenposcam.x;
	j["cenposcam"]["y"] = cenposcam.y;
	j["cenposcam"]["z"] = cenposcam.z;
	j["posx"] = posx / double(width);
	j["posy"] = posy / double(height);
	j["r"] = r;
	j["rtarget"] = rtarget;


	return j;
}

bool StarMap::save(int number) {

	std::string filepath = "save/" + std::to_string(number) + "star_data.json";
	if (number >= 1) {

		std::ofstream outFile(filepath, std::ios::out);
		if (!outFile) {
			std::cerr << "存档炸了" << std::endl;
		}
		outFile << std::setw(4) << savej() << std::endl;
	} else if (number == 0) {

		bool aaaa = 1;
		number = 1;
		while (aaaa) {
			filepath = "save/" + std::to_string(number) + "star_data.json";
			std::ifstream file(filepath);

			if (file.is_open()) {
				file.close();
				number += 1;

			} else if (!file.is_open()) {

				std::ofstream outFile(filepath, std::ios::out);
				if (!outFile) {
					std::cerr << "存档炸了" << std::endl;
				}


				aaaa = 0;
				outFile << std::setw(4) << savej() << std::endl;

			}

		}

	}
	return 1;
}

void StarMap::generate_stars(double n) {//生成，可能无误
	const int nstar0 = n;
	const double stardensity = 0.004;
	const double rmap0 = std::pow(nstar0 * 3 / (stardensity * 4 * PI), 1.0 / 3);
	Npgs::Modules::StellarGenerator Gen(42, 0.075);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-rmap0, rmap0);
	std::uniform_real_distribution<> temp_dis(1000, 12000);
	std::uniform_real_distribution<> phi_dis(0, 1);
	std::uniform_real_distribution<> theta_dis(0, PI);
	std::uniform_int_distribution<> type_dis(0, 4);
	std::uniform_real_distribution<> radius_dis(-3.3, -1);
	std::cout << nstar0 << std::endl;
	std::cout << rmap0 << std::endl;
	int nnn = 0;
	StarVe tstar(
		0,
		0,
		1.7,
		Vec3(0, 0, 0),
		2800,
		std::pow(2.71828, -6.9077 + 23),
		type_dis(gen),
		0,
		-1,
		0,
		0
	);
	stars.push_back(tstar);

	StarVe ssstar(
		0,
		0,
		0,
		Vec3(1000, 0, 0),
		2800,
		std::pow(2.71828, -6.9077 + 23),
		type_dis(gen),
		0,
		-1,
		0,
		0
	);
	stars.push_back(ssstar);


	for (int i = 0; i < nstar0; i++) {
		double xstar0 = dis(gen);
		double ystar0 = dis(gen);
		double zstar0 = dis(gen);
		bool aaa = false;
		if (xstar0 * xstar0 + ystar0 * ystar0 + zstar0 * zstar0 < rmap0 * rmap0) {

			auto dstar = Gen.GenerateStar();
			while (std::isnan(dstar.GetRadius())) {
				dstar = Gen.GenerateStar(); std::cout<<"a";
			}
			std::string name = random_name();
			int tn = 0;
			if (i % 3 == 0) {
				tn = 1;
			} else if (i % 3 == 1) {
				tn = -1;
			}
			//	star_messages[i] = name + std::to_string(i) + convertChar8ArrayToString(u8"s introduction\ntestline, 114514中w");
				if (phi_dis(gen) > 0.9) {//测试垂线
					aaa = true;
				}
			double the = theta_dis(gen);
			double phi = phi_dis(gen);
			StarVe astar(
				i,
				the,
				phi,
				Vec3(xstar0, ystar0, zstar0),
				dstar.GetTeff(),
				dstar.GetLuminosity() / Npgs::kSolarLuminosity,
				type_dis(gen),
				dstar.GetRadius() * 0.000000000000000105700083402,
				tn,
				aaa,
				0
			);
			astar.StarData = dstar;

			for (int j = 1; j < 5; j++) {
				double r = 0.00001581 * j;
				
				PlaneT aplanet = {
					j,
					{0,0,0},{0,0,0},0,0,{0,0},
					0.00001581*j,
					the,
					phi,
					1
				};
				astar.planets.push_back(aplanet);
			}

			stars.push_back(astar);
			nnn = i;
		} else {
			i -= 1;
		}
	}
	//Npgs::Modules::StellarGenerator Genx(42, 8.0, 0.0, pow(10, 7));
	//
	//for (int i = 0; i < nstar0/1000.0; i++) {
	//	double xstar0 = dis(gen);
	//	double ystar0 = dis(gen);
	//	double zstar0 = dis(gen);
	//	bool aaa = false;
	//	if (xstar0 * xstar0 + ystar0 * ystar0 + zstar0 * zstar0 < rmap0 * rmap0) {
	//
	//		auto dstar = Genx.GenerateStar();
	//		while (dstar.GetMass() == 0) {
	//			dstar = Genx.GenerateStar();
	//		}
	//		std::string name = random_name();
	//		std::string tn = "fr";
	//		if (i % 2 == 0) {
	//			tn = "em";
	//		}
	//		star_messages[i] = name + std::to_string(i) + convertChar8ArrayToString(u8"s introduction\ntestline, 114514中w");
	//		if (power_dis(gen) > 0.9) {//测试垂线
	//			aaa = true;
	//		}
	//		StarVe astar = {
	//			i,
	//			Vec3(xstar0, ystar0, zstar0),
	//			Vec3(), 0, 0,{0, 0},
	//			dstar.GetTeff(),
	//			dstar.GetLuminosity() / Npgs::kSolarLuminosity,
	//			type_dis(gen),
	//			dstar.GetRadius() * 0.000000000000000105700083402,
	//			tn,
	//			aaa,
	//			{0, 0},
	//			0,
	//			0,
	//			dstar
	//		};
	//		stars.push_back(astar);
	//		nnn = i;
	//	} else {
	//		i -= 1;
	//	}
	//}
	//add_ship_into_route(1, 2, 1, 0, 0, 0.7, 0, 0, 0);
	//add_ship_into_route(1, 2, 1, 1, 0, 0.8, 0, 0, 0);
	//add_ship_into_route(1, 2, 1, 2, 0, 1, 0, 0, 0);
	//add_ship_into_route(1, 2, 1, 0, 1, 0.7, 0, 0, 0);
	//add_ship_into_route(1, 2, 1, 1, 1, 0.8, 0, 0, 0);
	//add_ship_into_route(1, 2, 1, 2, 1, 1, 0, 0, 0);
	//add_ship_into_route(1, 4, 1, 0, 0, 0.7, 0, 0, 0);
	//add_ship_into_route(1, 4, 1, 1, 0, 0.8, 0, 0, 0);
	//add_ship_into_route(1, 4, 1, 2, 0, 1, 0, 0, 0);
	//add_ship_into_route(2, 3, 1, 0, 0, 0.7, 0, 0, 0);
	//add_ship_into_route(2, 3, 1, 1, 0, 0.8, 0, 0, 0);
	//add_ship_into_route(2, 3, 1, 2, 0, 1, 0, 0, 0);
	//std::uniform_int_distribution<> n_dis(0, nnn);
	//std::uniform_int_distribution<> cat_dis(0, 2);
	//std::uniform_int_distribution<> dir_dis(0, 1);
    //for (int i = 0; i < 10000; i++) {
    //	add_ship_into_route(n_dis(gen), n_dis(gen), 1, cat_dis(gen), dir_dis(gen), phi_dis(gen), 0, 0, 0);
    //}

	//std::vector<double> dist = { 0.05,0.1,0.2,0.4,0.8,6.4,51.2,409.6,1638.5,3276.8 };
	//for (int i = 0; i < 10; i += 1) {
	//	double r = dist[i];
	//	tstar = {
	//		i + 1,
	//		Vec3(r * cos(PI / 9 * i), r * sin(PI / 9 * i), 0),
	//		Vec3(), 0,0, {0, 0},
	//		7000,
	//		10000,
	//		0,
	//		0.01 * 0.000016,
	//		"fr",
	//		true,
	//		{0, 0},
	//		0,
	//		0
	//	};
	//	stars.push_back(tstar);
	//}

}

void StarMap::generate_nebula() {
//	const int nstar0 = 2;
//	const double rmap0 = 100;
//
//	std::random_device rd;
//	std::mt19937 gen(rd());
//	std::uniform_real_distribution<> dis(-rmap0, rmap0);
//
//	std::uniform_real_distribution<> radius_dis(0, 1);
//	std::cout << std::ceil(nstar0 * 6 / PI) << std::endl;
//	std::cout << rmap0 << "a" << std::endl;
//	for (int i = 0; i < std::ceil(nstar0 * 6 / PI); ++i) {
//		double xstar0 = dis(gen);
//		double ystar0 = dis(gen);
//		double zstar0 = dis(gen);
//		bool aaa = false;
//		if (xstar0 * xstar0 + ystar0 * ystar0 + zstar0 * zstar0 < rmap0 * rmap0) {
//			std::string name = random_name();
//			star_messages[i] = name + std::to_string(i) + "s introduction\ntestline, 114514w";
//
//			StarVe nebula = {
//				1919810 + i,
//				Vec3(xstar0, ystar0, zstar0),
//				Vec3(), 0, 0,{0, 0},
//				0,
//				0,
//				-1,
//				pow(10,radius_dis(gen)),
//				"cl",
//				0,
//				{0, 0},
//				0,
//				1
//			};
//			stars.push_back(nebula);
//		}
//	}
}

void StarMap::generate_opoints() {
	for (double r = 1; r < 26; r *= 1.174618944) {
		for (double the = 0; the < 2 * PI; the += 2 * PI / 100.0) {
			Opoint apoint = {
				Vec3(r * std::cos(the), r * std::sin(the), 0),
				Vec3(), 0, 0,{0, 0}
			};
			opoints.push_back(apoint);

		}
	}
	for (double the = 0; the < 2 * PI; the += 2 * PI / 8) {
		for (int r = 0; r < 26; r += 1) {
			Opoint apoint = {
				Vec3(r * std::cos(the), r * std::sin(the), 0),
				Vec3(), 0, 0,{0, 0}
			};
			opoints.push_back(apoint);

		}
	}

}

void StarMap::run(int number) {//主循环，可能完善
	int a = 0;
	if (number == 0) {
		//  generate_stars();
		generate_nebula();
	} else {
		a = read(number);
	}
	if (a == 0) {
		Uint32 current_time = SDL_GetTicks();
		deltatime = (current_time - last_time) / 1000.0;
		last_time = current_time;

		while (running && !ifsave) {



			handle_events();

			update();


			render();
			if (ifexit) {
				ifsave = save(number);
			}
		}
	}
	cleanup();
}

bool StarMap::read(int number) {
	std::string filepath = "save/" + std::to_string(number) + "star_data.json";
	std::ifstream file(filepath);

	if (!file.is_open()) {
		std::cerr << "Error opening file: " << filepath << std::endl;
	}
	json j;
	file >> j;
	try {
		for (const auto& star_json : j["stars"]) {
			StarVe star;

			// Read star properties from JSON object
			star.number = star_json["name"];
			star.absolute_pos.x = star_json["absolute_pos"]["x"];
			star.absolute_pos.y = star_json["absolute_pos"]["y"];
			star.absolute_pos.z = star_json["absolute_pos"]["z"];
			star.temperature = star_json["temperature"];
			star.power = star_json["power"];
			star.dysondensity = star_json["dysondensity"];
			star.radius = star_json["radius"];
			star.teamname = star_json["teamname"];
			star.needtoshowpos = star_json["needtoshowpos"];
			star.type = star_json["type"];

			// Add star to vector
			stars.push_back(star);
		}
		for (auto& route_json : j["routes"]) {
			Route route;

			route.show = route_json["show"];

			route.origin.number = route_json["origin"]["name"];
			route.origin.absolute_pos.x = route_json["origin"]["absolute_pos"]["x"];
			route.origin.absolute_pos.y = route_json["origin"]["absolute_pos"]["y"];
			route.origin.absolute_pos.z = route_json["origin"]["absolute_pos"]["z"];
			route.origin.temperature = route_json["origin"]["temperature"];
			route.origin.power = route_json["origin"]["power"];
			route.origin.dysondensity = route_json["origin"]["dysondensity"];
			route.origin.radius = route_json["origin"]["radius"];
			route.origin.teamname = route_json["origin"]["teamname"];
			route.origin.needtoshowpos = route_json["origin"]["needtoshowpos"];
			route.origin.type = route_json["origin"]["type"];


			route.destin.number = route_json["destin"]["name"];
			route.destin.absolute_pos.x = route_json["destin"]["absolute_pos"]["x"];
			route.destin.absolute_pos.y = route_json["destin"]["absolute_pos"]["y"];
			route.destin.absolute_pos.z = route_json["destin"]["absolute_pos"]["z"];
			route.destin.temperature = route_json["destin"]["temperature"];
			route.destin.power = route_json["destin"]["power"];
			route.destin.dysondensity = route_json["destin"]["dysondensity"];
			route.destin.radius = route_json["destin"]["radius"];
			route.destin.teamname = route_json["destin"]["teamname"];
			route.destin.needtoshowpos = route_json["destin"]["needtoshowpos"];
			route.destin.type = route_json["destin"]["type"];
			for (auto& ship_json : route_json["ships"]) {
				Starship ship;
				ship.number = ship_json["number"];
				ship.category = ship_json["category"];
				ship.loadmess = ship_json["loadmess"];
				ship.dir = ship_json["dir"];
				ship.fuelmess = ship_json["fuelmess"];
				ship.volatilesmess = ship_json["volatilesmess"];
				ship.starttime = ship_json["starttime"];
				ship.endtime = ship_json["endtime"];
				ship.v = ship_json["v"];

				route.ships.push_back(ship);
			}

			routes.push_back(route);
		}
		timeingame = j["timeingame"];
		cenposcam.x = j["cenposcam"]["x"];
		cenposcam.y = j["cenposcam"]["y"];
		cenposcam.z = j["cenposcam"]["z"];
		posx = j["posx"] * width;
		posy = j["posy"] * height;
		r = j["r"];
		rtarget = j["rtarget"];
		file.close();
	} catch (const json::type_error& e) {
		std::cerr << "JSON type error: " << e.what() << std::endl;
		return 1;
	}
	return 0;

}

void StarMap::handle_events() {//鼠标事件，可能完善

	bool aa = !menubutton.ProcessEvent(0, 0,  width/30.0, width / 30.0, 1) && !menubutton._bMouseOnButton;

	doubleclick = false;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {

		if (event.type == SDL_QUIT) {
			running = false;
		} else if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				width = event.window.data1;
				height = event.window.data2;
				SDL_RenderSetLogicalSize(renderer, width, height);
			}
		} else if (event.type == SDL_MOUSEWHEEL) {
			handle_mouse_wheel(event);
		} else if (event.type == SDL_MOUSEBUTTONDOWN && aa) {
			handle_mouse_button_down(event);
		} else if (event.type == SDL_MOUSEBUTTONUP && aa) {
			handle_mouse_button_up(event);
		} else if (event.type == SDL_MOUSEMOTION && !menubutton.ProcessEvent(0, 0, width / 30.0, width / 30.0, 1)) {
			handle_mouse_motion(event);
		}
	}

}

void StarMap::handle_mouse_wheel(SDL_Event& event) {//页面内摄影机距离和页面外侧边栏滚动

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	if (!menubutton.ProcessEvent(0, 0, width / 30.0, width / 30.0, 1) && !menubutton._bMouseOnButton) {
		if (!showmessage || (showmessage && double(mouseX) / width < (1 - messagewidthrate))) {
			if (event.wheel.y > 0) {
				rtarget /= 1.2;
			} else if (event.wheel.y < 0) {
				rtarget *= 1.2;
			}
		} else if (showmessage && double(mouseX) / width > (1 - messagewidthrate)) {
			if (event.wheel.y > 0) {
				if (targetscroll_y < 0) {
					targetscroll_y += static_cast<int>(height / 32.0);
				} else {
					targetscroll_y = 0;
				}
			} else if (event.wheel.y < 0) {
				targetscroll_y -= static_cast<int>(height / 32.0);
			}
		}
	} else if (menubutton.ProcessEvent(0, 0, width / 30.0, width / 30.0, 1)) {
		if (event.wheel.y > 0) {
			if (menutargetscolly < 0) {
				menutargetscolly += static_cast<int>(height / 32.0);
			} else {
				menutargetscolly = 0;
			}
		} else if (event.wheel.y < 0) {
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
			if (deltaclicktime < 0.3) {
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
	static int framenum = 0;
	static bool showframe = 1;
	Uint32 current_time = SDL_GetTicks();
	framenum++;
	if ((int(current_time) % 1000 < 500) && showframe) {
		std::cout << framenum << std::endl;
		framenum = 0;
		showframe = 0;
	}
	if (int(current_time) % 1000 > 500) {
		showframe = 1;
	}
	deltatime = (current_time - last_time) / 1000.0;
	last_time = current_time;
	//std::cout << 1 / deltatime << std::endl;
	if (!menubutton._bPressState && stop._bPressState) {
		timeingame += deltatime * timerate;
	}
	// std::cout << timeingame << std::endl;
	lastmonth = month;
	month = int(fmod(timeingame , 31536000.0)/ 2628000.0);

	if(month!=lastmonth||deltatime>6* 2628000.0)
	{
		shiptakeof();
	}



	update_camera();
	if (showcircle == 1) {
		update_opoints();
	}
	// 更新消息面板横向运动
	if (showmessage) {
		messagextarget = width * (1 - messagewidthrate);
	} else {
		messagextarget = 1.1 * width;
	}
	messagex += (messagextarget - messagex) * variable_threshold1(18 * deltatime);

	// 更新侧边栏滚动
	scroll_y += (targetscroll_y - scroll_y) * variable_threshold1(18 * deltatime);
	//菜单滚动
	if (menubutton.ProcessEvent(0, 0, width / 30.0, width / 30.0, 1)) {
		menuscolly += (menutargetscolly - menuscolly) * variable_threshold1(18 * deltatime);
	}

	// 更新相机距离
	r += (rtarget - r) * variable_threshold1(9.6 * deltatime);
	//std::cout << messagex<< std::endl;
}

void StarMap::update_camera() {

	if (totar == true&&!toplanet) {
		if (vector_length(subtract_vectors(targetstar.absolute_pos, cenposcam)) < pow(10, -14)) {
			totar = false;
		}
		if (totar == true)
		{
			followplanet = -1;
			toplanet = false;
			onplanet = false;
			cenposcam = add_vectors(cenposcam, scalar_multiply(variable_threshold1(0.05 * 60 * deltatime), (subtract_vectors(targetstar.absolute_pos, cenposcam))));
			//circen = target;
			//cirtheta = ttheta;
			//cirphi = tphi;
			vec1to2(zvectheta, zvecphi, targetstar.theta, targetstar.phi, variable_threshold1(0.05 * 60 * deltatime));
		}


	}
	if (vector_length(subtract_vectors(controlstar.absolute_pos, circen)) < pow(10, -14)) {
		tocontrol = false;
	}
	if (tocontrol) {

		circen = add_vectors(circen, scalar_multiply(variable_threshold1(0.05 * 240 * deltatime), (subtract_vectors(controlstar.absolute_pos, circen))));
		vec1to2(cirtheta, cirphi, controlstar.theta, controlstar.phi, variable_threshold1(0.05 * 140 * deltatime));
	}
	if (toplanet) {
		if (vector_length(subtract_vectors(targetplanet, cenposcam)) < pow(10, -13)) {
			toplanet = false;
			onplanet = true;
		}
		if (toplanet)
		{
			onplanet = 0;
			Vec3 dx = subtract_vectors(targetplanet, lasttargetplanet);
			if (lastfollowplanet == followplanet) {
				cenposcam = add_vectors(cenposcam, dx);
			}
			cenposcam = add_vectors(cenposcam, scalar_multiply(variable_threshold1(0.05 * 60 * deltatime), (subtract_vectors(targetplanet, cenposcam))));
		}

	}	if (onplanet) {
		cenposcam = targetplanet;

	}
	// 更新相机位置和方向,可能完善
	if (posx > width) {
		posx -= width;
	} else if (posx < 0) {
		posx += width;
	}
	if (posy > height) {
		posy = height;
	} else if (posy < 0) {
		posy = 0;
	}
	theta = posx / width * 4 * PI;
	phi = 0.999 * (posy / height - 0.5) * PI;

	reposcam.x = r * std::cos(phi) * std::cos(theta);
	reposcam.y = r * std::cos(phi) * std::sin(theta);
	reposcam.z = r * std::sin(phi);
	reposcam = vecrotate(reposcam, zvectheta, zvecphi);
	poscam = add_vectors(cenposcam, reposcam);
	Vec3 vectoz = vecrotate({0,0,1}, zvectheta, zvecphi);
	vecx = normalize_vector(cross_product(vectoz, reposcam));
	vecy = normalize_vector(cross_product(reposcam, vecx));
	vecz = normalize_vector(reposcam);
	Vec3 t1(
    std::sin(phi) * std::cos(theta),
	std::sin(phi) * std::sin(theta),
	std::cos(phi)
	);


}
void StarMap::shiptakeof() {
	std::map<Parameterofship, Parameterofship> mergedMap;
	for (auto& aship : shipwaittotakeof) {
		auto it = mergedMap.find(aship);
		if (it != mergedMap.end()) {
			// 如果key存在，则累加其他参数
			it->second.number += aship.number;
			it->second.loadmess += aship.loadmess;
			it->second.fuelmess += aship.fuelmess;
			it->second.volatilesmess += aship.volatilesmess;
		} else {
			// 如果key不存在，则插入新的entry
			mergedMap[aship] = aship;
		}
	}
	shipwaittotakeof.clear();
	for (const auto& entry : mergedMap) {
		const auto& aship = entry.second;
		add_ship_into_route(aship.fnum, aship.znum, aship.number, aship.category, aship.dir, aship.v, aship.loadmess, aship.fuelmess, aship.volatilesmess,aship.planetnum);
	}
}
void StarMap::add_ship_into_route(int number1, int number2, int numberofship, int cat, bool dir, double v, double m1, double m2, double m3,int pnum) {
	int sm;
	int bm;

	if (number1 != number2) {
		if (number1 > number2) {
			bm = number1;
			sm = number2;
		} else {
			bm = number2;
			sm = number1;
		}
		if (std::find_if(stars.begin(), stars.end(), [sm](const StarVe& d) { return d.number == sm; }) != stars.end() && std::find_if(stars.begin(), stars.end(), [bm](const StarVe& d) { return d.number == bm; }) != stars.end()) {
			auto it = std::find_if(routes.begin(), routes.end(),
				[sm, bm](const Route& d) {
					return d.origin.number == sm && d.destin.number == bm;
				});
			StarVe astar = stars[std::distance(stars.begin(), std::find_if(stars.begin(), stars.end(), [sm](const StarVe& d) { return d.number == sm; }))];//需要处理不存在恒星。。。。但我暂时懒得
			StarVe bstar = stars[std::distance(stars.begin(), std::find_if(stars.begin(), stars.end(), [bm](const StarVe& d) { return d.number == bm; }))];
			double dis = vector_length(subtract_vectors(astar.absolute_pos, bstar.absolute_pos));
			Starship ship = Starship(numberofship, cat, dir, timeingame + dis / v, m1, m2, m3, timeingame, v,pnum);
			if (it != routes.end()) {

				it->ships.push_back(ship);
			} else {
				StarVe ostar = stars[std::distance(stars.begin(), std::find_if(stars.begin(), stars.end(), [sm](const StarVe& d) { return d.number == sm; }))];//同上
				StarVe dstar = stars[std::distance(stars.begin(), std::find_if(stars.begin(), stars.end(), [bm](const StarVe& d) { return d.number == bm; }))];
				Route route = Route(ostar, dstar);
				route.ships.push_back(ship);
				route.show = 0;
				routes.push_back(route);

			}

		}




	}
}

void StarMap::sortsatrsbydistance() {//待优化
	// 分离距离小于10的星星和其他星星
	/*std::vector<Star> starsLessThan10;
	std::vector<Star> starsGreaterThanEqual10;

	for (const Star& star : stars) {
		if (star.distance < 10&& star.screen_pos.x >= 0 && star.screen_pos.x < width &&
			star.screen_pos.y >= 0 && star.screen_pos.y < height && star.depth>0) {
			starsLessThan10.push_back(star);
		}
		else {
			starsGreaterThanEqual10.push_back(star);
		}
	}

	// 对距离小于10的星星按距离排序
	std::sort(starsLessThan10.begin(), starsLessThan10.end(), [](const Star& a, const Star& b) {
		return a.distance > b.distance;
		});

	// 将排序后的星星放回原始向量，先放大于等于10的，再放小于10的
	stars.clear();
	stars.reserve(starsGreaterThanEqual10.size() + starsLessThan10.size());
	stars.insert(stars.end(), starsGreaterThanEqual10.begin(), starsGreaterThanEqual10.end());
	stars.insert(stars.end(), starsLessThan10.begin(), starsLessThan10.end());*/
}

void StarMap::update_and_draw_stars() {//右键边栏目标确定，双击移动目标确定，可能完善
	lastfollowplanet = followplanet;
	lasttargetplanet = targetplanet;
	sortsatrsbydistance();
	int nearstars = 0;
	int mouseX, mouseY;
	int nplant=0;
	const Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);
	//double maxr = 0;
	Vec3 cirz = vecrotate({0,0,1},cirtheta,cirphi);
	for (auto& star : stars) {
		//maxr = std::max(maxr,std::sqrt(pow(star.absolute_pos.x,2)+ pow(star.absolute_pos.y, 2)));
		Vec3 relative_pos = subtract_vectors(star.absolute_pos, poscam);
		star.relative_pos = relative_pos;
		star.distance = vector_length(relative_pos);
		//std::cout << star.distance << std::endl;
		double projx = dot_product(relative_pos, vecx);
		double projy = dot_product(relative_pos, vecy);
		double projz = dot_product(relative_pos, vecz);


		double screenx = width * (0.5 - 0.5 * projx / projz);
		double screeny = height * (0.5 - 0.5 * width / double(height) * projy / projz);
		star.screen_pos = { static_cast<int>(screenx), static_cast<int>(screeny) };
		star.depth = -projz;
		if (star.needtoshowpos == true) {
			Vec3 zpos = subtract_vectors(star.absolute_pos, scalar_multiply(dot_product(subtract_vectors(star.absolute_pos, circen), cirz), cirz));

			Vec3 zrelative_pos = subtract_vectors(zpos, poscam);
			double zx = dot_product(zrelative_pos, vecx);
			double zy = dot_product(zrelative_pos, vecy);
			double zz = dot_product(zrelative_pos, vecz);
			double zscreenx = width * (0.5 - 0.5 * zx / zz);
			double zscreeny = height * (0.5 - 0.5 * width / double(height) * zy / zz);
			star.zpoint = { static_cast<int>(zscreenx), static_cast<int>(zscreeny) };
			star.zpdep = -zz;
		}
	}
	if (showcircle) {
		for (auto& star : stars) {
			if (vector_length(subtract_vectors(circen,star.absolute_pos))<=25 && (star.depth > 0 || star.zpdep > 0) && star.needtoshowpos == true && star.type == 0 && showvertical == 1) {
				double x = star.screen_pos.x;
				double y = star.screen_pos.y;
				double d = star.depth;
				if (d > 0 && star.zpdep < 0) {
					star.zpoint.x = x - (star.zpoint.x - x) * double(d - star.zpdep) / variable_threshold00(double(d));
					star.zpoint.y = y - (star.zpoint.y - y) * double(d - star.zpdep) / variable_threshold00(double(d));
				}
				if (d < 0 && star.zpdep > 0) {
					x = star.zpoint.x - (x - star.zpoint.x) * double(star.zpdep - d) / variable_threshold00((star.zpdep));
					y = star.zpoint.y - (y - star.zpoint.y) * double(star.zpdep - d) / variable_threshold00((star.zpdep));
				}
				if ((dot_product(cirz, subtract_vectors(circen, poscam))) / (dot_product(cirz, subtract_vectors(circen, star.absolute_pos))) >= 0) {
					SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
					SDL_RenderDrawLine(renderer, x, y, star.zpoint.x, star.zpoint.y);
				} else if ((dot_product(cirz, subtract_vectors(circen, poscam))) / (dot_product(cirz, subtract_vectors(circen, star.absolute_pos))) < 0) {
					SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
					drawDashedLine(renderer, x, y, star.zpoint.x, star.zpoint.y, 10);
				}
			}
		}
	}
	for (auto& star : stars) {
		double ll = 280 * pow(star.distance, -(0.6 + star.distance / 11000.0)) * pow(star.power, 1 / 4.0) * width / 1920;

		double starrad = width / std::sqrt(variable_threshold001(pow((star.distance / star.radius), 2) - 1));

		if (ll > height / 1.8) {
			ll = height / 1.8;
		}
		double screenx = star.screen_pos.x;
		double screeny = star.screen_pos.y;
		if (star.screen_pos.x >= 0 && star.screen_pos.x < width &&
			star.screen_pos.y >= 0 && star.screen_pos.y < height && star.depth>0 && mousestorey<=0 && mouseX < messagex) {
			if ((buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) && star.type == 0 && !menubutton.ProcessEvent(0, 0, width / 30.0, width / 30.0, 1) && !menubutton._bMouseOnButton) {
				if ((mouseX - screenx) * (mouseX - screenx) + (mouseY - screeny) * (mouseY - screeny) < std::max(pow((std::max(ll / 8, starrad)), 2), 4.0)) {
					nearstars++;
					showmessage = true;
					rightclickstar = star;
					
				}
				if (nearstars == 0) {
					showmessage = false;
				}
				planetmessage = 0;
			}
			if (doubleclick == true) {
				if ((mouseX - screenx) * (mouseX - screenx) + (mouseY - screeny) * (mouseY - screeny) < std::max(pow((std::max(ll / 8, starrad)), 2), 4.0) && star.type == 0) {
					totar = true;
					toplanet = false;
					targetstar = star;
				}
			}
			if ((mouseX - screenx) * (mouseX - screenx) + (mouseY - screeny) * (mouseY - screeny) < std::max(pow((std::max(ll / 8, starrad)), 2), 4.0) && star.type == 0 && !menubutton.ProcessEvent(0, 0, width / 30.0, width / 30.0, 1)) {
				std::string str = convertChar8ArrayToString(u8"恒星") + std::to_string(star.number) + "\n" + convertChar8ArrayToString(u8"半径:") + std::to_string(star.radius / 0.000016) + convertChar8ArrayToString(u8"天文单位") + "\n" + convertChar8ArrayToString(u8"功率:") + std::to_string(star.power) + convertChar8ArrayToString(u8"倍太阳") + std::to_string(ll);
				hovermessage.ProcessEvent(str, height / 70.0, width, height);
			}
		}
		//draw
		if (vector_length(subtract_vectors(star.absolute_pos, targetstar.absolute_pos)) < 0.00000000000000001) {
			for (auto& planet : star.planets) {
				{

					Vec3 relative_pos = subtract_vectors(planet.absolute_pos, poscam);
					planet.relative_pos = relative_pos;
					planet.distance = vector_length(relative_pos);

					double projx = dot_product(relative_pos, vecx);
					double projy = dot_product(relative_pos, vecy);
					double projz = dot_product(relative_pos, vecz);
					double screenx = width * (0.5 - 0.5 * projx / projz);
					double screeny = height * (0.5 - 0.5 * width / double(height) * projy / projz);
					planet.screen_pos = { static_cast<int>(screenx), static_cast<int>(screeny) };
					planet.depth = -projz;

				}
				if (planet.orbit_r / planet.distance > 1 / 100.0) {

					if (planet.screen_pos.x >= 0 && planet.screen_pos.x < width &&
						planet.screen_pos.y >= 0 && planet.screen_pos.y < height && planet.depth>0) {
						drawFilledCircle(renderer, planet.screen_pos.x, planet.screen_pos.y, 2, { 0,0,255,150 });
					}

					static std::vector<Opoint> orbit(500);
					if (showorbit) {
						for (size_t i = 0; i < orbit.size(); i++) {
							double rot;
							if (!menubutton._bPressState && stop._bPressState) {
								rot = -2 * PI * i / orbit.size() + (timeingame - deltatime * timerate) * sqrt(6.6743 * pow(10, -11) * star.StarData.GetMass() * pow(planet.orbit_r * 9460730472580800, -3));
							} else {
								rot = -2 * PI * i / orbit.size() + (timeingame)*sqrt(6.6743 * pow(10, -11) * star.StarData.GetMass() * pow(planet.orbit_r * 9460730472580800, -3));
							}
							orbit[i].absolute_pos = add_vectors(star.absolute_pos, scalar_multiply(planet.orbit_r, vec90rotate(rot, planet.theta, planet.phi)));
							Vec3 relative_pos = subtract_vectors(orbit[i].absolute_pos, poscam);
							orbit[i].relative_pos = relative_pos;
							orbit[i].distance = vector_length(relative_pos);
							//std::cout << star.distance << std::endl;
							double projx = dot_product(relative_pos, vecx);
							double projy = dot_product(relative_pos, vecy);
							double projz = dot_product(relative_pos, vecz);

							double screenx = width * (0.5 - 0.5 * projx / projz);
							double screeny = height * (0.5 - 0.5 * width / double(height) * projy / projz);
							orbit[i].screen_pos = { static_cast<int>(screenx), static_cast<int>(screeny) };
							//std::cout<< screenx << screeny<<std::endl;
							orbit[i].depth = -projz;
						}
						for (size_t i = 1; i < orbit.size(); i++) {
							Opoint tpoint = orbit[i];
							Opoint lpoint = orbit[i - 1];

							if (((tpoint.screen_pos.x >= 0 && tpoint.screen_pos.x < width &&
								tpoint.screen_pos.y >= 0 && tpoint.screen_pos.y < height && tpoint.depth>0) && (lpoint.screen_pos.x >= 0 && lpoint.screen_pos.x < width &&
									lpoint.screen_pos.y >= 0 && lpoint.screen_pos.y < height && lpoint.depth>0))) {
								int col = 255 * (orbit.size() - i) / double(orbit.size());
								SDL_SetRenderDrawColor(renderer, 0, 174, 255, col);
								SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
								SDL_RenderDrawLine(renderer, lpoint.screen_pos.x, lpoint.screen_pos.y, tpoint.screen_pos.x, tpoint.screen_pos.y);

							}


						}
					}
				}
				{
					double rot = timeingame * sqrt(6.6743 * pow(10, -11) * star.StarData.GetMass() * pow(planet.orbit_r * 9460730472580800, -3));
					planet.absolute_pos = add_vectors(star.absolute_pos, scalar_multiply(planet.orbit_r, vec90rotate(rot, planet.theta, planet.phi)));
				}
				if (planet.orbit_r / planet.distance > 1 / 100.0) {
					if (mousestorey <= 0 && mouseX < messagex) {
						if (doubleclick == true) {
							if ((mouseX - planet.screen_pos.x) * (mouseX - planet.screen_pos.x) + (mouseY - planet.screen_pos.y) * (mouseY - planet.screen_pos.y) < 9) {
								toplanet = 1;
								followplanet = planet.number;
							}
						}

						if ((mouseX - planet.screen_pos.x) * (mouseX - planet.screen_pos.x) + (mouseY - planet.screen_pos.y) * (mouseY - planet.screen_pos.y) < 9 && mousestorey <= 0 && !menubutton.ProcessEvent(0, 0, width / 30.0, width / 30.0, 1)) {
							std::string str = convertChar8ArrayToString(u8"planet") + std::to_string(planet.number);
							hovermessage.ProcessEvent(str, height / 70.0, width, height);
							if ((buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) && !menubutton.ProcessEvent(0, 0, width / 30.0, width / 30.0, 1) && !menubutton._bMouseOnButton) {
								rightclickplanet = planet;
								rightclickstar = star;
								planetmessage = true;
								nplant+=1;
							}
						}
					}
					if (lastfollowplanet != followplanet) {
						onplanet = 0;
					}
					if (followplanet >= 0 && followplanet == planet.number) {


						targetplanet = planet.absolute_pos;
					}
				}

			}
		}
		
		if (star.screen_pos.x >= 0 && star.screen_pos.x < width &&
			star.screen_pos.y >= 0 && star.screen_pos.y < height && star.depth>0 && star.type == 0) {
			SDL_Color color = ScaleSDLColor(pow(star.power, 1.0 / 8.0) / 8.0 / (variable_threshold001(100 * (star.distance)) / 100), kelvin_to_rgb(star.temperature));
			SDL_Color color0 = kelvin_to_rgb(star.temperature);
			double  starrad = width / sqrt(variable_threshold001(pow((star.distance / star.radius), 2) - 1));
			if (starrad >= 1) {
				drawFilledCircle(renderer, star.screen_pos.x, star.screen_pos.y, int(starrad), color0); //恒星本体，原色
			} else {
				SDL_Color acolor = ScaleSDLColor(variable_threshold1(ll / 7), color0);
				SDL_SetRenderDrawColor(renderer, acolor.r, acolor.g, acolor.b, acolor.a);
				SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
				SDL_RenderDrawPoint(renderer, star.screen_pos.x, star.screen_pos.y);//处理过远不显示的情况
				SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			}
			SDL_Color AA = { 0, 0, 0, 255 };



			SDL_Texture* modified_image = tieimg[3];

			if (ll > height / 1.5) {
				ll = height / 1.5;
			}
			if (ll > 128) {
				modified_image = tieimg[0];
			} else if (ll > 64) {
				modified_image = tieimg[1];
			} else if (ll > 32) {
				modified_image = tieimg[2];
			} else if (ll <= 32) {
				modified_image = tieimg[3];
			}

			AA.r = color0.r;
			AA.g = color0.g;
			AA.b = color0.b;
			AA.a = static_cast<uint8_t>(255 * 2 * (0.5 - 0.5 * tanh(0.05 * log1p(starrad))));


			if (ll > 7) {
				SDL_Rect dest_rect = { int(star.screen_pos.x - ll / 2 + 1), int(star.screen_pos.y - ll / 2 + 1), int(ll), int(ll) };
				renderTextureWithColor(renderer, modified_image, AA, dest_rect);

				if (ll > 20 && starrad < 1) {
					ll /= 3.0 / std::min(1.0, 0.1 / starrad);
					if (ll > 7) {
						dest_rect = { int(star.screen_pos.x - ll / 2 + 1), int(star.screen_pos.y - ll / 2 + 1), int(ll), int(ll) };
						renderTextureWithColor(renderer, modified_image, { 255,255,255,static_cast<unsigned char>(std::min(255,int(255 * ll / 20.0))) }, dest_rect);
					}
				}



			}
		}


		if (star.type == 1 && star.depth > 0) {
			double ll = std::min((width / sqrt(variable_threshold00(pow((star.distance / star.radius), 2) - 1))), double(2 * width)) * width / 1920;
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
			drawFilledCircle(renderer, star.screen_pos.x, star.screen_pos.y, ll, { 200,0,200,50 });
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


		}

	}
	if (nplant > 0) {
		showmessage = 1;
		planetmessage = 1;
	}


}


void StarMap::update_opoints() {
	int i = 0;
	for (double r = 1; r < 26; r *= 1.174618944) {
		for (double the = 0; the < 2 * PI; the += 2 * PI / 100.0) {
			Vec3 avec(r * std::cos(the), r * std::sin(the), 0);
			avec =add_vectors(circen, vecrotate(avec, cirtheta, cirphi));
			Opoint apoint = {
				avec,
				Vec3(), 0, 0,{0, 0}
			};
			opoints[i]=apoint;
			i++;
		}
	}
	for (double the = 0; the < 2 * PI; the += 2 * PI / 8) {
		for (int r = 0; r < 26; r += 1) {
			Vec3 avec(r * std::cos(the), r * std::sin(the), 0);
			avec = add_vectors(circen, vecrotate(avec, cirtheta, cirphi));
			Opoint apoint = {
				avec,
				Vec3(), 0, 0,{0, 0}
			};
			opoints[i] = apoint;
			i++;
		}
	}
	for (auto& opoint : opoints) {
		Vec3 relative_pos = subtract_vectors(opoint.absolute_pos, poscam);
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
	open = menubutton.ProcessEvent(0, 0, width / 30.0, width / 30.0, 1);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);



	SDL_RenderClear(renderer);


	if (showcircle == 1) {
		draw_opoints();
	}
	if (ShipAndRKKV._bPressState) {
		updata_and_draw_ships();
	}
	update_and_draw_stars();


	draw_time(timeingame);

	if (open) {//menu open
		SDL_Rect backgroundrect = { 0,0,width,height };

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

		SDL_RenderFillRect(renderer, &backgroundrect);

	
		ifexit = exitbutton.ProcessEvent(0.6 * width, 0.3 * height + menuscolly, 0.2 * width, 0.07 * height, 1);
		ShipAndRKKV.ProcessEvent(0);

		coordinate.ProcessEvent(0);
		orbit.ProcessEvent(0);
		addtimerate.ProcessEvent(0);
		stop.ProcessEvent(0);
		subtimerate.ProcessEvent(0);


		

	}
	if (!open) {
		if(messagex >= width- 0.5*width * messagewidthrate)
		{
			showcircle = coordinate.ProcessEvent((1 - 5 / 30.0) * width, height - width / 30.0, width / 30.0, width / 30.0, 1);
			showorbit = orbit.ProcessEvent((1 - 1 / 30.0) * width, height - width / 30.0, width / 30.0, width / 30.0, 1);
			showvertical = showcircle;
			ShipAndRKKV.ProcessEvent((1 - 1 / 30.0) * width, height - 2 * width / 30.0, width / 30.0, width / 30.0, 1);
			if (addtimerate.ProcessEvent((1 - 1.5 / 30.0) * width, 0, width / 30.0, width / 30.0, 1)) {
				addtimerate._bPressState = 0;
				timerate *= 1.584893;
			}
			if (subtimerate.ProcessEvent((1 - 4.5 / 30.0) * width, 0, width / 30.0, width / 30.0, 1)) {
				subtimerate._bPressState = 0;
				timerate /= 1.584893;
			}
			stop.ProcessEvent((1 - 3 / 30.0) * width, 0, width / 30.0, width / 30.0, 1);
			if (stop._bPressState == 1) {
				stop._MapName = "pause.png";
			} else {
				stop._MapName = "continue.png";
			}
		}
		else {
			coordinate.ProcessEvent(0);
			orbit.ProcessEvent(0);
			addtimerate.ProcessEvent(0);
			stop.ProcessEvent(0);
			subtimerate.ProcessEvent(0);
		}


		exitbutton.ProcessEvent(0);
		if (messagex >= width - 0.5 * width * messagewidthrate) {
			std::string ratetext = std::to_string(timerate);//时间显示
			if (timerate < 10000) {
				ratetext = "x" + ratetext.substr(0, ratetext.length() - 4);
			} else if (timerate < 10000000) {
				ratetext = "x" + std::to_string(int(timerate) / 1000) + "K";
			} else {
				ratetext = "x" + std::to_string(long long int(timerate) / 1000000) + "M";
			}
			showtimeingame.Draw(renderer, ratetext, height / 32.0, width, height, 0.92, 0.06, "Assets/Fonts/FileDeletion.ttf");
		}
	}
	menubutton.DrawButtonPress(renderer);
	coordinate.DrawButtonWithoutRect_blue(renderer);
	orbit.DrawButtonWithoutRect_blue(renderer);
	exitbutton.DrawButtonRelease_TextLeftAndLineDown(renderer);
	addtimerate.DrawButtonWithoutRect_grey(renderer);
	subtimerate.DrawButtonWithoutRect_grey(renderer);
	ShipAndRKKV.DrawButtonWithoutRect_blue(renderer);
	stop.DrawButtonWithoutRect_grey(renderer);
	lastopen = open;
	if (!open) {
		draw_info_panel();
	}
	hovermessage.Draw(renderer, height / 70.0);
	SDL_RenderPresent(renderer);
}

void  StarMap::renderTextureWithColor(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Color color, SDL_Rect destRect) {
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(texture, color.a);
	SDL_RenderCopy(renderer, texture, nullptr, &destRect);
}

void StarMap::updata_and_draw_ships() {
	for (std::vector<Route>::iterator route = routes.begin(); route != routes.end();) {
		{
			Vec3 relative_pos = subtract_vectors(route->origin.absolute_pos, poscam);
			double projx = dot_product(relative_pos, vecx);
			double projy = dot_product(relative_pos, vecy);
			double projz = dot_product(relative_pos, vecz);
			double screenx = width * (0.5 - 0.5 * projx / projz);
			double screeny = height * (0.5 - 0.5 * width / double(height) * projy / projz);
			route->origin.screen_pos = { static_cast<int>(screenx), static_cast<int>(screeny) };
			route->origin.depth = -projz;
		}
		{
			Vec3 relative_pos = subtract_vectors(route->destin.absolute_pos, poscam);
			double projx = dot_product(relative_pos, vecx);
			double projy = dot_product(relative_pos, vecy);
			double projz = dot_product(relative_pos, vecz);
			double screenx = width * (0.5 - 0.5 * projx / projz);
			double screeny = height * (0.5 - 0.5 * width / double(height) * projy / projz);
			route->destin.screen_pos = { static_cast<int>(screenx), static_cast<int>(screeny) };
			route->destin.depth = -projz;
		}
		for (std::vector<Starship>::iterator ship = route->ships.begin(); ship != route->ships.end();) {
			if (ship->dir == 0) {
				ship->shippos = add_vectors(route->origin.absolute_pos, scalar_multiply((timeingame - ship->starttime) / (ship->endtime - ship->starttime) / (365 * 24 * 3600), (subtract_vectors(route->destin.absolute_pos, route->origin.absolute_pos))));
			} else {
				ship->shippos = add_vectors(route->destin.absolute_pos, scalar_multiply((timeingame - ship->starttime) / (ship->endtime - ship->starttime) / (365 * 24 * 3600), (subtract_vectors(route->origin.absolute_pos, route->destin.absolute_pos))));
			}
			Vec3 relative_pos = subtract_vectors(ship->shippos, poscam);
			double projx = dot_product(relative_pos, vecx);
			double projy = dot_product(relative_pos, vecy);
			double projz = dot_product(relative_pos, vecz);
			double screenx = width * (0.5 - 0.5 * projx / projz);
			double screeny = height * (0.5 - 0.5 * width / double(height) * projy / projz);
			ship->shippoint = { static_cast<int>(screenx), static_cast<int>(screeny) };
			ship->shipdep = -projz;
			if (ship->dir == 0 && vector_length(subtract_vectors(route->origin.absolute_pos, ship->shippos)) > vector_length(subtract_vectors(route->origin.absolute_pos, route->destin.absolute_pos))) {
				ship = route->ships.erase(ship);
			} else if (ship->dir == 1 && vector_length(subtract_vectors(route->destin.absolute_pos, ship->shippos)) > vector_length(subtract_vectors(route->origin.absolute_pos, route->destin.absolute_pos))) {
				ship = route->ships.erase(ship);
			} else {
				ship++;
			}
		}

		if (route->ships.size() == 0) {
			route = routes.erase(route);
		} else {
			std::sort(route->ships.begin(), route->ships.end(), [](const Starship& a, const Starship& b) {
				return a.shipdep < b.shipdep;
				});
			route++;
		}
	}
	bool fir = 1;
	for (auto& route : routes) {
		if (route.origin.number == controlstar.number || route.destin.number == controlstar.number) {
			route.show = 1;
		}
		bool frse = 0;
		bool emse = 0;
		bool frat = 0;
		bool emat = 0;
		if (route.destin.depth > 0 || route.origin.depth > 0) {
			if (route.destin.depth > 0 && route.origin.depth < 0) {
				route.origin.screen_pos.x = route.destin.screen_pos.x - (route.origin.screen_pos.x - route.destin.screen_pos.x) * double(route.destin.depth - route.origin.depth) / (route.destin.depth);
				route.origin.screen_pos.y = route.destin.screen_pos.y - (route.origin.screen_pos.y - route.destin.screen_pos.y) * double(route.destin.depth - route.origin.depth) / (route.destin.depth);
			}
			if (route.destin.depth < 0 && route.origin.depth > 0) {
				route.destin.screen_pos.x = route.origin.screen_pos.x - (route.destin.screen_pos.x - route.origin.screen_pos.x) * double(route.origin.depth - route.destin.depth) / (route.origin.depth);
				route.destin.screen_pos.y = route.origin.screen_pos.y - (route.destin.screen_pos.y - route.origin.screen_pos.y) * double(route.origin.depth - route.destin.depth) / (route.origin.depth);
			}

			bool mouson = isMouseNearLine(route.destin.screen_pos, route.origin.screen_pos);
			if (menubutton.ProcessEvent()) {
				mouson = 0;
			}
			if (!fir) {
				mouson = 0;
			}
			if (mouson) {
				fir = 0;
				if (!menubutton.ProcessEvent() && mousestorey <= 0) {
					std::string str = convertChar8ArrayToString(u8"航线，天体") + std::to_string(route.origin.number) + convertChar8ArrayToString(u8"到天体") + std::to_string(route.destin.number) + "\n" + convertChar8ArrayToString(u8"长:") + std::to_string(vector_length(subtract_vectors(route.origin.absolute_pos, route.destin.absolute_pos))) + convertChar8ArrayToString(u8"光年") + "\n" + convertChar8ArrayToString(u8"当前飞行物群") + std::to_string(route.ships.size()) + convertChar8ArrayToString(u8"个");
					hovermessage.ProcessEvent(str, height / 70.0, width, height);
				}
			}
			route.p = 0;
			int aaa, bbb;
			const Uint32 mou = SDL_GetMouseState(&aaa, &bbb);
			if (mouson == true && (mou & SDL_BUTTON(SDL_BUTTON_LEFT)) && mousestorey <= 0) {
				route.p = true;
			}
			if (!route.p && route.lp && mouson && mousestorey <= 0) {
				route.show = !route.show;

			}
			route.lp = route.p;

			for (auto& ship : route.ships) {
				if ((ship.dir == 0 && route.origin.teamname <= 0) || (ship.dir == 1 && route.destin.teamname <= 0)) {
					if (ship.category == 0) {
						frse = 1;
					} else if (ship.category == 1 || ship.category == 2) {
						frat = 1;
					}
				}
				if ((ship.dir == 0 && route.origin.teamname >=1) || (ship.dir == 1 && route.destin.teamname >=1)) {
					if (ship.category == 0) {
						emse = 1;
					} else if (ship.category == 1 || ship.category == 2) {
						emat = 1;
					}
				}

				if (ship.shipdep > 0 && ship.shippoint.x > 0 && ship.shippoint.x < width && ship.shippoint.y>0 && ship.shippoint.y < height && route.show) {
					SDL_Color color = { 0,0,0,0 };
					if (ship.category == 0) {
						color = { 0,100,255,0 };
					} else if (ship.category == 1) {
						color = { 200,0,0,0 };
					} else if (ship.category == 2) {
						color = { 0,255,0,0 };
					}
					drawFilledCircle(renderer, ship.shippoint.x, ship.shippoint.y, 2, { color.r,color.g,color.b,150 });
					double ll = std::min(450 / sqrt(variable_threshold00(pow(vector_length(subtract_vectors(ship.shippos, poscam)), 2) - 1)), 300.0) * width / 1920*(0.6+0.5*tanh(0.5*(log10(ship.loadmess)-10)));
					SDL_Texture* modified_image = tieimg[3];
					if (ll > height/3.0) {
						ll = height / 3.0;
					}
					if (ll > 128) {
						modified_image = tieimg[0];
					} else if (ll > 64) {
						modified_image = tieimg[1];
					} else if (ll > 32) {
						modified_image = tieimg[2];
					} else if (ll <= 32) {
						modified_image = tieimg[3];
					}
					SDL_Rect dest_rect = { int(ship.shippoint.x - ll / 2 + 1), int(ship.shippoint.y - ll / 2 + 1), int(ll), int(ll) };
					renderTextureWithColor(renderer, modified_image, { color.r,color.g,color.b,255 }, dest_rect);
					if ((aaa - ship.shippoint.x) * (aaa - ship.shippoint.x) + (bbb - ship.shippoint.y) * (bbb - ship.shippoint.y) < pow((std::max(ll / 8, 2.0)), 2) && !menubutton.ProcessEvent() && mousestorey <= 0) {
						std::string str = convertChar8ArrayToString(u8"飞行器集群，数量") + std::to_string(ship.number) + "\n" + convertChar8ArrayToString(u8"速度:") + std::to_string(ship.v) + convertChar8ArrayToString(u8"倍光速");
						hovermessage.ProcessEvent(str, height / 70.0, width, height);
					}
				}
			}//上为飞船下为航道
			if (route.show) {
				if (frse) {
					SDL_SetRenderDrawColor(renderer, 60, 179, 113, 200);//绿
				}
				if (emse) {
					SDL_SetRenderDrawColor(renderer, 0, 191, 255, 200);//青
				}

				if (frat) {
					SDL_SetRenderDrawColor(renderer, 153, 50, 204, 255);//紫
				}
				if (emat) {
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);//红
				}
				if ((frat || frse) && (emat || emse)) {
					SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
				}
			} else if (mouson) {
				if (frse) {
					SDL_SetRenderDrawColor(renderer, 60, 179, 113, 120);//绿
				}
				if (emse) {
					SDL_SetRenderDrawColor(renderer, 0, 191, 255, 120);//青
				}

				if (frat) {
					SDL_SetRenderDrawColor(renderer, 153, 50, 204, 150);//紫
				}
				if (emat) {
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 130);//红
				}
				if ((frat || frse) && (emat || emse)) {
					SDL_SetRenderDrawColor(renderer, 255, 215, 0, 130);
				}
			} else {
				if (frse) {
					SDL_SetRenderDrawColor(renderer, 60, 179, 113, 30);//绿
				}
				if (emse) {
					SDL_SetRenderDrawColor(renderer, 0, 191, 255, 30);//青
				}

				if (frat) {
					SDL_SetRenderDrawColor(renderer, 153, 50, 204, 41);//紫
				}
				if (emat) {
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 26);//红
				}
				if ((frat || frse) && (emat || emse)) {
					SDL_SetRenderDrawColor(renderer, 255, 215, 0, 26);
				}
			}
			SDL_RenderDrawLine(renderer, route.destin.screen_pos.x, route.destin.screen_pos.y, route.origin.screen_pos.x, route.origin.screen_pos.y);

		}
	}

}

void StarMap::draw_opoints() {

	for (size_t i = 1; i < opoints.size(); i++) {
		Opoint tpoint = opoints[i];
		Opoint lpoint = opoints[i - 1];
		if ((tpoint.screen_pos.x >= 0 && tpoint.screen_pos.x < width &&
			tpoint.screen_pos.y >= 0 && tpoint.screen_pos.y < height && tpoint.depth>0) && (lpoint.screen_pos.x >= 0 && lpoint.screen_pos.x < width &&
				lpoint.screen_pos.y >= 0 && lpoint.screen_pos.y < height && lpoint.depth>0)) {
			SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
			SDL_RenderDrawLine(renderer, lpoint.screen_pos.x, lpoint.screen_pos.y, tpoint.screen_pos.x, tpoint.screen_pos.y);

		}
	}
}

void StarMap::draw_info_panel() {

	if (messagex <= width) {
		int mouseXp, mouseYp;
		const Uint32 mop = SDL_GetMouseState(&mouseXp, &mouseYp);
		if (mouseXp > messagex) {
			hovermessage.ProcessEvent("", height / 70.0, width, height);
		}
		// 绘制信息面板背景
		SDL_Rect message_rect = { static_cast<int>(messagex), 0,
								 static_cast<int>(width * messagewidthrate), height };
		SDL_SetRenderDrawColor(renderer, 10, 10, 10, 200);
		SDL_RenderFillRect(renderer, &message_rect);



	
		if(planetmessage==0)//star
		{
			// 绘制星球图像
			int image_size = static_cast<int>(width * messagewidthrate);
			SDL_Rect image_rect = { static_cast<int>(messagex),
				0 + scroll_y, image_size, image_size * 2 / 3.0 };
			renderTextureWithColor(renderer, mesimg[rightclickstar.dysondensity], kelvin_to_rgb(rightclickstar.temperature), image_rect);

			SDL_RenderCopy(renderer, mesimg[rightclickstar.dysondensity], nullptr, &image_rect);

			// 信息配置
			std::string a = convertChar8ArrayToString(u8"恒星") + std::to_string(rightclickstar.number) + "," + "\n"
				//+ convertChar8ArrayToString(u8"光谱型:") + rightclickstar.StarData.GetStellarClass().ToString() + "\n"
				+ convertChar8ArrayToString(u8"演化阶段:") + PhaseToString(rightclickstar.StarData.GetEvolutionPhase()) + "\n"
				+ convertChar8ArrayToString(u8"半径:") + std::to_string(rightclickstar.radius / 0.000016) + convertChar8ArrayToString(u8"天文单位") + "\n"
				+ convertChar8ArrayToString(u8"功率:") + std::to_string(rightclickstar.power) + convertChar8ArrayToString(u8"倍太阳");
			int yy = detailed.DrawLeft(renderer, a, height / 64.0, width, height, static_cast<int>(messagex + 10) / double(width), (image_size * 2 / 3.0 + scroll_y) / height, "Assets/Fonts/FangZhengShuSongJianTi-1.ttf");
			if(rightclickstar.teamname==0)
			{
				if(rightclickstar.number!=controlstar.number)
				{
					if (control.ProcessEvent(messagex + 5, yy, width / 15.0, width / 15.0, 1)) {
						controlstar = rightclickstar;
						tocontrol = 1;
						control._bPressState = 0;
					}
					if (control._bMouseOnButton) {
						hovermessage.ProcessEvent(convertChar8ArrayToString(u8"前往控制该恒星,耗时") + std::to_string(vector_length(subtract_vectors(rightclickstar.absolute_pos, controlstar.absolute_pos))) + convertChar8ArrayToString(u8"年。"), height / 60.0, width, height);
						hovermessage.Draw(renderer, height / 60.0);
					}
					control.DrawButtonWithoutRect_grey(renderer);
					control.ProcessEvent(0);
					if (sendrkkv.ProcessEvent(messagex + 5, yy + width / 15.0, width / 15.0, width / 15.0, 1)) {
						bool dir;
						if (controlstar.number > rightclickstar.number) {
							dir = 1;
						} else {
							dir = 0;
						}
						Parameterofship aship{ controlstar.number, rightclickstar.number, 1, 1, dir, 0.8, pow(10,11), 0, 0,-1 };
						shipwaittotakeof.push_back(aship);
						sendrkkv._bPressState = 0;
					}
					if (sendrkkv._bMouseOnButton) {
						hovermessage.ProcessEvent(convertChar8ArrayToString(u8"向友方发射rkkv。注意，是友方！！！"), height / 30.0, width, height);
						hovermessage.Draw(renderer, height / 30.0);
					}
					sendrkkv.DrawButtonWithoutRect_grey(renderer);
					sendrkkv.ProcessEvent(0);
					if (sendship.ProcessEvent(messagex + 5, yy + 2 * width / 15.0, width / 15.0, width / 15.0, 1)) {
						bool dir;
						if (controlstar.number > rightclickstar.number) {
							dir = 1;
						} else {
							dir = 0;
						}
						Parameterofship aship{ controlstar.number, rightclickstar.number, 1, 0, dir, 0.4, pow(10,11), 0, 0,-1 };
						shipwaittotakeof.push_back(aship);
						sendship._bPressState = 0;
					}
					if (sendship._bMouseOnButton) {
						hovermessage.ProcessEvent(convertChar8ArrayToString(u8"向友方发射飞船。"), height / 60.0, width, height);
						hovermessage.Draw(renderer, height / 60.0);
					}
					sendship.DrawButtonWithoutRect_grey(renderer);
					sendship.ProcessEvent(0);
					if (sendlaser.ProcessEvent(messagex + 5, yy + 3 * width / 15.0, width / 15.0, width / 15.0, 1)) {
						bool dir;
						if (controlstar.number > rightclickstar.number) {
							dir = 1;
						} else {
							dir = 0;
						}
						Parameterofship aship{ controlstar.number, rightclickstar.number, 1, 2, dir, 1, pow(10,11), 0, 0,-1 };
						shipwaittotakeof.push_back(aship);
						sendlaser._bPressState = 0;
					}
					if (sendlaser._bMouseOnButton) {
						hovermessage.ProcessEvent(convertChar8ArrayToString(u8"向友方发射戴森光束。注意，是友方！！！"), height / 30.0, width, height);
						hovermessage.Draw(renderer, height / 30.0);
					}
					sendlaser.DrawButtonWithoutRect_grey(renderer);
					sendlaser.ProcessEvent(0);
				}
			}
			if (rightclickstar.teamname != 0) {
				
				if (sendrkkv.ProcessEvent(messagex + 5, yy + width / 15.0, width / 15.0, width / 15.0, 1)) {
					bool dir;
					if (controlstar.number > rightclickstar.number) {
						dir = 1;
					} else {
						dir = 0;
					}
					Parameterofship aship{ controlstar.number, rightclickstar.number, 1, 1, dir, 0.8, pow(10,11), 0, 0,-1 };
					shipwaittotakeof.push_back(aship);
					sendrkkv._bPressState = 0;
				}
				if (sendrkkv._bMouseOnButton) {
					hovermessage.ProcessEvent(convertChar8ArrayToString(u8"向敌方或未被占领星发射rkkv。"), height / 60.0, width, height);
					hovermessage.Draw(renderer, height / 60.0);
				}
				sendrkkv.DrawButtonWithoutRect_grey(renderer);
				sendrkkv.ProcessEvent(0);
				if (sendship.ProcessEvent(messagex + 5, yy + 2 * width / 15.0, width / 15.0, width / 15.0, 1)) {
					bool dir;
					if (controlstar.number > rightclickstar.number) {
						dir = 1;
					} else {
						dir = 0;
					}
					Parameterofship aship{ controlstar.number, rightclickstar.number, 1, 0, dir, 0.4, pow(10,11), 0, 0,-1 };
					shipwaittotakeof.push_back(aship);
					sendship._bPressState = 0;
				}
				if (sendship._bMouseOnButton) {
					hovermessage.ProcessEvent(convertChar8ArrayToString(u8"向敌方或未被占领星发射飞船。"), height / 60.0, width, height);
					hovermessage.Draw(renderer, height / 60.0);
				}
				sendship.DrawButtonWithoutRect_grey(renderer);
				sendship.ProcessEvent(0);
				if (sendlaser.ProcessEvent(messagex + 5, yy + 3 * width / 15.0, width / 15.0, width / 15.0, 1)) {
					bool dir;
					if (controlstar.number > rightclickstar.number) {
						dir = 1;
					} else {
						dir = 0;
					}
					Parameterofship aship{ controlstar.number, rightclickstar.number, 1, 2, dir, 1, pow(10,11), 0, 0,-1 };
					shipwaittotakeof.push_back(aship);
					sendlaser._bPressState = 0;
				}
				if (sendlaser._bMouseOnButton) {
					hovermessage.ProcessEvent(convertChar8ArrayToString(u8"向敌方或未被占领星发射戴森光束。"), height / 60.0, width, height);
					hovermessage.Draw(renderer, height / 60.0);
				}
				sendlaser.DrawButtonWithoutRect_grey(renderer);
				sendlaser.ProcessEvent(0);
			}

		} else {
			// 绘制星球图像
			int image_size = static_cast<int>(width * messagewidthrate);
			SDL_Rect image_rect = { static_cast<int>(messagex),
				0 + scroll_y, image_size, image_size * 2 / 3.0 };
			renderTextureWithColor(renderer, mesimg[0], {0,255,0,255}, image_rect);

			SDL_RenderCopy(renderer, mesimg[0], nullptr, &image_rect);

			// 信息配置
			std::string a = convertChar8ArrayToString(u8"行星") + std::to_string(rightclickplanet.number) ;
			int yy = detailed.DrawLeft(renderer, a, height / 64.0, width, height, static_cast<int>(messagex + 10) / double(width), (image_size * 2 / 3.0 + scroll_y) / height, "Assets/Fonts/FangZhengShuSongJianTi-1.ttf");
			if (rightclickstar.teamname == 0) {
				if (sendrkkv.ProcessEvent(messagex + 5, yy , width / 15.0, width / 15.0, 1)) {
					bool dir;
					if (controlstar.number > rightclickstar.number) {
						dir = 1;
					} else {
						dir = 0;
					}
					Parameterofship aship{ controlstar.number, rightclickstar.number, 1, 1, dir, 0.8, pow(10,11), 0, 0,rightclickplanet.number };
					shipwaittotakeof.push_back(aship);
					sendrkkv._bPressState = 0;
				}
				if (sendrkkv._bMouseOnButton) {
					hovermessage.ProcessEvent(convertChar8ArrayToString(u8"向友方发射rkkv。注意，是友方！！！"), height / 30.0, width, height);
					hovermessage.Draw(renderer, height / 30.0);
				}
				sendrkkv.DrawButtonWithoutRect_grey(renderer);
				sendrkkv.ProcessEvent(0);
				if (sendship.ProcessEvent(messagex + 5, yy + 1 * width / 15.0, width / 15.0, width / 15.0, 1)) {
					bool dir;
					if (controlstar.number > rightclickstar.number) {
						dir = 1;
					} else {
						dir = 0;
					}
					Parameterofship aship{ controlstar.number, rightclickstar.number, 1, 0, dir, 0.4, pow(10,11), 0, 0,rightclickplanet.number };
					shipwaittotakeof.push_back(aship);
					sendship._bPressState = 0;
				}
				if (sendship._bMouseOnButton) {
					hovermessage.ProcessEvent(convertChar8ArrayToString(u8"向友方发射飞船。"), height / 60.0, width, height);
					hovermessage.Draw(renderer, height / 60.0);
				}
				sendship.DrawButtonWithoutRect_grey(renderer);
				sendship.ProcessEvent(0);

			}
			if (rightclickstar.teamname != 0) {
				if (sendrkkv.ProcessEvent(messagex + 5, yy, width / 15.0, width / 15.0, 1)) {
					bool dir;
					if (controlstar.number > rightclickstar.number) {
						dir = 1;
					} else {
						dir = 0;
					}
					Parameterofship aship{ controlstar.number, rightclickstar.number, 1, 1, dir, 0.8, pow(10,11), 0, 0,rightclickplanet.number };
					shipwaittotakeof.push_back(aship);
					sendrkkv._bPressState = 0;
				}
				if (sendrkkv._bMouseOnButton) {
					hovermessage.ProcessEvent(convertChar8ArrayToString(u8"向敌方或未被占领星发射rkkv。"), height / 60.0, width, height);
					hovermessage.Draw(renderer, height / 60.0);
				}
				sendrkkv.DrawButtonWithoutRect_grey(renderer);
				sendrkkv.ProcessEvent(0);
				if (sendship.ProcessEvent(messagex + 5, yy + 1 * width / 15.0, width / 15.0, width / 15.0, 1)) {
					bool dir;
					if (controlstar.number > rightclickstar.number) {
						dir = 1;
					} else {
						dir = 0;
					}
					Parameterofship aship{ controlstar.number, rightclickstar.number, 1, 0, dir, 0.4, pow(10,11), 0, 0,rightclickplanet.number };
					shipwaittotakeof.push_back(aship);
					sendship._bPressState = 0;
				}
				if (sendship._bMouseOnButton) {
					hovermessage.ProcessEvent(convertChar8ArrayToString(u8"向敌方或未被占领星发射飞船。"), height / 60.0, width, height);
					hovermessage.Draw(renderer, height / 60.0);
				}
				sendship.DrawButtonWithoutRect_grey(renderer);
				sendship.ProcessEvent(0);

			}


		}

	} else {
		scroll_y = 0;
		targetscroll_y = 0;
	}
	lastrightname = rightclickstar.number;
}

void StarMap::draw_time(double atime) {
	std::string timemessage = formatTime(atime);
	showtimeingame.Draw(renderer, timemessage, height / 32.0, width, height, 0.5, 0, "Assets/Fonts/FileDeletion.ttf");
}

void StarMap::cleanup() {
	exitbutton._bPressState = 0;
	menubutton._bPressState = 0;
	stop._bPressState = 1;
	ifexit = 0;
	ifsave = 0;
	stars.clear();
	stars.shrink_to_fit();
	routes.clear();
	routes.shrink_to_fit();
	star_messages.clear();

	timeingame = 0;
	timerate = 1;
	month = 0;
	lastmonth = 0;

	mousestorey = 0;

	cenposcam = Vec3(0, 0, 0);//对应11111
	reposcam = Vec3(0, 0, 0);
	poscam = Vec3(0, 0, 0);
	vecx = Vec3(0, 0, 0);
	vecy = Vec3(0, 0, 0);
	vecz = Vec3(0, 0, 0);
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


	zvectheta = 0;
	zvecphi = 0;
	cirtheta = 0;
	cirphi = 0;

	lastrightname = -1;
	totar = false;
	text = "";//确定
	toplanet = false;
	onplanet = false;
	followplanet = -1;

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
	showorbit = 1;
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

#pragma endregion

