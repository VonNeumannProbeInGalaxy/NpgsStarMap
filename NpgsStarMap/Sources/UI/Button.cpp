#include "Button.h"
#include "Utils/Drawers.h"

namespace UI
{

// Class implementations
// ---------------------
Button::Button()
	: _ButtonColor({ 100, 100, 100 }), _ButtonRect({ 10, 10, 10, 10 }), _BackRect({ 9, 9, 8, 8 }),
	_TextRect({ 9, 9, 8, 8 }), _Text(""), _FontFilename(kTextFontFilename),
	_Texture(nullptr), _Map(nullptr), _bPressState(false), _bToShow(false), _bMouseOnButton(false),
	_bLastPress(false), _bPress(false), _bIsFirstLoad(true) {}

Button::Button(int x, int y, int w, int h, bool bToShow, const SDL_Color& Color, const std::string_view Text, const std::string_view FontFilename,const std::string MapName)
	: _ButtonColor(Color), _ButtonRect({}), _BackRect({}),
	_TextRect({}), _Text(Text), _FontFilename(kTextFontFilename),_MapName(MapName),
	_Texture(nullptr), _Map(nullptr),_bPressState(false), _bToShow(bToShow), _bMouseOnButton(false),
	_bLastPress(false), _bPress(false), _bIsFirstLoad(true) {
	_ButtonRect = { x, y, w, h };
	_BackRect = { int(x + 0.05 * w), int(y + 0.05 * h), int(0.9 * w), int(0.9 * h) };
	_TextRect = { int(x + 0.1 * w), int(y + 0.1 * h), int(0.8 * w), int(0.8 * h) };
}

bool Button::ProcessEvent() {
	if (_bToShow)
		SetButtonState(GetMouseState());
	return _bPressState;
}

bool Button::ProcessEvent(bool bShow) {
	_bToShow = bShow;
	return ProcessEvent();
}

bool Button::ProcessEvent(const int x, int y, int w, int h) {
	_ButtonRect = { x, y, w, h };
	_BackRect = { int(x + 0.05 * w), int(y + 0.05 * h), int(0.9 * w), int(0.9 * h) };
	_TextRect = { int(x + 0.1 * w), int(y + 0.1 * h), int(0.8 * w), int(0.8 * h) };
	return ProcessEvent();
}

bool Button::ProcessEvent(int x, int y, int w, int h, bool bShow) {
	_bToShow = bShow;
	return ProcessEvent(x, y, w, h);
}

void Button::DrawButtonPress(SDL_Renderer* Renderer) {
	if (_bToShow) {
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
		if (_bPressState) {
			
			SDL_SetRenderDrawColor(Renderer, 200, 200, 200, 255);
			if (_bMouseOnButton) {
				SDL_SetRenderDrawColor(Renderer, 180, 180, 180, 200);
			}
		} else {
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
			if (_bMouseOnButton) {
				SDL_SetRenderDrawColor(Renderer, 155, 155, 155, 128);
			}
		}


		SDL_RenderDrawRect(Renderer, &_ButtonRect);
		Utils::DrawThickRectBorder(Renderer, _ButtonRect, 0.01 * _ButtonRect.w);

		TTF_Font* Font = TTF_OpenFont(_FontFilename.c_str(), 500);
		DrawFont(Renderer, Font);
	}
}

void Button::DrawButtonRelease(SDL_Renderer* Renderer) {
	if (_bToShow) {
		PushButton(Renderer);

		SDL_Rect LineRect{ _ButtonRect.x, _ButtonRect.y, _ButtonRect.w, 0.05 * _ButtonRect.h };
		SDL_RenderFillRect(Renderer, &LineRect);

		TTF_Font* Font = TTF_OpenFont(_FontFilename.c_str(), 500);
		DrawFont(Renderer, Font);
	}
}

void Button::DrawButtonRelease_TextLeftAndLineDown(SDL_Renderer* Renderer) {
	if (_bToShow) {
		SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
		if (_bPressState) {
			SDL_SetRenderDrawColor(Renderer, 0, 255, 255, 255);
			if (_bMouseOnButton) {
				SDL_SetRenderDrawColor(Renderer, 0, 200, 200, 255);
			}
		} else {
			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 200);
			if (_bMouseOnButton) {
				SDL_SetRenderDrawColor(Renderer, 200, 200, 200, 150);
			}
		}

		SDL_Rect line_rect = { _ButtonRect.x, _ButtonRect.y + 0.2 * _ButtonRect.h, _ButtonRect.w, 2 };
		SDL_RenderFillRect(Renderer, &line_rect);

		TTF_Font* Font = TTF_OpenFont(_FontFilename.c_str(), 500);
		DrawFont(Renderer, Font);
	}
}
void Button::DrawButtonWithoutRect_grey(SDL_Renderer* Renderer) {
	if (_bToShow) {
		SDL_Color color;
		color = { 200, 200, 200, 255 };
		if (_bPressState) {
			color = { 200, 200, 200, 255 };
			if (_bMouseOnButton) {
				color = { 180, 180, 180, 200 };
			}
		} else {
			color = { 200, 200, 200, 255 };
			if (_bMouseOnButton) {
				color = { 180, 180, 180, 200 };
				if (_bPress) {
					color = { 150, 150, 150, 200 };
				}
			}
		}
		TTF_Font* Font = TTF_OpenFont(_FontFilename.c_str(), 500);
		int w = 0, h = 0;
		TTF_SizeUTF8(Font, _Text.c_str(), &w, &h);
		if (w / static_cast<double>(h) > 1.01 * _TextRect.w / static_cast<double>(_TextRect.h)) {
			_TextRect.h = h / static_cast<double>(w) * _TextRect.w;
		} else if (w / static_cast<double>(h) < 0.99 * _TextRect.w / static_cast<double>(_TextRect.h)) {
			_TextRect.w = w / static_cast<double>(h) * _TextRect.h;
		}

		_TextRect.y = _BackRect.y + 0.5 * _BackRect.h - 0.5 * _TextRect.h;
		_TextRect.x = _ButtonRect.x;
		if (_bIsFirstLoad || (_Texture == nullptr && _Text != "") || (_Map == nullptr && _MapName != "") || (_LastMapName != _MapName)) {
			_LastMapName = _MapName;
			if (_MapName != "") {
				std::string path = "Assets/ButtonMap/" + _MapName;
				SDL_Surface* loadedSurface = IMG_Load(path.c_str());
				if (loadedSurface == nullptr) {
					throw std::runtime_error("Unable to load image " + path + "! SDL_image Error: " + IMG_GetError());
				}

				_Map = SDL_CreateTextureFromSurface(Renderer, loadedSurface);
				if (_Map == nullptr) {
					throw std::runtime_error("Unable to create texture from " + path + "! SDL Error: " + SDL_GetError());
				}

				SDL_FreeSurface(loadedSurface);
			}
			_bIsFirstLoad = false;
			if (_Text != "") {
				SDL_Surface* Surface = TTF_RenderText_Blended(Font, _Text.c_str(), { 255,255,255 });
				_Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
				SDL_FreeSurface(Surface);
			}
		}

		TTF_CloseFont(Font);
		if (_Texture != nullptr) {

			SDL_RenderCopy(Renderer, _Texture, nullptr, &_TextRect);
		}
		if (_Map != nullptr) {
			SDL_SetTextureColorMod(_Map, color.r, color.g, color.b);
			SDL_SetTextureAlphaMod(_Map, color.a);
			SDL_RenderCopy(Renderer, _Map, nullptr, &_ButtonRect);
		}
	}
	

}

