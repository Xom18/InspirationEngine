#include "InspirationEngine.h"

#ifdef IE_LEGACY_TTF
// ──────────────────────────────────────────────
//  레거시 경로 (SDL2_ttf)
// ──────────────────────────────────────────────

void cTextBox::transToTexture()
{
	if (!m_lpRenderer)
		return;

	if (m_lpFont == nullptr)
		return;
	if (m_lpFont->get() == nullptr)
		return;

	m_bTextChanged = false;
	size_t szHash = std::hash<std::string>{}(m_strText);
	if (m_szDrawHash == szHash)
		return;

	m_szDrawHash = szHash;
	resetTexture();

	std::stack<SDL_Color> stkColor;
	stkColor.push(m_Color);

	std::stack<TTF_Font*> stkFont;
	stkFont.push(m_lpFont->get());

	size_t szTextLength = m_strText.length();

	if (szTextLength == 0)
		return;

	size_t szBegPoint = 0;

	int iXOffset = 0;
	int iYOffset = 0;
	while (szTextLength > szBegPoint)
	{
		size_t szEndPoint = szTextLength;
		bool bIsEnterLine = false;
		size_t iNextOffset = 0;

		{
			size_t szCodeBegPoint = m_strText.find("<s:", szBegPoint);
			if (szCodeBegPoint == szBegPoint)
			{
				size_t szCodeEndPoint = m_strText.find(">", szBegPoint);
				if (szCodeEndPoint != std::string::npos)
				{
					++szCodeEndPoint;
					std::string strStyleCode;
					strStyleCode.append(m_strText, szCodeBegPoint, szCodeEndPoint - szCodeBegPoint);
					int iStyle = TTF_STYLE_NORMAL;
					if (operateStyleCode(&strStyleCode, &iStyle))
					{
						stkFont.push(m_lpFont->get(iStyle));
						szBegPoint = szCodeEndPoint;
						continue;
					}
				}
				szCodeBegPoint = std::string::npos;
			}
			if (szCodeBegPoint < szEndPoint)
				szEndPoint = szCodeBegPoint;
		}

		{
			const char* c_csCloseCode = "</s>";
			size_t szClosePoint = m_strText.find(c_csCloseCode, szBegPoint);
			if (szClosePoint == szBegPoint)
			{
				if (stkFont.size() > 1)
					stkFont.pop();
				szBegPoint += std::strlen(c_csCloseCode);
				continue;
			}
			if (szClosePoint < szEndPoint)
				szEndPoint = szClosePoint;
		}

		{
			size_t szCodeBegPoint = m_strText.find("<c:", szBegPoint);
			if (szCodeBegPoint == szBegPoint)
			{
				size_t szCodeEndPoint = m_strText.find(">", szBegPoint);
				if (szCodeEndPoint != std::string::npos)
				{
					++szCodeEndPoint;
					std::string strStyleCode;
					strStyleCode.append(m_strText, szCodeBegPoint, szCodeEndPoint - szCodeBegPoint);
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
			if (szCodeBegPoint < szEndPoint)
				szEndPoint = szCodeBegPoint;
		}

		{
			const char* c_csCloseCode = "</c>";
			size_t szClosePoint = m_strText.find(c_csCloseCode, szBegPoint);
			if (szClosePoint == szBegPoint)
			{
				if (stkColor.size() > 1)
					stkColor.pop();
				szBegPoint += std::strlen(c_csCloseCode);
				continue;
			}
			if (szClosePoint < szEndPoint)
				szEndPoint = szClosePoint;
		}

		{
			size_t szEnterPoint = m_strText.find("\n", szBegPoint);
			if (szEnterPoint != std::string::npos && szEnterPoint < szEndPoint)
			{
				bIsEnterLine = true;
				++iNextOffset;
				szEndPoint = szEnterPoint;
			}
		}

		std::string strTargetText;
		strTargetText.append(m_strText, szBegPoint, szEndPoint - szBegPoint);

		if (m_rtRect.w != 0 && m_rtRect.h != 0 && (m_iTextBoxStyle & dTEXT_BOX_AUTO_NEXTLINE))
		{
			int iTextWidth = 0;
			int iTextHeight = 0;
			TTF_SizeUTF8(stkFont.top(), strTargetText.c_str(), &iTextWidth, &iTextHeight);

			if (iXOffset + iTextWidth > m_rtRect.w)
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
							if ((m_iTextBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE) && bIsCorrectSpace)
							{
								size_t removeSpaceBegin = strResultText.find_last_not_of(' ');
								size_t removeSpaceEnd = strResultText.find_last_of(' ');
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

						int iTempWidth = 0;
						int iTempHeight = 0;
						TTF_SizeUTF8(stkFont.top(), strResultText.c_str(), &iTempWidth, &iTempHeight);

						if (m_iTextBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE)
						{
							if (m_rtRect.w < iXOffset + iTempWidth)
							{
								size_t spaceBeginPoint = strResultText.find_last_not_of(' ');
								if (spaceBeginPoint != std::string::npos)
								{
									std::string tempString;
									tempString.append(strResultText, 0, spaceBeginPoint + 1);

									int spaceRemoveWidth = 0;
									int spaceRemoveHeight = 0;
									TTF_SizeUTF8(stkFont.top(), tempString.c_str(), &spaceRemoveWidth, &spaceRemoveHeight);
									if (iXOffset + spaceRemoveWidth <= m_rtRect.w)
									{
										iNextOffset += strResultText.length() - spaceBeginPoint - 1;
										strResultText = tempString;
										break;
									}
								}
							}
						}

						if (m_rtRect.w < iXOffset + iTempWidth)
						{
							strResultText.erase(szResultLength);
							if ((m_iTextBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE) && bIsCorrectSpace)
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
						size_t szSecondPos = cStrUTF8::getMemoryPoint(strTargetText, 1);
						size_t szResultLength = strResultText.length();
						strResultText.append(strTargetText, 0, szSecondPos);

						int iTempWidth = 0;
						int iTempHeight = 0;
						TTF_SizeUTF8(stkFont.top(), strResultText.c_str(), &iTempWidth, &iTempHeight);

						if (iXOffset + iTempWidth > m_rtRect.w)
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

		int iWidthOffset = 0;

		if (strTargetText.length())
		{
			SDL_Surface* pSurface = TTF_RenderUTF8_Solid(stkFont.top(), strTargetText.c_str(), stkColor.top());

			cTextTexture* pTTexture = new cTextTexture();
			pTTexture->m_pTexture = SDL_CreateTextureFromSurface(m_lpRenderer->m_pRenderer, pSurface);
			SDL_FreeSurface(pSurface);

			pTTexture->m_iBufferPos = static_cast<int>(szBegPoint);
			pTTexture->m_rtRect.x = iXOffset;
			pTTexture->m_rtRect.y = iYOffset;
			SDL_QueryTexture(pTTexture->m_pTexture, NULL, NULL, &pTTexture->m_rtRect.w, &pTTexture->m_rtRect.h);

			m_listTexture.push_back(pTTexture);

			iWidthOffset = pTTexture->m_rtRect.w;
		}

		size_t cursurPos = m_szCursorPos - m_szIMEInputLength;
		if (szBegPoint <= cursurPos && cursurPos < szEndPoint + iNextOffset)
		{
			int iTextWidth = 0;
			int iTextHeight = 0;

			strTargetText.erase(cursurPos - szBegPoint);
			TTF_SizeUTF8(stkFont.top(), strTargetText.c_str(), &iTextWidth, &iTextHeight);

			m_CursorScreenPos.x = m_rtRect.x + iXOffset + iTextWidth;
			m_CursorScreenPos.y = m_rtRect.y + iYOffset;

			SDL_Rect rect{
				.x = m_rtRect.x + m_CursorScreenPos.x,
				.y = m_rtRect.y + m_CursorScreenPos.y,
				.w = 0,
				.h = iTextHeight,
			};
			cIECore::updateTextEditPosition(rect);
		}

		if (szTextLength <= szEndPoint + iNextOffset || szEndPoint == std::string::npos)
			break;

		szBegPoint = szEndPoint + iNextOffset;

		if (bIsEnterLine)
		{
			iYOffset += m_iFontHeight;
			iXOffset = 0;
		}
		else
		{
			iXOffset += iWidthOffset;
		}
	}
}

#else
// ──────────────────────────────────────────────
//  새 렌더링 경로 (FreeType + HarfBuzz)
// ──────────────────────────────────────────────

void cTextBox::transToTexture()
{
	if (!m_lpRenderer)
		return;
	if (m_lpFont == nullptr)
		return;

	cFontFace* pFontFace = m_lpFont->get();
	if (!pFontFace || !pFontFace->ftFace)
		return;

	m_bTextChanged = false;
	size_t szHash = std::hash<std::string>{}(m_strText);
	if (m_szDrawHash == szHash)
		return;

	m_szDrawHash = szHash;
	resetTexture();

std::stack<SDL_Color> stkColor;
	stkColor.push(m_Color);

	// 스타일 스택: int (IE_FONT_STYLE_*)
	std::stack<int> stkStyle;
	stkStyle.push(IE_FONT_STYLE_NORMAL);

	size_t szTextLength = m_strText.length();
	if (szTextLength == 0)
		return;

	FT_Face ftFace = pFontFace->ftFace;
	int iAscent = m_lpFont->getAscent();

	size_t szBegPoint = 0;
	int iXOffset = 0;
	int iYOffset = 0;

	while (szTextLength > szBegPoint)
	{
		size_t szEndPoint = szTextLength;
		bool bIsEnterLine = false;
		size_t iNextOffset = 0;

		// 마크업 파싱 (기존 로직 동일)
		{
			size_t szCodeBegPoint = m_strText.find("<s:", szBegPoint);
			if (szCodeBegPoint == szBegPoint)
			{
				size_t szCodeEndPoint = m_strText.find(">", szBegPoint);
				if (szCodeEndPoint != std::string::npos)
				{
					++szCodeEndPoint;
					std::string strStyleCode;
					strStyleCode.append(m_strText, szCodeBegPoint, szCodeEndPoint - szCodeBegPoint);
					int iStyle = IE_FONT_STYLE_NORMAL;
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
			size_t szClosePoint = m_strText.find(c_csCloseCode, szBegPoint);
			if (szClosePoint == szBegPoint)
			{
				if (stkStyle.size() > 1) stkStyle.pop();
				szBegPoint += std::strlen(c_csCloseCode);
				continue;
			}
			if (szClosePoint < szEndPoint) szEndPoint = szClosePoint;
		}
		{
			size_t szCodeBegPoint = m_strText.find("<c:", szBegPoint);
			if (szCodeBegPoint == szBegPoint)
			{
				size_t szCodeEndPoint = m_strText.find(">", szBegPoint);
				if (szCodeEndPoint != std::string::npos)
				{
					++szCodeEndPoint;
					std::string strStyleCode;
					strStyleCode.append(m_strText, szCodeBegPoint, szCodeEndPoint - szCodeBegPoint);
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
			size_t szClosePoint = m_strText.find(c_csCloseCode, szBegPoint);
			if (szClosePoint == szBegPoint)
			{
				if (stkColor.size() > 1) stkColor.pop();
				szBegPoint += std::strlen(c_csCloseCode);
				continue;
			}
			if (szClosePoint < szEndPoint) szEndPoint = szClosePoint;
		}
		{
			size_t szEnterPoint = m_strText.find("\n", szBegPoint);
			if (szEnterPoint != std::string::npos && szEnterPoint < szEndPoint)
			{
				bIsEnterLine = true;
				++iNextOffset;
				szEndPoint = szEnterPoint;
			}
		}

		std::string strTargetText;
		strTargetText.append(m_strText, szBegPoint, szEndPoint - szBegPoint);

		// 현재 스타일의 폰트 페이스 가져오기
		cFontFace* pCurFace = m_lpFont->get(stkStyle.top());
		if (!pCurFace) pCurFace = pFontFace;
		FT_Face curFTFace = pCurFace->ftFace;
		hb_font_t* curHBFont = pCurFace->hbFont;

		// 너비 제한 처리
		if (m_rtRect.w != 0 && m_rtRect.h != 0 && (m_iTextBoxStyle & dTEXT_BOX_AUTO_NEXTLINE))
		{
			auto shaped = cTextRenderer::shape(curHBFont, strTargetText.c_str(), (int)strTargetText.size());
			auto m = cTextRenderer::measure(curFTFace, shaped);
			int iTextWidth = m.width;

			if (iXOffset + iTextWidth > m_rtRect.w)
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
							if ((m_iTextBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE) && bIsCorrectSpace)
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

						auto ts = cTextRenderer::shape(curHBFont, strResultText.c_str(), (int)strResultText.size());
						int iTempWidth = cTextRenderer::measure(curFTFace, ts).width;

						if (m_iTextBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE)
						{
							if (m_rtRect.w < iXOffset + iTempWidth)
							{
								size_t spaceBeginPoint = strResultText.find_last_not_of(' ');
								if (spaceBeginPoint != std::string::npos)
								{
									std::string tempString;
									tempString.append(strResultText, 0, spaceBeginPoint + 1);
									auto ts2 = cTextRenderer::shape(curHBFont, tempString.c_str(), (int)tempString.size());
									int spaceRemoveWidth = cTextRenderer::measure(curFTFace, ts2).width;
									if (iXOffset + spaceRemoveWidth <= m_rtRect.w)
									{
										iNextOffset += strResultText.length() - spaceBeginPoint - 1;
										strResultText = tempString;
										break;
									}
								}
							}
						}

						if (m_rtRect.w < iXOffset + iTempWidth)
						{
							strResultText.erase(szResultLength);
							if ((m_iTextBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE) && bIsCorrectSpace)
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
						size_t szSecondPos = cStrUTF8::getMemoryPoint(strTargetText, 1);
						size_t szResultLength = strResultText.length();
						strResultText.append(strTargetText, 0, szSecondPos);

						auto ts = cTextRenderer::shape(curHBFont, strResultText.c_str(), (int)strResultText.size());
						int iTempWidth = cTextRenderer::measure(curFTFace, ts).width;

						if (iXOffset + iTempWidth > m_rtRect.w)
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

		int iWidthOffset = 0;

		if (strTargetText.length())
		{
			auto shaped = cTextRenderer::shape(curHBFont, strTargetText.c_str(), (int)strTargetText.size());
			auto ms     = cTextRenderer::measure(curFTFace, shaped);

			SDL_Texture* pTex = cTextRenderer::renderToTexture(
				m_lpRenderer->m_pRenderer,
				curFTFace,
				shaped,
				stkColor.top(),
				ms.width  > 0 ? ms.width  : 1,
				ms.height > 0 ? ms.height : 1,
				iAscent);

			if (pTex)
			{
				cTextTexture* pTTexture = new cTextTexture();
				pTTexture->m_pTexture = pTex;
				pTTexture->m_iBufferPos = static_cast<int>(szBegPoint);
				pTTexture->m_rtRect.x = iXOffset;
				pTTexture->m_rtRect.y = iYOffset;
				pTTexture->m_rtRect.w = ms.width;
				pTTexture->m_rtRect.h = ms.height;

				m_listTexture.push_back(pTTexture);
				iWidthOffset = ms.width;
			}
		}

		// 커서 픽셀 위치 계산
		size_t cursurPos = m_szCursorPos - m_szIMEInputLength;
		if (szBegPoint <= cursurPos && cursurPos < szEndPoint + iNextOffset)
		{
			std::string prefix = strTargetText.substr(0, cursurPos - szBegPoint);
			auto ps = cTextRenderer::shape(curHBFont, prefix.c_str(), (int)prefix.size());
			int iTextWidth  = cTextRenderer::measure(curFTFace, ps).width;
			int iTextHeight = m_iFontHeight;

			m_CursorScreenPos.x = m_rtRect.x + iXOffset + iTextWidth;
			m_CursorScreenPos.y = m_rtRect.y + iYOffset;

			SDL_Rect rect{
				.x = m_rtRect.x + m_CursorScreenPos.x,
				.y = m_rtRect.y + m_CursorScreenPos.y,
				.w = 0,
				.h = iTextHeight,
			};
			cIECore::updateTextEditPosition(rect);
		}

		if (szTextLength <= szEndPoint + iNextOffset || szEndPoint == std::string::npos)
			break;

		szBegPoint = szEndPoint + iNextOffset;

		if (bIsEnterLine)
		{
			iYOffset += m_iFontHeight;
			iXOffset = 0;
		}
		else
		{
			iXOffset += iWidthOffset;
		}
	}
}
#endif // IE_LEGACY_TTF

// ──────────────────────────────────────────────
//  공통 (양쪽 경로 동일)
// ──────────────────────────────────────────────

void cTextBox::draw()
{
	if (m_lpRenderer == nullptr)
		return;

	if (m_bTextChanged)
		transToTexture();

	std::list<cTextTexture*>::iterator ite = m_listTexture.begin();
	for (; ite != m_listTexture.end(); ++ite)
	{
		cTextTexture* pTTexture = *ite;
		m_lpRenderer->drawTexture(pTTexture->m_pTexture, m_rtRect.x + pTTexture->m_rtRect.x, m_rtRect.y + pTTexture->m_rtRect.y);
	}
}

void cTextBox::update()
{
}

bool cTextBox::operateStyleCode(const std::string* _lpStrText, int* _lpOutResult)
{
	size_t szCursor = _lpStrText->find_first_not_of("<s:");

	if (szCursor == std::string::npos || _lpStrText->find_first_of("<s:") == std::string::npos)
		return false;

	if (_lpStrText->find_last_of(">") == std::string::npos)
		return false;

#ifdef IE_LEGACY_TTF
	int _iResult = TTF_STYLE_NORMAL;
	if (_lpStrText->find("BOLD",          szCursor) != std::string::npos) _iResult |= TTF_STYLE_BOLD;
	if (_lpStrText->find("ITALIC",        szCursor) != std::string::npos) _iResult |= TTF_STYLE_ITALIC;
	if (_lpStrText->find("UNDERLINE",     szCursor) != std::string::npos) _iResult |= TTF_STYLE_UNDERLINE;
	if (_lpStrText->find("STRIKETHROUGH", szCursor) != std::string::npos) _iResult |= TTF_STYLE_STRIKETHROUGH;
#else
	int _iResult = IE_FONT_STYLE_NORMAL;
	if (_lpStrText->find("BOLD",          szCursor) != std::string::npos) _iResult |= IE_FONT_STYLE_BOLD;
	if (_lpStrText->find("ITALIC",        szCursor) != std::string::npos) _iResult |= IE_FONT_STYLE_ITALIC;
	if (_lpStrText->find("UNDERLINE",     szCursor) != std::string::npos) _iResult |= IE_FONT_STYLE_UNDERLINE;
	if (_lpStrText->find("STRIKETHROUGH", szCursor) != std::string::npos) _iResult |= IE_FONT_STYLE_STRIKETHROUGH;
#endif

	*_lpOutResult = _iResult;
	return true;
}

bool cTextBox::operateColorCode(const std::string* _lpStrText, SDL_Color* _lpOutResult)
{
	size_t szEndPoint = _lpStrText->find_last_of(">");
	if (szEndPoint == std::string::npos)
		return false;

	if (_lpStrText->find_first_of("<c:0x") != std::string::npos)
	{
		size_t szPos = _lpStrText->find_first_not_of("<c:0x");
		std::string strTempString;
		strTempString.append(*_lpStrText, szPos, szEndPoint - szPos);

		std::stringstream StrStream;
		Uint32 uiHexValue = 0;
		StrStream << std::hex << strTempString;
		StrStream >> uiHexValue;

		_lpOutResult->b = uiHexValue & 0xff; uiHexValue >>= 8;
		_lpOutResult->g = uiHexValue & 0xff; uiHexValue >>= 8;
		_lpOutResult->r = uiHexValue & 0xff; uiHexValue >>= 8;
		_lpOutResult->a = uiHexValue & 0xff;

		return true;
	}
	return false;
}
