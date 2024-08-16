#pragma once
#include <SDL2/SDL.h>

SDL_Color ScaleSDLColor(double x, const SDL_Color& color);
SDL_Color kelvin_to_rgb(double temperature);
void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color);
double variable_threshold001(double variable);
double variable_threshold00(double variable);
double variable_threshold1(double variable);
std::string random_name();
bool isMouseNearLine(SDL_Point p1, SDL_Point p2);