void Button::DrawButtonWithoutRect_blue(SDL_Renderer* Renderer) {
	if (_bToShow) {
		SDL_Color color;
		color = { 200, 200, 200, 255 };
		if (_bPressState) {
			color = { 0, 255, 255, 255 };
			if (_bMouseOnButton) {
				color = { 0, 200, 255, 255 };
			}
		} else {
			color = { 200, 200, 200, 255 };
			if (_bMouseOnButton) {
				color = { 180, 180, 180, 200 };
			}
		}
		TTF_Font* Font = TTF_OpenFont(_FontFilename.c_str(), 500);
		int w = 0, h = 0;
		TTF_SizeUTF8(Font, _Text.c_str(), &w, &h);
		if (w / static_cast<double>(h) > 1.01 * _TextRect.w / static_cast<double>(_TextRect.h)) {
			_TextRect.h = h / static_cast<double>(w) * _TextRect.w;
		} else if (w / static_cast<double>(h) < 0.99 * _TextRect.w / static_cast<double>(_TextRect.h)) {
			_TextRect.w = w / static_cast<double>(h) * _TextRect.h;
		}

		_TextRect.y = _BackRect.y + 0.5 * _BackRect.h - 0.5 * _TextRect.h;
		_TextRect.x = _ButtonRect.x;
		if (_bIsFirstLoad || (_Texture == nullptr && _Text != "") || (_Map == nullptr && _MapName != "") || (_LastMapName != _MapName)) {
			_LastMapName = _MapName;
			if (_MapName != "") {
				std::string path = "Assets/ButtonMap/" + _MapName;
				SDL_Surface* loadedSurface = IMG_Load(path.c_str());
				if (loadedSurface == nullptr) {
					throw std::runtime_error("Unable to load image " + path + "! SDL_image Error: " + IMG_GetError());
				}

				_Map = SDL_CreateTextureFromSurface(Renderer, loadedSurface);
				if (_Map == nullptr) {
					throw std::runtime_error("Unable to create texture from " + path + "! SDL Error: " + SDL_GetError());
				}

				SDL_FreeSurface(loadedSurface);
			}
			_bIsFirstLoad = false;
			if (_Text != "") {
				SDL_Surface* Surface = TTF_RenderText_Blended(Font, _Text.c_str(), { 255,255,255 });
				_Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
				SDL_FreeSurface(Surface);
			}
		}

		TTF_CloseFont(Font);
		if (_Texture != nullptr) {

			SDL_RenderCopy(Renderer, _Texture, nullptr, &_TextRect);
		}
		if (_Map != nullptr) {
			SDL_SetTextureColorMod(_Map, color.r, color.g, color.b);
			SDL_SetTextureAlphaMod(_Map, color.a);
			SDL_RenderCopy(Renderer, _Map, nullptr, &_ButtonRect);
		}
	}


}

