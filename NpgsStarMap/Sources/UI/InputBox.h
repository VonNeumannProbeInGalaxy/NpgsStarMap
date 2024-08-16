#pragma once

#include <string_view>
#include <vector>
#include <SDL2/SDL.h>

#include "Constants.h"

namespace UI {

class InputBox
{
public:
    InputBox(int x, int y, int w, int h, bool bToShow, const std::string_view PromptText,
             const std::string_view FontFilename = kTextFontFilename);

    std::string ProcessEvent();
    std::string ProcessEvent(bool bShow);
    std::string ProcessEvent(int x, int y, int w, int h);
    std::string ProcessEvent(int x, int y, int w, int h, bool bShow);

    void Draw(SDL_Renderer* Renderer);
    void Clear();

public:
    SDL_Texture* _Texture;

    SDL_Rect     _BackRect;
    SDL_Rect     _TextRect;
    std::string  _InputText;
    std::string  _PromptText;
    std::string  _LastText;
    std::string  _FontFilename;

    bool _bPressState;
    bool _bToShow;
    bool _bMouseOnBox;
    bool _bPress;
    bool _bLastPress;
    bool _bIsFirstLoad;

    std::vector<Uint8> _PrevState;
};

}
