#pragma once

#include <string>
#include <string_view>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Constants.h"

namespace UI
{

class Button {
public:
	Button();
	Button(int x, int y, int w, int h, bool bToShow, const SDL_Color& Color,
		const std::string_view Text, const std::string_view FontFilename = kTextFontFilename);

	bool ProcessEvent();
	bool ProcessEvent(bool bShow);
	bool ProcessEvent(int x, int y, int w, int h);
	bool ProcessEvent(int x, int y, int w, int h, bool bShow);

	void DrawButtonPress(SDL_Renderer* Renderer);
	void DrawButtonRelease(SDL_Renderer* Renderer);
	void DrawButtonRelease_TextLeftAndLineDown(SDL_Renderer* Renderer);

	void SetState(bool bState);

private:
	Uint32 GetMouseState();
	void SetButtonState(Uint32 MouseButton);
	void PushButton(SDL_Renderer* Renderer) const;
	void DrawFont(SDL_Renderer* Renderer, TTF_Font* Font);

public:
	SDL_Texture* _Texture;

	SDL_Color    _ButtonColor;
	SDL_Rect     _ButtonRect;
	SDL_Rect     _BackRect;
	SDL_Rect     _TextRect;
	std::string  _Text;
	std::string  _FontFilename;

	bool _bPressState;
	bool _bToShow;
	bool _bMouseOnButton;
	bool _bPress;
	bool _bLastPress;
	bool _bIsFirstLoad;
};

}
