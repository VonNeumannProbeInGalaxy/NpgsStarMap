#include "StarMap.h"

StarMap::StarMap(int w, int h) : width(w), height(h), running(true) {//屏幕初始化

	init_SDL();
	TTF_Init();
	load_textures();
	generate_opoints();

	hovermessage = UI::HoverMessage();
	menubutton = UI::Button(0, 0, 100, 100, 1, { 100,100,100 ,255 }, "menu");
	coordinate = UI::Button(0, 0, 100, 100, 0, { 100,100,100 ,255 }, "coordinate");
	exitbutton = UI::Button(0, 0, 100, 100, 0, { 100,100,100 ,255 }, "exit&save");
	addtimerate = UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "->");
	subtimerate = UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "<-");
	ShipAndRKKV = UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "ShipAndRKKV");
	stop = UI::Button(0, 0, 100, 100, 0, { 100,100,100,255 }, "||/>");
	stop._bPressState = 1;
	timeingame = 0;
	timerate = 1;

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
			hovermessage.ProcessEvent(u8"aa启动\nsdhusdhuudh\nsh\naaaaaaaaaaaaaaaaaaaaaaaaaaa", height / 32.0, width, height);
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
				hovermessage.ProcessEvent(u8"存档" + std::to_string(n) + u8"将被永久删除\n下一个庞加莱回归见。", height / 32.0, width, height);
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

