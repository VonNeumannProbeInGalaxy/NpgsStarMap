#include "TooolfuncForStarMap.h"
#include <cmath>
#include <algorithm>
#include <Utils/Vec3.h>

SDL_Color ScaleSDLColor(double x, const SDL_Color& color) {//rgb数乘,带过曝处理
	SDL_Color newColor;

	newColor.r = static_cast<Uint8>(std::min(color.r * x, 255.0));  // 缩放并限制在 [0, 255]
	newColor.g = static_cast<Uint8>(std::min(color.g * x, 255.0));  // 缩放并限制在 [0, 255]
	newColor.b = static_cast<Uint8>(std::min(color.b * x, 255.0));  // 缩放并限制在 [0, 255]
	newColor.a = color.a;  // 保持 alpha 不变

	return newColor;
}

SDL_Color kelvin_to_rgb(double T) {

	double _ = (T - 6500.0) / (6500.0 * T * 2.2);
	double R = std::exp(2.05539304255011812e+04 * _);
	double G = std::exp(2.63463675111716257e+04 * _);
	double B = std::exp(3.30145738821726118e+04 * _);
	double LmulRate = 1.0 / std::max(std::max(R, G), B);
	LmulRate*= 255.999999;
	R *= LmulRate;
	G *= LmulRate;
	B *= LmulRate;

	Uint8 r = static_cast<Uint8>(R);
	Uint8 g = static_cast<Uint8>(G);
	Uint8 b = static_cast<Uint8>(B);

	SDL_Color color = { r, g, b, 255 }; 
	return color;
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
std::string convertChar8ArrayToString(const char8_t* char8Array) {
	std::u8string u8str(reinterpret_cast<const char8_t*>(char8Array));


	return std::string(reinterpret_cast<const char*>(u8str.c_str()));
}
Vec3 vec90rotate(double the, double theta, double phi) {
	double CA = cos(the);
	double SA = sin(the);
	double CO = cos(phi);
	double SO = sin(phi);
	double CT = cos(theta);
	double ST = sin(theta);
	double SO2 = sin(phi / 2.0);
	double S2T = sin(2 * theta);
	Vec3 result(
	CA*(CO*CT*CT+ST*ST)-SA*SO2*SO2*S2T,
	SA*(CT*CT+CO*ST*ST)-CA*SO2*SO2*S2T,
	-CA*CT*SO-SA*SO*ST
	);
	return result;

}
Vec3 vecrotate(Vec3 v, double theta, double phi) {
	
	double CO = cos(phi);
	double SO = sin(phi);
	double CT = cos(theta);
	double ST = sin(theta);
	double SO2 = sin(phi / 2.0);
	double S2T = sin(2 * theta);
	Vec3 result(
    v.z*CT*SO+v.x*(CO*CT*CT+ST*ST)- v.y * SO2 * SO2 * S2T,
	v.z*SO*ST+v.y * (CT * CT + CO * ST * ST) - v.x * SO2 * SO2 * S2T,
	v.z*CO- v.x * CT * SO - v.y * SO * ST
	);
	return result;
}


void vec1to2(double& theta, double& phi, double theta0, double phi0,double a) {
	Vec3 v(cos(theta)*sin(phi),sin(theta)*sin(phi),cos(phi));
	Vec3 v0(cos(theta0) * sin(phi0), sin(theta0) * sin(phi0), cos(phi0));
	if (vector_length(subtract_vectors(v0, v)) > 0) {
		v = normalize_vector(add_vectors(v, scalar_multiply(a * 2 * asin(vector_length(subtract_vectors(v0, v)) / 2), normalize_vector(cross_product(cross_product(v, subtract_vectors(v0, v)), v)))));

		phi = acos(v.z);
		theta = acos(v.x / sin(phi));
	}
	phi = std::clamp(phi, 0.0, PI);
	theta = std::clamp(theta, 0.0, PI);
	if (std::isnan(theta)) { theta = 0; }
	if (std::isnan(phi)) { phi = 0; }
}
std::string formatTime(double seconds) {
	// Define the units in seconds
	const double yearInSeconds = 31536000;  // 365 days
	const double monthInSeconds = 2628000;  // 30 days
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
		result += std::to_string(months+1) + " months ";
	}
	if (days > 0) {
		result += std::to_string(days) + " days ";
	}
	result += std::to_string(remainingSeconds) + " s ";


	return result;
}

void drawDashedLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, double dashLength) {

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

std::string PhaseToString(Npgs::AstroObject::Star::Phase phase) {
	switch (phase) {
	case Npgs::AstroObject::Star::Phase::kPrevMainSequence: return          convertChar8ArrayToString(u8"前主序");
	case Npgs::AstroObject::Star::Phase::kMainSequence: return              convertChar8ArrayToString(u8"主序星");
	case Npgs::AstroObject::Star::Phase::kRedGiant: return                  convertChar8ArrayToString(u8"红巨星");
	case Npgs::AstroObject::Star::Phase::kCoreHeBurn: return                convertChar8ArrayToString(u8"氦核燃烧");
	case Npgs::AstroObject::Star::Phase::kEarlyAgb: return                  convertChar8ArrayToString(u8"早期渐近巨星");
	case Npgs::AstroObject::Star::Phase::kThermalPulseAgb: return           convertChar8ArrayToString(u8"热脉冲渐近巨星");
	case Npgs::AstroObject::Star::Phase::kPostAgb: return                   convertChar8ArrayToString(u8"后渐近巨星支");
	case Npgs::AstroObject::Star::Phase::kWolfRayet: return                 convertChar8ArrayToString(u8"沃尔夫-拉叶星");
	case Npgs::AstroObject::Star::Phase::kHeliumWhiteDwarf: return          convertChar8ArrayToString(u8"氦白矮星");
	case Npgs::AstroObject::Star::Phase::kCarbonOxygenWhiteDwarf: return    convertChar8ArrayToString(u8"碳氧白矮星");
	case Npgs::AstroObject::Star::Phase::kOxygenNeonMagnWhiteDwarf: return  convertChar8ArrayToString(u8"氧氖镁白矮星");
	case Npgs::AstroObject::Star::Phase::kNeutronStar: return               convertChar8ArrayToString(u8"中子星");
	case Npgs::AstroObject::Star::Phase::kBlackHole: return                 convertChar8ArrayToString(u8"恒星质量黑洞");
	case Npgs::AstroObject::Star::Phase::kNull: return                      convertChar8ArrayToString(u8"kNull");
	}
}