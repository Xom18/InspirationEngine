#include "InspirationEngine.h"

void TextBox::transToTexture()
{
	if (m_renderer == nullptr)
		return;
	if (m_font == nullptr)
		return;

	FontFace* pFontFace = m_font->get();
	if (pFontFace == nullptr || pFontFace->ftFace == nullptr)
		return;

	m_textChanged = false;
	size_t szHash = std::hash<std::string>{}(m_text);
	if (m_drawHash == szHash)
		return;

	m_drawHash = szHash;
	resetTexture();

	std::stack<SDL_Color> stkColor;
	stkColor.push(m_color);

	// 스타일 스택: int32_t (IE_FONT_STYLE_*)
	std::stack<int32_t> stkStyle;
	stkStyle.push(IE_FONT_STYLE_NORMAL);

	size_t szTextLength = m_text.length();
	if (szTextLength == 0)
		return;

	FT_Face ftFace = pFontFace->ftFace;
	int32_t iAscent = m_font->getAscent();

	size_t szBegPoint = 0;
	int32_t iXOffset = 0;
	int32_t iYOffset = 0;

	while (szTextLength > szBegPoint)
	{
		size_t szEndPoint = szTextLength;
		bool bIsEnterLine = false;
		size_t iNextOffset = 0;

		// 마크업 파싱 (기존 로직 동일)
		{
			size_t szCodeBegPoint = m_text.find("<s:", szBegPoint);
			if (szCodeBegPoint == szBegPoint)
			{
				size_t szCodeEndPoint = m_text.find(">", szBegPoint);
				if (szCodeEndPoint != std::string::npos)
				{
					++szCodeEndPoint;
					std::string strStyleCode;
					strStyleCode.append(m_text, szCodeBegPoint, szCodeEndPoint - szCodeBegPoint);
					int32_t iStyle = IE_FONT_STYLE_NORMAL;
					if (operateStyleCode(&strStyleCode, &iStyle))
					{
						stkStyle.push(iStyle);
						szBegPoint = szCodeEndPoint;
						continue;
					}
				}
				szCodeBegPoint = std::string::npos;
			}
			if (szCodeBegPoint < szEndPoint) szEndPoint = szCodeBegPoint;
		}
		{
			const char* c_csCloseCode = "</s>";
			size_t szClosePoint = m_text.find(c_csCloseCode, szBegPoint);
			if (szClosePoint == szBegPoint)
			{
				if (stkStyle.size() > 1) stkStyle.pop();
				szBegPoint += std::strlen(c_csCloseCode);
				continue;
			}
			if (szClosePoint < szEndPoint) szEndPoint = szClosePoint;
		}
		{
			size_t szCodeBegPoint = m_text.find("<c:", szBegPoint);
			if (szCodeBegPoint == szBegPoint)
			{
				size_t szCodeEndPoint = m_text.find(">", szBegPoint);
				if (szCodeEndPoint != std::string::npos)
				{
					++szCodeEndPoint;
					std::string strStyleCode;
					strStyleCode.append(m_text, szCodeBegPoint, szCodeEndPoint - szCodeBegPoint);
					SDL_Color FontColor;
					if (operateColorCode(&strStyleCode, &FontColor))
					{
						stkColor.push(FontColor);
						szBegPoint = szCodeEndPoint;
						continue;
					}
				}
				szCodeBegPoint = std::string::npos;
			}
			if (szCodeBegPoint < szEndPoint) szEndPoint = szCodeBegPoint;
		}
		{
			const char* c_csCloseCode = "</c>";
			size_t szClosePoint = m_text.find(c_csCloseCode, szBegPoint);
			if (szClosePoint == szBegPoint)
			{
				if (stkColor.size() > 1) stkColor.pop();
				szBegPoint += std::strlen(c_csCloseCode);
				continue;
			}
			if (szClosePoint < szEndPoint) szEndPoint = szClosePoint;
		}
		{
			size_t szEnterPoint = m_text.find("\n", szBegPoint);
			if (szEnterPoint != std::string::npos && szEnterPoint < szEndPoint)
			{
				bIsEnterLine = true;
				++iNextOffset;
				szEndPoint = szEnterPoint;
			}
		}

		std::string strTargetText;
		strTargetText.append(m_text, szBegPoint, szEndPoint - szBegPoint);

		// 현재 스타일의 폰트 페이스 가져오기
		FontFace* pCurFace = m_font->get(stkStyle.top());
		if (pCurFace == nullptr) pCurFace = pFontFace;
		FT_Face curFTFace = pCurFace->ftFace;
		hb_font_t* curHBFont = pCurFace->hbFont;

		// 너비 제한 처리
		if (m_rect.w != 0 && m_rect.h != 0 && (m_textBoxStyle & dTEXT_BOX_AUTO_NEXTLINE))
		{
			auto shaped = IETextRenderer::shape(curHBFont, strTargetText.c_str(), static_cast<int32_t>(strTargetText.size()));
			auto m = IETextRenderer::measure(curFTFace, shaped);
			int32_t iTextWidth = m.width;

			if (iXOffset + iTextWidth > m_rect.w)
			{
				bool bIsSpaceCut = true;
				bool bIsCorrectSpace = false;
				std::string strResultText;

				while (strTargetText.length())
				{
					if (bIsSpaceCut)
					{
						size_t szSpaceBeginPos = strTargetText.find_first_of(" ");
						if (szSpaceBeginPos == std::string::npos)
						{
							if ((m_textBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE) && bIsCorrectSpace)
							{
								size_t removeSpaceBegin = strResultText.find_last_not_of(' ');
								size_t removeSpaceEnd   = strResultText.find_last_of(' ');
								if (removeSpaceBegin != std::string::npos && removeSpaceEnd != std::string::npos)
								{
									strResultText.erase(removeSpaceBegin + 1, removeSpaceEnd);
									iNextOffset += removeSpaceEnd - removeSpaceBegin;
								}
								break;
							}
							bIsSpaceCut = false;
							continue;
						}
						size_t szSpaceEndPos = strTargetText.find_first_not_of(" ", szSpaceBeginPos + 1);

						size_t szResultLength = strResultText.length();
						if (szSpaceEndPos == std::string::npos)
							strResultText.append(strTargetText, 0, szSpaceBeginPos + 1);
						else
							strResultText.append(strTargetText, 0, szSpaceEndPos);

						auto ts = IETextRenderer::shape(curHBFont, strResultText.c_str(), static_cast<int32_t>(strResultText.size()));
						int32_t iTempWidth = IETextRenderer::measure(curFTFace, ts).width;

						if (m_textBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE)
						{
							if (m_rect.w < iXOffset + iTempWidth)
							{
								size_t spaceBeginPoint = strResultText.find_last_not_of(' ');
								if (spaceBeginPoint != std::string::npos)
								{
									std::string tempString;
									tempString.append(strResultText, 0, spaceBeginPoint + 1);
									auto ts2 = IETextRenderer::shape(curHBFont, tempString.c_str(), static_cast<int32_t>(tempString.size()));
									int32_t spaceRemoveWidth = IETextRenderer::measure(curFTFace, ts2).width;
									if (iXOffset + spaceRemoveWidth <= m_rect.w)
									{
										iNextOffset += strResultText.length() - spaceBeginPoint - 1;
										strResultText = tempString;
										break;
									}
								}
							}
						}

						if (m_rect.w < iXOffset + iTempWidth)
						{
							strResultText.erase(szResultLength);
							if ((m_textBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE) && bIsCorrectSpace)
								break;
							bIsSpaceCut = false;
							continue;
						}

						if (szSpaceEndPos == std::string::npos)
							strTargetText.erase(0, szSpaceBeginPos + 1);
						else
							strTargetText.erase(0, szSpaceEndPos);
						bIsCorrectSpace = true;
					}
					else
					{
						size_t szSecondPos = StrUTF8::getMemoryPoint(strTargetText, 1);
						size_t szResultLength = strResultText.length();
						strResultText.append(strTargetText, 0, szSecondPos);

						auto ts = IETextRenderer::shape(curHBFont, strResultText.c_str(), static_cast<int32_t>(strResultText.size()));
						int32_t iTempWidth = IETextRenderer::measure(curFTFace, ts).width;

						if (iXOffset + iTempWidth > m_rect.w)
						{
							strResultText.erase(szResultLength);
							break;
						}
						strTargetText.erase(0, szSecondPos);
					}
				}
				strTargetText = strResultText;

				if (strTargetText.length() == 0)
					break;
				if (bIsSpaceCut == false)
					iNextOffset = 0;

				szEndPoint = szBegPoint + strTargetText.length();
				bIsEnterLine = true;
			}
		}

		int32_t iWidthOffset = 0;

		if (strTargetText.length())
		{
			auto shaped = IETextRenderer::shape(curHBFont, strTargetText.c_str(), static_cast<int32_t>(strTargetText.size()));
			auto ms     = IETextRenderer::measure(curFTFace, shaped);

			SDL_Texture* pTex = IETextRenderer::renderToTexture(
				m_renderer->m_renderer,
				curFTFace,
				shaped,
				stkColor.top(),
				ms.width  > 0 ? ms.width  : 1,
				ms.height > 0 ? ms.height : 1,
				iAscent,
				pCurFace->bBold);

			if (pTex)
			{
				auto pTTexture = std::make_unique<TextTexture>();
				pTTexture->m_texture = pTex;
				pTTexture->m_bufferPos = static_cast<int32_t>(szBegPoint);
				pTTexture->m_rect.x = iXOffset;
				pTTexture->m_rect.y = iYOffset;
				pTTexture->m_rect.w = ms.width;
				pTTexture->m_rect.h = ms.height;

				m_textures.push_back(std::move(pTTexture));
				iWidthOffset = ms.width;
			}
		}

		// 커서 픽셀 위치 계산
		size_t cursurPos = m_cursorPos - m_imeInputLength;
		if (szBegPoint <= cursurPos && cursurPos < szEndPoint + iNextOffset)
		{
			std::string prefix = strTargetText.substr(0, cursurPos - szBegPoint);
			auto ps = IETextRenderer::shape(curHBFont, prefix.c_str(), static_cast<int32_t>(prefix.size()));
			int32_t iTextWidth  = IETextRenderer::measure(curFTFace, ps).width;
			int32_t iTextHeight = m_fontHeight;

			m_cursorScreenPos.x = m_rect.x + iXOffset + iTextWidth;
			m_cursorScreenPos.y = m_rect.y + iYOffset;

			SDL_Rect rect{
				.x = m_rect.x + m_cursorScreenPos.x,
				.y = m_rect.y + m_cursorScreenPos.y,
				.w = 0,
				.h = iTextHeight,
			};
			IECore::updateTextEditPosition(rect);
		}

		if (szTextLength <= szEndPoint + iNextOffset || szEndPoint == std::string::npos)
			break;

		szBegPoint = szEndPoint + iNextOffset;

		if (bIsEnterLine)
		{
			iYOffset += m_fontHeight;
			iXOffset = 0;
		}
		else
		{
			iXOffset += iWidthOffset;
		}
	}
}