static void drawDashedLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, double dashLength) {

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

static std::string formatTime(double seconds) {
	// Define the units in seconds
	const double yearInSeconds = 31536000;  // 365 days
	const double monthInSeconds = 2592000;  // 30 days
	const double dayInSeconds = 86400;


	// Calculate years
	long long years = (seconds / yearInSeconds);
	seconds -= years * yearInSeconds;

	// Calculate months
	long long months = (seconds / monthInSeconds);
	seconds -= months * monthInSeconds;

	// Calculate days
	long long days = (seconds / dayInSeconds);
	seconds -= days * dayInSeconds;


	// Seconds left
	int remainingSeconds = static_cast<int>(seconds);

	// Prepare the formatted string
	std::string result;
	if (years > 0) {
		result += std::to_string(years) + " years ";
	}
	if (months > 0) {
		result += std::to_string(months) + " months ";
	}
	if (days > 0) {
		result += std::to_string(days) + " days ";
	}
	result += std::to_string(remainingSeconds) + " s ";


	return result;
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

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-rmap0, rmap0);
	std::uniform_real_distribution<> temp_dis(1000, 12000);
	std::uniform_real_distribution<> power_dis(0, 1);
	std::uniform_int_distribution<> type_dis(0, 4);
	std::uniform_real_distribution<> radius_dis(-3.3, -1);
	std::cout << nstar0 << std::endl;
	std::cout << rmap0 << std::endl;
	int nnn = 0;
	for (int i = 0; i < nstar0; i++) {
		double xstar0 = dis(gen);
		double ystar0 = dis(gen);
		double zstar0 = dis(gen);
		bool aaa = false;
		if (xstar0 * xstar0 + ystar0 * ystar0 + zstar0 * zstar0 < rmap0 * rmap0) {
			std::string name = random_name();
			std::string tn = "fr";
			if (i % 2 == 0) {
				tn = "em";
			}
			star_messages[i] = name + std::to_string(i) + u8"s introduction\ntestline, 114514中w";
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
				tn,
				aaa,
				{0, 0},
				0,
				0
			};
			stars.push_back(astar);
			nnn = i;
		} else {
			i -= 1;
		}
	}
	add_ship_into_route(1, 2, 1, 0, 0, 0.7, 0, 0, 0);
	add_ship_into_route(1, 2, 1, 1, 0, 0.8, 0, 0, 0);
	add_ship_into_route(1, 2, 1, 2, 0, 1, 0, 0, 0);
	add_ship_into_route(1, 2, 1, 0, 1, 0.7, 0, 0, 0);
	add_ship_into_route(1, 2, 1, 1, 1, 0.8, 0, 0, 0);
	add_ship_into_route(1, 2, 1, 2, 1, 1, 0, 0, 0);
	add_ship_into_route(1, 4, 1, 0, 0, 0.7, 0, 0, 0);
	add_ship_into_route(1, 4, 1, 1, 0, 0.8, 0, 0, 0);
	add_ship_into_route(1, 4, 1, 2, 0, 1, 0, 0, 0);
	add_ship_into_route(2, 3, 1, 0, 0, 0.7, 0, 0, 0);
	add_ship_into_route(2, 3, 1, 1, 0, 0.8, 0, 0, 0);
	add_ship_into_route(2, 3, 1, 2, 0, 1, 0, 0, 0);
	std::uniform_int_distribution<> n_dis(0, nnn);
	std::uniform_int_distribution<> cat_dis(0, 2);
	std::uniform_int_distribution<> dir_dis(0, 1);
	for (int i = 0; i < 10; i++) {
		add_ship_into_route(n_dis(gen), n_dis(gen), 1, cat_dis(gen), dir_dis(gen), power_dis(gen), 0, 0, 0);
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
	std::vector<double> dist = { 0.05,0.1,0.2,0.4,0.8,6.4,51.2,409.6,1638.5,3276.8 };
	for (int i = 0; i < 10; i += 1) {
		double r = dist[i];
		tstar = {
			i + 1,
			Vec3(r * cos(PI / 9 * i), r * sin(PI / 9 * i), 0),
			Vec3(), 0,0, {0, 0},
			7000,
			10000,
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

}

void StarMap::generate_nebula() {
	const int nstar0 = 2;
	const double rmap0 = 100;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-rmap0, rmap0);

	std::uniform_real_distribution<> radius_dis(0, 1);
	std::cout << std::ceil(nstar0 * 6 / PI) << std::endl;
	std::cout << rmap0 << "a" << std::endl;
	for (int i = 0; i < std::ceil(nstar0 * 6 / PI); ++i) {
		double xstar0 = dis(gen);
		double ystar0 = dis(gen);
		double zstar0 = dis(gen);
		bool aaa = false;
		if (xstar0 * xstar0 + ystar0 * ystar0 + zstar0 * zstar0 < rmap0 * rmap0) {
			std::string name = random_name();
			star_messages[i] = name + std::to_string(i) + "s introduction\ntestline, 114514w";

			Star nebula = {
				1919810 + i,
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
			Opoint apoint = {
				Vec3(r * std::cos(the), r * std::sin(the), 0),
				Vec3(), 0, 0,{0, 0}
			};
			opoints.push_back(apoint);

		}
	}
	for (double the = 0; the < 2 * PI; the += 2 * PI / 8) {
		for (int r = 0; r < 100; r += 1) {
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
			Star star;

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

	bool aa = !menubutton.ProcessEvent(0, 0, 0.1 * width, 0.1 * height, 1) && !menubutton._bMouseOnButton;

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
		} else if (event.type == SDL_MOUSEMOTION && !menubutton.ProcessEvent(0, 0, 0.1 * width, 0.1 * height, 1)) {
			handle_mouse_motion(event);
		}
	}

}

void StarMap::handle_mouse_wheel(SDL_Event& event) {//页面内摄影机距离和页面外侧边栏滚动

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	if (!menubutton.ProcessEvent(0, 0, 0.1 * width, 0.1 * height, 1) && !menubutton._bMouseOnButton) {
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
	} else if (menubutton.ProcessEvent(0, 0, 0.1 * width, 0.1 * height, 1)) {
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
	update_camera();
	update_stars();
	if (showcircle == 1) {
		update_opoints();
	}
	if (ShipAndRKKV._bPressState) {
		updata_ship();
	}
	// 更新消息面板
	if (showmessage) {
		messagextarget = width * (1 - messagewidthrate);
	} else {
		messagextarget = 1.1 * width;
	}
	messagex += (messagextarget - messagex) * variable_threshold1(18 * deltatime);

	// 更新滚动
	scroll_y += (targetscroll_y - scroll_y) * variable_threshold1(18 * deltatime);
	if (menubutton.ProcessEvent(0, 0, 0.1 * width, 0.1 * height, 1)) {
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
	} else if (posx < 0) {
		posx += width;
	}
	if (posy > height) {
		posy = height;
	} else if (posy < 0) {
		posy = 0;
	}
	theta = posx / width * 4 * PI;
	phi = 0.99 * (posy / height - 0.5) * PI;

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

void StarMap::add_ship_into_route(int number1, int number2, int numberofship, int cat, bool dir, double v, double m1, double m2, double m3) {
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
		if (std::find_if(stars.begin(), stars.end(), [sm](const Star& d) { return d.number == sm; }) != stars.end() && std::find_if(stars.begin(), stars.end(), [bm](const Star& d) { return d.number == bm; }) != stars.end()) {
			auto it = std::find_if(routes.begin(), routes.end(),
				[sm, bm](const Route& d) {
					return d.origin.number == sm && d.destin.number == bm;
				});
			Star astar = stars[std::distance(stars.begin(), std::find_if(stars.begin(), stars.end(), [sm](const Star& d) { return d.number == sm; }))];//需要处理不存在恒星。。。。但我暂时懒得
			Star bstar = stars[std::distance(stars.begin(), std::find_if(stars.begin(), stars.end(), [bm](const Star& d) { return d.number == bm; }))];
			double dis = vector_length(subtract_vectors(astar.absolute_pos, bstar.absolute_pos));
			Starship ship = Starship(numberofship, cat, dir, timeingame + dis / v, m1, m2, m3, timeingame, v);
			if (it != routes.end()) {

				it->ships.push_back(ship);
			} else {
				Star ostar = stars[std::distance(stars.begin(), std::find_if(stars.begin(), stars.end(), [sm](const Star& d) { return d.number == sm; }))];//同上
				Star dstar = stars[std::distance(stars.begin(), std::find_if(stars.begin(), stars.end(), [bm](const Star& d) { return d.number == bm; }))];
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
		double ll = ll = 90 * pow(variable_threshold00(0.7568915 - 0.08115159 * log(star.distance)), 3) * pow(star.power, 1 / 7.0) * width / 1920;
		if (ll > height / 1.5) {
			ll = height / 1.5;
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
		if (star.screen_pos.x >= 0 && star.screen_pos.x < width &&
			star.screen_pos.y >= 0 && star.screen_pos.y < height && star.depth>0 && (mouseX < (messagex))) {
			if ((buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) && star.type == 0 && !menubutton.ProcessEvent(0, 0, 0.1 * width, 0.1 * height, 1) && !menubutton._bMouseOnButton) {
				if ((mouseX - screenx) * (mouseX - screenx) + (mouseY - screeny) * (mouseY - screeny) < pow((std::max(ll / 8, starrad)), 2)) {
					nearstars++;
					showmessage = true;
					targetname = star.number;
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
			if ((mouseX - screenx) * (mouseX - screenx) + (mouseY - screeny) * (mouseY - screeny) < pow((std::max(ll / 8, starrad)), 2) && star.type == 0 && !menubutton.ProcessEvent(0, 0, 0.1 * width, 0.1 * height, 1)) {
				std::string str = u8"恒星" + std::to_string(star.number) + "\n" + u8"半径:" + std::to_string(star.radius / 0.000016) + u8"天文单位" + "\n" + u8"功率:" + std::to_string(star.power) + u8"倍太阳";
				hovermessage.ProcessEvent(str, height / 70.0, width, height);
			}
		}

	}



}

void StarMap::update_opoints() {
	for (auto& opoint : opoints) {
		Vec3 relative_pos = add_vectors(subtract_vectors(opoint.absolute_pos, poscam), circen);
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

void StarMap::updata_ship() {
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
}

void StarMap::render() {
	static bool lastopen;
	static bool open;
	open = menubutton.ProcessEvent(0, 0, 0.1 * width, 0.1 * height, 1);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);



	SDL_RenderClear(renderer);


	if (showcircle == 1) {
		draw_opoints();
	}
	if (ShipAndRKKV._bPressState) {
		draw_ships();
	}
	draw_stars();


	draw_time(timeingame);

	if (open) {
		SDL_Rect backgroundrect = { 0,0,width,height };

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

		SDL_RenderFillRect(renderer, &backgroundrect);
		showcircle = coordinate.ProcessEvent(0.2 * width, 0.3 * height + menuscolly, 0.2 * width, 0.07 * height, 1);
		ifexit = exitbutton.ProcessEvent(0.6 * width, 0.3 * height + menuscolly, 0.2 * width, 0.07 * height, 1);
		ShipAndRKKV.ProcessEvent(0.2 * width, 0.5 * height + menuscolly, 0.2 * width, 0.07 * height, 1);


		addtimerate.ProcessEvent(0.2 * width, 0.3 * height, 0.1 * width, 0.07 * height, 0);
		stop.ProcessEvent(0.85 * width, 0, 0.1 * width, 0.07 * height, 0);
		subtimerate.ProcessEvent(0.3 * width, 0.3 * height, 0.1 * width, 0.07 * height, 0);


		showvertical = showcircle;

	}
	if (!open) {
		coordinate.ProcessEvent(0.2 * width, 0.3 * height + menuscolly, 0.2 * width, 0.07 * height, 0);
		exitbutton.ProcessEvent(0.6 * width, 0.3 * height + menuscolly, 0.2 * width, 0.07 * height, 0);
		if (addtimerate.ProcessEvent(0.95 * width, 0, 0.05 * width, 0.07 * height, 1)) {
			addtimerate._bPressState = 0;
			timerate *= 1.584893;
		}
		if (subtimerate.ProcessEvent(0.8 * width, 0, 0.05 * width, 0.07 * height, 1)) {
			subtimerate._bPressState = 0;
			timerate /= 1.584893;
		}
		stop.ProcessEvent(0.85 * width, 0, 0.1 * width, 0.07 * height, 1);
		std::string ratetext = std::to_string(timerate);//时间显示
		SDL_Color text_color = { 255, 255, 255, 255 };
		TTF_Font* font = TTF_OpenFont("Assets/Fonts/FileDeletion.ttf", height / 32.0);
		SDL_Surface* surface = TTF_RenderText_Blended(font, ratetext.c_str(), text_color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		int text_width = surface->w;//单行字尺寸
		int text_height = surface->h;
		SDL_FreeSurface(surface);

		SDL_Rect text_rect = { width * 0.9 - 0.5 * text_width,0.07 * height, text_width, text_height };
		SDL_RenderCopy(renderer, texture, nullptr, &text_rect);
		SDL_DestroyTexture(texture);

		TTF_CloseFont(font);
		ShipAndRKKV.ProcessEvent(0.2 * width, 0.5 * height + menuscolly, 0.2 * width, 0.07 * height, 0);
	}
	menubutton.DrawButtonRelease_TextLeftAndLineDown(renderer);
	coordinate.DrawButtonRelease_TextLeftAndLineDown(renderer);
	exitbutton.DrawButtonRelease_TextLeftAndLineDown(renderer);
	addtimerate.DrawButtonRelease_TextLeftAndLineDown(renderer);
	subtimerate.DrawButtonRelease_TextLeftAndLineDown(renderer);
	ShipAndRKKV.DrawButtonRelease_TextLeftAndLineDown(renderer);
	stop.DrawButtonRelease_TextLeftAndLineDown(renderer);
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



void StarMap::draw_stars() {
	for (auto& star : stars) {
		if (star.screen_pos.x >= 0 && star.screen_pos.x < width &&
			star.screen_pos.y >= 0 && star.screen_pos.y < height && star.depth>0 && star.type == 0) {
			SDL_Color color = ScaleSDLColor(pow(star.power, 1.0 / 8.0) / 8.0 / (variable_threshold001(100 * (star.distance)) / 100), kelvin_to_rgb(star.temperature));
			SDL_Color color0 = kelvin_to_rgb(star.temperature);
			double  starrad = width / sqrt(variable_threshold001(pow((star.distance / star.radius), 2) - 1));
			double ll = 90 * pow(0.7568915 - 0.08115159 * log(star.distance), 3) * pow(star.power, 1 / 7.0) * width / 1920;
			if (ll < 0.05) {
				ll = 0.05 / (2 - 20 * ll);
			}
			if (starrad >= 1) {
				drawFilledCircle(renderer, star.screen_pos.x, star.screen_pos.y, int(starrad), color0); //恒星本体，原色
			} else {
				SDL_Color acolor = ScaleSDLColor(pow(variable_threshold1(ll), 0.2), color0);
				SDL_SetRenderDrawColor(renderer, acolor.r, acolor.g, acolor.b, acolor.a);
				SDL_RenderDrawPoint(renderer, star.screen_pos.x, star.screen_pos.y);//处理过远不显示的情况
			}
			SDL_Color AA = { 0, 0, 0, 255 };

			//星星
			//星芒
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


			if (ll > 1) {
				SDL_Rect dest_rect = { int(star.screen_pos.x - ll / 2 + 1), int(star.screen_pos.y - ll / 2 + 1), int(ll), int(ll) };
				renderTextureWithColor(renderer, modified_image, AA, dest_rect);



			}
		}
		if ((star.depth > 0 || star.zpdep > 0) && star.needtoshowpos == true && star.type == 0 && showvertical == 1) {
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
			} else if ((circen.z - poscam.z) / (circen.z - star.absolute_pos.z) < 0) {
				SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
				drawDashedLine(renderer, star.screen_pos.x, star.screen_pos.y, star.zpoint.x, star.zpoint.y, 10);
			}
		}

		if (star.type == 1 && star.depth > 0) {
			double ll = std::min((width / sqrt(variable_threshold00(pow((star.distance / star.radius), 2) - 1))), double(2 * width)) * width / 1920;
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
			drawFilledCircle(renderer, star.screen_pos.x, star.screen_pos.y, ll, { 200,0,200,50 });
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


		}

	}
}

void StarMap::draw_ships() {

	bool fir = 1;
	for (auto& route : routes) {
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
				if (!menubutton.ProcessEvent()) {
					std::string str = u8"航线，天体" + std::to_string(route.origin.number) + u8"到天体" + std::to_string(route.destin.number) + "\n" + u8"长:" + std::to_string(vector_length(subtract_vectors(route.origin.absolute_pos, route.destin.absolute_pos))) + u8"光年" + "\n" + u8"当前飞行物" + std::to_string(route.ships.size()) + u8"个";
					hovermessage.ProcessEvent(str, height / 70.0, width, height);
				}
			}
			route.p = 0;
			int aaa, bbb;
			const Uint32 mou = SDL_GetMouseState(&aaa, &bbb);
			if (mouson == true && (mou & SDL_BUTTON(SDL_BUTTON_LEFT))) {
				route.p = true;
			}
			if (!route.p && route.lp && mouson) {
				route.show = !route.show;

			}
			route.lp = route.p;

			for (auto& ship : route.ships) {
				if ((ship.dir == 0 && route.origin.teamname == "fr") || (ship.dir == 1 && route.destin.teamname == "fr")) {
					if (ship.category == 0) {
						frse = 1;
					} else if (ship.category == 1 || ship.category == 2) {
						frat = 1;
					}
				}
				if ((ship.dir == 0 && route.origin.teamname == "em") || (ship.dir == 1 && route.destin.teamname == "em")) {
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
					double ll = std::min(450 / sqrt(variable_threshold00(pow(vector_length(subtract_vectors(ship.shippos, poscam)), 2) - 1)), 300.0) * width / 1920;
					SDL_Texture* modified_image = tieimg[3];
					if (ll > 1024) {
						ll = 1024;
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
					if ((aaa - ship.shippoint.x) * (aaa - ship.shippoint.x) + (bbb - ship.shippoint.y) * (bbb - ship.shippoint.y) < pow((std::max(ll / 8, 2.0)), 2) && !menubutton.ProcessEvent()) {
						std::string str = u8"飞行器" + std::to_string(ship.number) + "\n" + u8"速度:" + std::to_string(ship.v) + u8"倍光速";
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

void StarMap::draw_info_panel() {//报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

	if (messagex <= width) {
		// 绘制信息面板背景
		SDL_Rect message_rect = { static_cast<int>(messagex), 0,
								 static_cast<int>(width * messagewidthrate), height };
		SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
		SDL_RenderFillRect(renderer, &message_rect);



		// 绘制星球图像

		int image_size = static_cast<int>(width * messagewidthrate);
		SDL_Rect image_rect = { static_cast<int>(messagex),
			0 + scroll_y, image_size, image_size * 2 / 3.0 };
		renderTextureWithColor(renderer, mesimg[targetcloud], targetcolor, image_rect);

		SDL_RenderCopy(renderer, mesimg[targetcloud], nullptr, &image_rect);

		// 信息配置
		std::string aname = std::to_string(targetname);
		SDL_Color text_color = { 255, 255, 255, 255 };
		TTF_Font* font = TTF_OpenFont("Assets/Fonts/SJbangkaijianti.ttf", height / 32.0);
		SDL_Surface* surface = TTF_RenderText_Blended(font, aname.c_str(), text_color);//函数未定义
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		int text_width = surface->w;//单行字尺寸
		int text_height = surface->h;
		SDL_FreeSurface(surface);

		SDL_Rect text_rect = { static_cast<int>(messagex + 10), image_size * 2 / 3.0 + scroll_y, text_width, text_height };
		SDL_RenderCopy(renderer, texture, nullptr, &text_rect);
		SDL_DestroyTexture(texture);

		// 绘制详细信息
		std::vector<std::string> info_lines = UI::SplitStr(star_messages[targetname], '\n');
		int y_offset = text_height + 30 + image_size * 2 / 3.0 + scroll_y;
		if (!(info_lines.size() == 1 && info_lines[0] == "")) {
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
		}
		TTF_CloseFont(font);
	}
	lasttargetname = targetname;
}

void StarMap::draw_time(double atime) {
	std::string timemessage = formatTime(atime);
	SDL_Color text_color = { 255, 255, 255, 255 };
	TTF_Font* font = TTF_OpenFont("Assets/Fonts/FileDeletion.ttf", height / 32.0);
	SDL_Surface* surface = TTF_RenderText_Blended(font, timemessage.c_str(), text_color);//函数未定义
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	int text_width = surface->w;//单行字尺寸
	int text_height = surface->h;
	SDL_FreeSurface(surface);

	SDL_Rect text_rect = { width * 0.5 - 0.5 * text_width,0, text_width, text_height };
	SDL_RenderCopy(renderer, texture, nullptr, &text_rect);
	SDL_DestroyTexture(texture);

	TTF_CloseFont(font);
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

#pragma endregion

