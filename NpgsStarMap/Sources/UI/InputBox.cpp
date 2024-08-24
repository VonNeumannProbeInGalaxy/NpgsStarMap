#include "InputBox.h"

#include <SDL2/SDL_ttf.h>
#include "Utils/Drawers.h"

namespace UI
{

InputBox::InputBox(int x, int y, int w, int h, bool bToShow, const std::string_view PromptText, const std::string_view FontFilename)
	: _BackRect({}), _TextRect({}), _PromptText(PromptText), _FontFilename(kTextFontFilename), _Texture(nullptr),
	_bPressState(false), _bToShow(bToShow), _bMouseOnBox(false), _bLastPress(false), _bPress(false), _bIsFirstLoad(false),
	_PrevState(static_cast<int>(SDL_NUM_SCANCODES), 0) {}

std::string InputBox::ProcessEvent() {
	if (_bToShow) {
		int MouseX = 0, MouseY = 0;
		Uint32 MouseState = SDL_GetMouseState(&MouseX, &MouseY);
		SDL_Point MousePos{ MouseX, MouseY };
		_bMouseOnBox = SDL_PointInRect(&MousePos, &_BackRect);

		_bPress = false;
		if (_bMouseOnBox && (MouseState & SDL_BUTTON(SDL_BUTTON_LEFT)))
			_bPress = true;
		if (_bPress && _bLastPress && _bMouseOnBox)
			_bPressState = true;
		if (!_bMouseOnBox && (MouseState & SDL_BUTTON(SDL_BUTTON_LEFT)))
			_bPressState = false;
		_bLastPress = _bPress;

		const Uint8* KeyboardStatePtr = SDL_GetKeyboardState(nullptr);
		std::vector<Uint8> CurrentState(KeyboardStatePtr, KeyboardStatePtr + static_cast<int>(SDL_NUM_SCANCODES));

		for (int i = 0; i != static_cast<int>(SDL_NUM_SCANCODES); ++i) {
			if (CurrentState[i] && !_PrevState[i]) {
				if (_bPressState) {
					if ((i >= 30 && i <= 39) || (i >= 4 && i <= 29)) {
						_InputText += SDL_GetScancodeName(static_cast<SDL_Scancode>(i));
					}
					if (i == 42) {
						if (!_InputText.empty() && _bPressState) {
							_InputText.pop_back();
						}
					}
					if (i == 40) {
						if (_bPressState) {
							_bPressState = false;
						}
					}
				}
			}
		}

		_PrevState = CurrentState;
	}

	return _InputText;

}

std::string InputBox::ProcessEvent(bool bShow) {
	_bToShow = bShow;
	return ProcessEvent();
}

std::string InputBox::ProcessEvent(int x, int y, int w, int h) {
	_BackRect = { x, y, w, h };
	_TextRect = { int(x + 0.1 * w), int(y + 0.1 * h), int(0.8 * w), int(0.8 * h) };
	return ProcessEvent();
}

std::string InputBox::ProcessEvent(int x, int y, int w, int h, bool bShow) {
	_BackRect = { x, y, w, h };
	_TextRect = { int(x + 0.1 * w), int(y + 0.1 * h), int(0.8 * w), int(0.8 * h) };
	return ProcessEvent(bShow);
}

void InputBox::Draw(SDL_Renderer* Renderer) {
	if (_bToShow) {
		SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
		SDL_RenderDrawRect(Renderer, &_BackRect);
		if (_bPressState) {
			SDL_SetRenderDrawColor(Renderer, 0, 255, 255, 255);
		} else {
			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 200);
			if (_bMouseOnBox) {
				SDL_SetRenderDrawColor(Renderer, 200, 200, 200, 150);
			}
		}

		SDL_RenderDrawRect(Renderer, &_BackRect);
		Utils::DrawThickRectBorder(Renderer, { _BackRect.x, _BackRect.y, _BackRect.w, _BackRect.h }, 1);

		int w = 0, h = 0;
		TTF_Font* Font = TTF_OpenFont(_FontFilename.c_str(), 300);
		std::string Text = _PromptText + _InputText;
		TTF_SizeUTF8(Font, Text.c_str(), &w, &h);
		if (w / double(h) > 1.01 * _TextRect.w / double(_TextRect.h)) {
			_TextRect.h = h / double(w) * _TextRect.w;
		} else if (w / double(h) < 0.99 * _TextRect.w / double(_TextRect.h)) {
			_TextRect.w = w / double(h) * _TextRect.h;
		}

		_TextRect.y = _BackRect.y + 0.5 * _BackRect.h - 0.5 * _TextRect.h;

		if (_InputText != _LastText) {
			SDL_Surface* Surface = TTF_RenderUTF8_Blended(Font, Text.c_str(), { 255, 255, 255 });

			SDL_DestroyTexture(_Texture);
			_Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
			SDL_FreeSurface(Surface);
		}

		_LastText = _InputText;

		TTF_CloseFont(Font);
		SDL_RenderCopy(Renderer, _Texture, nullptr, &_TextRect);
	}
}

void InputBox::Clear() {
	_InputText.clear();
}

}
