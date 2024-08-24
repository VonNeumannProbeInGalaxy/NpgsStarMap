#include "Drawers.h"

namespace Utils {

void DrawRect(SDL_Renderer* Renderer, const SDL_Rect& Rect)
{
    SDL_RenderDrawRect(Renderer, &Rect);
}

void DrawThickRectBorder(SDL_Renderer* Renderer, const SDL_Rect& Rect, int Thickness)
{
    DrawRect(Renderer, Rect);
    if (Thickness < 1) { Thickness = 1; }
    for (int i = 1; i != Thickness; ++i) {
        DrawRect(Renderer, { Rect.x + i, Rect.y + i, Rect.w - 2 * i, Rect.h - 2 * i });
    }
}

}
