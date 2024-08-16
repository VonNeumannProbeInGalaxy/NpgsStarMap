#pragma once

#include <SDL2/SDL.h>

namespace Utils {

void DrawRect(SDL_Renderer* Renderer, const SDL_Rect& Rect);
void DrawThickRectBorder(SDL_Renderer* Renderer, const SDL_Rect& Rect, int Thickness);

}
