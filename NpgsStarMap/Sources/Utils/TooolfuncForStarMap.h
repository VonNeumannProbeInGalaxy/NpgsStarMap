#pragma once
#include <SDL2/SDL.h>
#include <cmath>
#include <algorithm>
#include <Utils/Vec3.h>
#define NOMINMAX
#include <Npgs/Npgs.h>

const double PI = 3.141592653589793238;


SDL_Color ScaleSDLColor(double x, const SDL_Color& color);
SDL_Color kelvin_to_rgb(double T);
void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color);
double variable_threshold001(double variable);
double variable_threshold00(double variable);
double variable_threshold1(double variable);
std::string random_name();
bool isMouseNearLine(SDL_Point p1, SDL_Point p2);
std::string convertChar8ArrayToString(const char8_t* char8Array);
Vec3 vec90rotate(double the, double theta, double phi);
Vec3 vecrotate(Vec3 v, double theta, double phi);
void vec1to2(double& theta, double& phi, double theta0, double phi0,double a);
std::string formatTime(double seconds);
void drawDashedLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, double dashLength);
std::string PhaseToString(Npgs::AstroObject::Star::Phase phase);