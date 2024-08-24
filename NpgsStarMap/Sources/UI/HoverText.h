#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "Constants.h"
#include <SDL2/SDL.h>

namespace UI
{

class HoverText {
public:
	HoverText();

	void Draw(SDL_Renderer* Renderer, const std::string Message,int FontSize,int w, int h,double x,double y,std::string ttfpath=kHoverMessageFontFilename);
	int DrawLeft(SDL_Renderer* Renderer, const std::string Message, int FontSize, int w, int h, double x, double y, std::string ttfpath = kHoverMessageFontFilename);

private:
	SDL_Rect    _BackRect;
	std::string _Text;
	std::string _LastText;

	std::vector<SDL_Texture*> _Textures;
	std::vector<int> _LineWidths;
	std::vector<int> _LineHeights;
};
std::vector<std::string> SplitStrh(const std::string& Str, char Delimiter);
}
