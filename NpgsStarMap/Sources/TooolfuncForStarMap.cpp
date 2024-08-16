#include "TooolfuncForStarMap.h"
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

	return { static_cast<Uint8>(red), static_cast<Uint8>(green), static_cast<Uint8>(blue), 255 };
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


bool isMouseNearLine(SDL_Point p1, SDL_Point p2) {
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	int x1 = p1.x;
	int y1 = p1.y;
	int x2 = p2.x;
	int y2 = p2.y;

	// 计算线段的长度
	float length = std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));

	// 计算单位向量
	float unitX = (x2 - x1) / length;
	float unitY = (y2 - y1) / length;

	// 计算垂直于线段的单位向量
	float perpX = -unitY;
	float perpY = unitX;

	// 计算点到直线的距离
	float dist = std::abs((mx - x1) * perpX + (my - y1) * perpY);

	// 如果距离小于等于 5 像素，并且点在线段的范围内，则返回 true
	if (dist <= 5.0f) {
		// 确定点在线段的范围内
		float dotProduct = (mx - x1) * unitX + (my - y1) * unitY;
		if (dotProduct >= 0 && dotProduct <= length) {
			return true;
		}
	}

	return false;
}
