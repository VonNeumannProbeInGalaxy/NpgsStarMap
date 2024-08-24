#include "HoverMessage.h"

#include <sstream>
#include <SDL2/SDL_ttf.h>

#include "Constants.h"
#include "Utils/Drawers.h"

namespace UI
{


HoverMessage::HoverMessage() : _BackRect({}), _Text({}), _LastText({}), _Textures({}), _LineWidths({}), _LineHeights({}) {}

void HoverMessage::ProcessEvent(const std::string Message, int FontSize, int w, int h) {
	_Text = Message;
	if (!_Text.empty()) {
		int MouseX = 0, MouseY = 0;
		SDL_GetMouseState(&MouseX, &MouseY);

		TTF_Font* Font = TTF_OpenFont(kHoverMessageFontFilename.c_str(), FontSize);
		std::istringstream Stream(_Text);
		std::string TextLine;

		int MaxWidth = 0;
		int TotalHeight = 0;
		int LineNumber = 0;

		// 计算每行的宽度和总高度
		while (std::getline(Stream, TextLine)) {
			int LineWidth = 0, LineHeight = 0;
			TTF_SizeUTF8(Font, TextLine.c_str(), &LineWidth, &LineHeight);
			if (LineWidth > MaxWidth)
				MaxWidth = LineWidth;
			TotalHeight += LineHeight;
			++LineNumber;
		}

		TTF_CloseFont(Font);

		if (((MouseX + MaxWidth) <= w) && ((MouseY - TotalHeight) >= 0))
			_BackRect = { MouseX, MouseY - TotalHeight - 4 , MaxWidth + 4, TotalHeight + 4 };
		else
			_BackRect = { MouseX - MaxWidth - 4, MouseY, MaxWidth + 4, TotalHeight + 4 };
		//std::cout<< _BackRect.x<<" " << _BackRect.y << " " << _BackRect.w << " " << _BackRect.h << std::endl;
	}
}

void HoverMessage::Draw(SDL_Renderer* Renderer, int FontSize) {
	if (!_Text.empty()) {
		TTF_Font* Font = TTF_OpenFont(kHoverMessageFontFilename.c_str(), FontSize);

		SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_MUL);
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 150);
		SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderFillRect(Renderer, &_BackRect);
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
		Utils::DrawThickRectBorder(Renderer, { _BackRect.x, _BackRect.y, _BackRect.w, _BackRect.h }, 0.005 * _BackRect.w);

		std::vector<std::string> InfoLines = SplitStr(_Text, '\n');
		int OffsetY = _BackRect.y + 2;
		for (int i = 0; i < InfoLines.size(); i++) {
			const auto& Line = InfoLines[i];
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

			SDL_Rect TextRect = { _BackRect.x + 2, OffsetY, _LineWidths[i], _LineHeights[i] };
			SDL_RenderCopy(Renderer, _Textures[i], nullptr, &TextRect);
			OffsetY += _LineHeights[i];
		}

		TTF_CloseFont(Font);
	}

	_LastText = _Text;
	_Text = "";
}

// Tool functions
// --------------
std::vector<std::string> SplitStr(const std::string& Str, char Delimiter) {
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
