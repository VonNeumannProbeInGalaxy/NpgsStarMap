#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <SDL2/SDL.h>

namespace UI
{

class HoverMessage {
public:
	HoverMessage();

	void ProcessEvent(const std::string Message, int FontSize, int w, int h);
	void Draw(SDL_Renderer* Renderer, int FontSize);

private:
	SDL_Rect    _BackRect;
	std::string _Text;
	std::string _LastText;

	std::vector<SDL_Texture*> _Textures;
	std::vector<int> _LineWidths;
	std::vector<int> _LineHeights;
};
std::vector<std::string> SplitStr(const std::string& Str, char Delimiter);
}
