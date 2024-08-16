#pragma once

#include "Vec3/Vec3.h"
#include <SDL2/SDL.h>

struct Star {//星星所有数据
	int number;//编号，不是天文学命名，就是编号，每个恒星唯一
	Vec3 absolute_pos;//绝对坐标
	Vec3 relative_pos;//可视化中间量，不能去掉
	double distance;//可视化中间量，不能去掉
	double depth;//可视化中间量，不能去掉
	SDL_Point screen_pos;//可视化中间量，不能去掉
	double temperature;//温度，开尔文
	double power;//功率，倍太阳
	int dysondensity;//戴森云等级
	double radius;//恒星半径，光年
	std::string teamname;//所在阵营名，考虑换成int，把队名存在一个词典里
	bool needtoshowpos;//是否显示垂线，不能去掉
	SDL_Point zpoint;//垂线相关，不能去掉
	double zpdep;//垂线相关，不能去掉
	int type;//0恒星，1星云（现在这俩共用一个struct

	//  Npgs::AstroObject::Star StarData;
};

struct Opoint {//其他标记点
	Vec3 absolute_pos;
	Vec3 relative_pos;
	double distance;
	double depth;
	SDL_Point screen_pos;

};

struct Starship {
	int number;
	int category;//0飞船，1rkkv,2laser
	bool dir;//方向，0正1负,正意味从小编号到大编号
	double loadmess;
	double fuelmess;
	double volatilesmess;
	double starttime;
	double endtime;
	Vec3 shippos;
	SDL_Point shippoint;
	double shipdep;
	double v;


	Starship() = default;

	Starship(int number, int category, bool dir, double endtime, double loadmess, double fuelmess, double volatilesmess, double starttime, double v)
		: number(number), category(category), dir(dir), endtime(endtime), loadmess(loadmess), fuelmess(fuelmess), volatilesmess(volatilesmess), starttime(starttime), v(v) {}
};

struct Route {//后端/决策器输入时请将恒星编号小者作为起点，定义飞船顺逆方向
	Star origin;
	Star destin;
	std::vector<Starship> ships;
	bool show;
	bool p;
	bool lp;

	Route(const Star& origin, const Star& destin)
		: origin(origin), destin(destin) {}

	Route() = default;
};