void Button::SetState(bool bState) {
	_bPressState = bState;
}

Uint32 Button::GetMouseState() {
	int MouseX = 0, MouseY = 0;
	Uint32 MouseButton = SDL_GetMouseState(&MouseX, &MouseY);
	SDL_Point MousePos{ MouseX, MouseY };

	_bMouseOnButton = SDL_PointInRect(&MousePos, &_ButtonRect);

	return MouseButton;
}

void Button::SetButtonState(Uint32 MouseButton) {
	_bPress = false;
	if (_bMouseOnButton && (MouseButton & SDL_BUTTON(SDL_BUTTON_LEFT)))
		_bPress = true;
	if (!_bPress && _bLastPress && _bMouseOnButton)
		_bPressState = !_bPressState;
	_bLastPress = _bPress;
}

void Button::PushButton(SDL_Renderer* Renderer) const {
	SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
	SDL_RenderDrawRect(Renderer, &_ButtonRect);
	if (_bPressState) {
		SDL_SetRenderDrawColor(Renderer, 0, 255, 255, 255);
		if (_bMouseOnButton) {
			SDL_SetRenderDrawColor(Renderer, 0, 200, 255, 255);
		}
	} else {
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 200);
		if (_bMouseOnButton) {
			SDL_SetRenderDrawColor(Renderer, 200, 200, 200, 150);
		}
	}
}

void Button::DrawFont(SDL_Renderer* Renderer, TTF_Font* Font) {
	int w = 0, h = 0;
	TTF_SizeUTF8(Font, _Text.c_str(), &w, &h);
	if (w / static_cast<double>(h) > 1.01 * _TextRect.w / static_cast<double>(_TextRect.h)) {
		_TextRect.h = h / static_cast<double>(w) * _TextRect.w;
	} else if (w / static_cast<double>(h) < 0.99 * _TextRect.w / static_cast<double>(_TextRect.h)) {
		_TextRect.w = w / static_cast<double>(h) * _TextRect.h;
	}

	_TextRect.y = _BackRect.y + 0.5 * _BackRect.h - 0.5 * _TextRect.h;
	_TextRect.x = _ButtonRect.x;
	if (_bIsFirstLoad || (_Texture == nullptr&&_Text != "") || (_Map == nullptr && _MapName != "")||(_LastMapName != _MapName)) {
		_LastMapName = _MapName;
		if (_MapName != "") {
			std::string path = "Assets/ButtonMap/" + _MapName;
			SDL_Surface* loadedSurface = IMG_Load(path.c_str());
			if (loadedSurface == nullptr) {
				throw std::runtime_error("Unable to load image " + path + "! SDL_image Error: " + IMG_GetError());
			}

			_Map =  SDL_CreateTextureFromSurface(Renderer, loadedSurface);
			if (_Map == nullptr) {
				throw std::runtime_error("Unable to create texture from " + path + "! SDL Error: " + SDL_GetError());
			}

			SDL_FreeSurface(loadedSurface);\
		}
		_bIsFirstLoad = false;
		if(_Text != "")
		{
			SDL_Surface* Surface = TTF_RenderText_Blended(Font, _Text.c_str(), { 255,255,255 });
			_Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
			SDL_FreeSurface(Surface);
		}
	}

	TTF_CloseFont(Font);
	if(_Texture!=nullptr)
	{
		SDL_RenderCopy(Renderer, _Texture, nullptr, &_TextRect);
	}
	if (_Map != nullptr) {
		SDL_RenderCopy(Renderer, _Map, nullptr, &_ButtonRect);
	}
}

}