// ──────────────────────────────────────────────
//  공통 (양쪽 경로 동일)
// ──────────────────────────────────────────────

void TextBox::draw()
{
	if (m_renderer == nullptr)
		return;

	if (m_textChanged)
		transToTexture();

	for (const auto& pTTexture : m_textures)
		m_renderer->drawTexture(pTTexture->m_texture, m_rect.x + pTTexture->m_rect.x, m_rect.y + pTTexture->m_rect.y);
}

void TextBox::update()
{
}

bool TextBox::operateStyleCode(const std::string* strText, int32_t* outResult)
{
	size_t szCursor = strText->find_first_not_of("<s:");

	if (szCursor == std::string::npos || strText->find_first_of("<s:") == std::string::npos)
		return false;

	if (strText->find_last_of(">") == std::string::npos)
		return false;

	int32_t iResult = IE_FONT_STYLE_NORMAL;
	if (strText->find("BOLD",          szCursor) != std::string::npos) iResult |= IE_FONT_STYLE_BOLD;
	if (strText->find("ITALIC",        szCursor) != std::string::npos) iResult |= IE_FONT_STYLE_ITALIC;
	if (strText->find("UNDERLINE",     szCursor) != std::string::npos) iResult |= IE_FONT_STYLE_UNDERLINE;
	if (strText->find("STRIKETHROUGH", szCursor) != std::string::npos) iResult |= IE_FONT_STYLE_STRIKETHROUGH;

	*outResult = iResult;
	return true;
}

bool TextBox::operateColorCode(const std::string* strText, SDL_Color* outResult)
{
	size_t szEndPoint = strText->find_last_of(">");
	if (szEndPoint == std::string::npos)
		return false;

	if (strText->find_first_of("<c:0x") != std::string::npos)
	{
		size_t szPos = strText->find_first_not_of("<c:0x");
		std::string strTempString;
		strTempString.append(*strText, szPos, szEndPoint - szPos);

		std::stringstream StrStream;
		Uint32 uiHexValue = 0;
		StrStream << std::hex << strTempString;
		StrStream >> uiHexValue;

		outResult->b = uiHexValue & 0xff; uiHexValue >>= 8;
		outResult->g = uiHexValue & 0xff; uiHexValue >>= 8;
		outResult->r = uiHexValue & 0xff; uiHexValue >>= 8;
		outResult->a = uiHexValue & 0xff;

		return true;
	}
	return false;
}
