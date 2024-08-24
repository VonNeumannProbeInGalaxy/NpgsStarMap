#include "HoverText.h"

#include <sstream>
#include <SDL2/SDL_ttf.h>

#include "Constants.h"
#include "Utils/Drawers.h"

namespace UI
{


HoverText::HoverText() : _BackRect({}), _Text({}), _LastText({}), _Textures({}), _LineWidths({}), _LineHeights({}) {}



void HoverText::Draw(SDL_Renderer* Renderer, const std::string Message,int FontSize, int w, int h, double x, double y,std::string ttfpath) {
	_BackRect = { int(w*x),int(h*y),0,0 };
	_Text = Message;
	if (!_Text.empty()) {
		TTF_Font* Font = TTF_OpenFont(ttfpath.c_str(), FontSize);

	//SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_MUL);
	//SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 150);
	//SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
	//SDL_RenderFillRect(Renderer, &_BackRect);
	//SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
	//Utils::DrawThickRectBorder(Renderer, { _BackRect.x, _BackRect.y, _BackRect.w, _BackRect.h }, 0.005 * _BackRect.w);
		int MaxWidth = 0;
		std::vector<std::string> InfoLines = SplitStrh(_Text, '\n');
		int OffsetY = _BackRect.y + 2;
		for (int i = 0; i < InfoLines.size(); i++) {
			const auto& Line = InfoLines[i];
			int LineWidth = 0, LineHeight = 0;
			TTF_SizeUTF8(Font, Line.c_str(), &LineWidth, &LineHeight);
			if (LineWidth > MaxWidth) {
				MaxWidth = LineWidth;
			}
			if (_LastText != _Text) {

				SDL_Surface* Surface = TTF_RenderUTF8_Blended(Font, Line.c_str(), {255, 255, 255, 255});
				_Textures.resize(InfoLines.size());
				_LineWidths.resize(InfoLines.size());
				_LineHeights.resize(InfoLines.size());

				SDL_DestroyTexture(_Textures[i]);
				_Textures[i] = SDL_CreateTextureFromSurface(Renderer, Surface);

				_LineWidths[i] = Surface->w;
				_LineHeights[i] = Surface->h;
				SDL_FreeSurface(Surface);
			}
		}
		for (int i = 0; i < InfoLines.size(); i++) {
			SDL_Rect TextRect = { _BackRect.x + 2 - 0.5 * MaxWidth, OffsetY, _LineWidths[i], _LineHeights[i] };
			SDL_RenderCopy(Renderer, _Textures[i], nullptr, &TextRect);
			OffsetY += _LineHeights[i];
		}

		TTF_CloseFont(Font);
	}

	_LastText = _Text;
	_Text = "";
}
int HoverText::DrawLeft(SDL_Renderer* Renderer, const std::string Message, int FontSize, int w, int h, double x, double y, std::string ttfpath) {
	_BackRect = { int(w * x),int(h * y),0,0 };
	_Text = Message;
	int OffsetY = _BackRect.y + 2;
	if (!_Text.empty()) {
		TTF_Font* Font = TTF_OpenFont(ttfpath.c_str(), FontSize);

		//SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_MUL);
		//SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 150);
		//SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
		//SDL_RenderFillRect(Renderer, &_BackRect);
		//SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
		//Utils::DrawThickRectBorder(Renderer, { _BackRect.x, _BackRect.y, _BackRect.w, _BackRect.h }, 0.005 * _BackRect.w);
		int MaxWidth = 0;
		std::vector<std::string> InfoLines = SplitStrh(_Text, '\n');
		
		for (int i = 0; i < InfoLines.size(); i++) {
			const auto& Line = InfoLines[i];
			int LineWidth = 0, LineHeight = 0;
			TTF_SizeUTF8(Font, Line.c_str(), &LineWidth, &LineHeight);
			if (LineWidth > MaxWidth) {
				MaxWidth = LineWidth;
			}
			if (_LastText != _Text) {

				SDL_Surface* Surface = TTF_RenderUTF8_Blended(Font, Line.c_str(), { 255, 255, 255, 255 });
				_Textures.resize(InfoLines.size());
				_LineWidths.resize(InfoLines.size());
				_LineHeights.resize(InfoLines.size());

				SDL_DestroyTexture(_Textures[i]);
				_Textures[i] = SDL_CreateTextureFromSurface(Renderer, Surface);

				_LineWidths[i] = Surface->w;
				_LineHeights[i] = Surface->h;
				SDL_FreeSurface(Surface);
			}
		}
		for (int i = 0; i < InfoLines.size(); i++) {
			SDL_Rect TextRect = { _BackRect.x + 2, OffsetY, _LineWidths[i], _LineHeights[i] };
			SDL_RenderCopy(Renderer, _Textures[i], nullptr, &TextRect);
			OffsetY += _LineHeights[i];
		}

		TTF_CloseFont(Font);

	}

	_LastText = _Text;
	_Text = "";
	return OffsetY;

}

// Tool functions
// --------------
std::vector<std::string> SplitStrh(const std::string& Str, char Delimiter) {
	std::vector<std::string> Result;
	std::string StrView(Str);
	std::size_t Start = 0;
	std::size_t End = 0;

	while ((End = StrView.find(Delimiter, Start)) != std::string::npos) {
		Result.emplace_back(StrView.substr(Start, End - Start));
		Start = End + 1;
	}

	Result.emplace_back(StrView.substr(Start));
	return Result;
}

}
