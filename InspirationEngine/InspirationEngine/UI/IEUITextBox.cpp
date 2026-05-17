#include "InspirationEngine.h"

void IEUITextBox::TransToTexture()
{
	if (GetRenderer() == nullptr)
		return;
	if (m_font == nullptr)
		return;

	IEFontFace* pFontFace = m_font->Get();
	if (pFontFace == nullptr || pFontFace->m_ftFace == nullptr)
		return;

	m_textChanged = false;
	size_t hash = std::hash<std::string>{}(m_text);
	hash ^= std::hash<size_t>{}(m_cursorPos) + 0x9e3779b9ull + (hash << 6) + (hash >> 2);
	if (m_drawHash == hash)
		return;

	m_drawHash = hash;
	ResetTexture();

	std::stack<SDL_Color> stkColor;
	stkColor.push(m_color);

	// 스타일 스택: int32_t (IE_FONT_STYLE_*)
	std::stack<int32_t> stkStyle;
	stkStyle.push(IE_FONT_STYLE_NORMAL);

	size_t textLength = m_text.length();
	if (textLength == 0)
		return;

	int32_t ascent = m_font->GetAscent();

	size_t begPoint = 0;
	int32_t xOffset = 0;
	int32_t yOffset = 0;

	while (textLength > begPoint)
	{
		size_t endPoint = textLength;
		bool isEnterLine = false;
		size_t nextOffset = 0;

		// 마크업 파싱 (기존 로직 동일)
		{
			size_t codeBegPoint = m_text.find("<s:", begPoint);
			if (codeBegPoint == begPoint)
			{
				size_t codeEndPoint = m_text.find(">", begPoint);
				if (codeEndPoint != std::string::npos)
				{
					++codeEndPoint;
					std::string styleCode;
					styleCode.append(m_text, codeBegPoint, codeEndPoint - codeBegPoint);
					int32_t style = IE_FONT_STYLE_NORMAL;
					if (OperateStyleCode(&styleCode, &style))
					{
						stkStyle.push(style);
						begPoint = codeEndPoint;
						continue;
					}
				}
				codeBegPoint = std::string::npos;
			}
			if (codeBegPoint < endPoint)
				endPoint = codeBegPoint;
		}
		{
			const char* c_csCloseCode = "</s>";
			size_t closePoint = m_text.find(c_csCloseCode, begPoint);
			if (closePoint == begPoint)
			{
				if (stkStyle.size() > 1)
					stkStyle.pop();
				begPoint += std::strlen(c_csCloseCode);
				continue;
			}
			if (closePoint < endPoint)
				endPoint = closePoint;
		}
		{
			size_t codeBegPoint = m_text.find("<c:", begPoint);
			if (codeBegPoint == begPoint)
			{
				size_t codeEndPoint = m_text.find(">", begPoint);
				if (codeEndPoint != std::string::npos)
				{
					++codeEndPoint;
					std::string styleCode;
					styleCode.append(m_text, codeBegPoint, codeEndPoint - codeBegPoint);
					SDL_Color fontColor;
					if (OperateColorCode(&styleCode, &fontColor))
					{
						stkColor.push(fontColor);
						begPoint = codeEndPoint;
						continue;
					}
				}
				codeBegPoint = std::string::npos;
			}
			if (codeBegPoint < endPoint)
				endPoint = codeBegPoint;
		}
		{
			const char* c_csCloseCode = "</c>";
			size_t closePoint = m_text.find(c_csCloseCode, begPoint);
			if (closePoint == begPoint)
			{
				if (stkColor.size() > 1)
					stkColor.pop();
				begPoint += std::strlen(c_csCloseCode);
				continue;
			}
			if (closePoint < endPoint)
				endPoint = closePoint;
		}
		{
			size_t enterPoint = m_text.find("\n", begPoint);
			if (enterPoint != std::string::npos && enterPoint < endPoint)
			{
				isEnterLine = true;
				++nextOffset;
				endPoint = enterPoint;
			}
		}

		std::string targetText;
		targetText.append(m_text, begPoint, endPoint - begPoint);

		// 현재 스타일의 폰트 페이스 가져오기
		IEFontFace* pCurFace = m_font->Get(stkStyle.top());
		if (pCurFace == nullptr)
			pCurFace = pFontFace;
		FT_Face curFTFace = pCurFace->m_ftFace;
		hb_font_t* curHBFont = pCurFace->m_hbFont;

		// FT_Face / hb_font_t 는 스레드 비안전 — 이 이터레이션 전체 직렬화
		std::lock_guard<std::mutex> faceLock(pCurFace->m_mutex);

		// 너비 제한 처리
		if (m_rect.w != 0 && m_rect.h != 0 && (m_textBoxStyle & dTEXT_BOX_AUTO_NEXTLINE))
		{
			auto shaped = IETextRenderer::shape(curHBFont, targetText.c_str(), static_cast<int32_t>(targetText.size()));
			auto m = IETextRenderer::measure(curFTFace, shaped);
			int32_t textWidth = m.width;

			if (xOffset + textWidth > m_rect.w)
			{
				bool isSpaceCut = true;
				bool isCorrectSpace = false;
				std::string resultText;

				while (targetText.length())
				{
					if (isSpaceCut)
					{
						size_t spaceBeginPos = targetText.find_first_of(" ");
						if (spaceBeginPos == std::string::npos)
						{
							if ((m_textBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE) && isCorrectSpace)
							{
								size_t removeSpaceBegin = resultText.find_last_not_of(' ');
								size_t removeSpaceEnd   = resultText.find_last_of(' ');
								if (removeSpaceBegin != std::string::npos && removeSpaceEnd != std::string::npos)
								{
									resultText.erase(removeSpaceBegin + 1, removeSpaceEnd);
									nextOffset += removeSpaceEnd - removeSpaceBegin;
								}
								break;
							}
							isSpaceCut = false;
							continue;
						}
						size_t spaceEndPos = targetText.find_first_not_of(" ", spaceBeginPos + 1);

						size_t resultLength = resultText.length();
						if (spaceEndPos == std::string::npos)
							resultText.append(targetText, 0, spaceBeginPos + 1);
						else
							resultText.append(targetText, 0, spaceEndPos);

						auto ts = IETextRenderer::shape(curHBFont, resultText.c_str(), static_cast<int32_t>(resultText.size()));
						int32_t tempWidth = IETextRenderer::measure(curFTFace, ts).width;

						if (m_textBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE)
						{
							if (m_rect.w < xOffset + tempWidth)
							{
								size_t spaceBeginPoint = resultText.find_last_not_of(' ');
								if (spaceBeginPoint != std::string::npos)
								{
									std::string tempString;
									tempString.append(resultText, 0, spaceBeginPoint + 1);
									auto ts2 = IETextRenderer::shape(curHBFont, tempString.c_str(), static_cast<int32_t>(tempString.size()));
									int32_t spaceRemoveWidth = IETextRenderer::measure(curFTFace, ts2).width;
									if (xOffset + spaceRemoveWidth <= m_rect.w)
									{
										nextOffset += resultText.length() - spaceBeginPoint - 1;
										resultText = tempString;
										break;
									}
								}
							}
						}

						if (m_rect.w < xOffset + tempWidth)
						{
							resultText.erase(resultLength);
							if ((m_textBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE) && isCorrectSpace)
								break;
							isSpaceCut = false;
							continue;
						}

						if (spaceEndPos == std::string::npos)
							targetText.erase(0, spaceBeginPos + 1);
						else
							targetText.erase(0, spaceEndPos);
						isCorrectSpace = true;
					}
					else
					{
						size_t secondPos = IEStrUTF8::GetMemoryPoint(targetText, 1);
						size_t resultLength = resultText.length();
						resultText.append(targetText, 0, secondPos);

						auto ts = IETextRenderer::shape(curHBFont, resultText.c_str(), static_cast<int32_t>(resultText.size()));
						int32_t tempWidth = IETextRenderer::measure(curFTFace, ts).width;

						if (xOffset + tempWidth > m_rect.w)
						{
							resultText.erase(resultLength);
							break;
						}
						targetText.erase(0, secondPos);
					}
				}
				targetText = resultText;

				if (targetText.length() == 0)
					break;
				if (isSpaceCut == false)
					nextOffset = 0;

				endPoint = begPoint + targetText.length();
				isEnterLine = true;
			}
		}

		int32_t widthOffset = 0;

		if (targetText.length())
		{
			auto shaped = IETextRenderer::shape(curHBFont, targetText.c_str(), static_cast<int32_t>(targetText.size()));
			auto ms     = IETextRenderer::measure(curFTFace, shaped);

			SDL_Texture* pTex = IETextRenderer::renderToTexture(
				GetRenderer()->GetSDLRenderer(),
				curFTFace,
				shaped,
				stkColor.top(),
				ms.width  > 0 ? ms.width  : 1,
				ms.height > 0 ? ms.height : 1,
				ascent,
				pCurFace->m_bold);

			if (pTex != nullptr)
			{
				auto pTTexture = std::make_unique<IETextTexture>();
				pTTexture->m_texture = pTex;
				pTTexture->m_bufferPos = static_cast<int32_t>(begPoint);
				pTTexture->m_rect.x = xOffset;
				pTTexture->m_rect.y = yOffset;
				pTTexture->m_rect.w = ms.width;
				pTTexture->m_rect.h = ms.height;

				m_textures.push_back(std::move(pTTexture));
				widthOffset = ms.width;
			}
		}

		// 커서 픽셀 위치 계산
		size_t cursorPos = m_cursorPos - m_imeInputLength;
		if (begPoint <= cursorPos && cursorPos < endPoint + nextOffset)
		{
			std::string prefix = targetText.substr(0, cursorPos - begPoint);
			auto ps = IETextRenderer::shape(curHBFont, prefix.c_str(), static_cast<int32_t>(prefix.size()));
			int32_t prefixWidth  = IETextRenderer::measure(curFTFace, ps).width;
			int32_t textHeight = m_fontHeight;

			m_cursorScreenPos.x = m_rect.x + xOffset + prefixWidth;
			m_cursorScreenPos.y = m_rect.y + yOffset;

			SDL_Rect rect{
				.x = m_cursorScreenPos.x,
				.y = m_cursorScreenPos.y,
				.w = 0,
				.h = textHeight,
			};
			IECore::UpdateTextEditPosition(rect);
		}

		if (textLength <= endPoint + nextOffset || endPoint == std::string::npos)
			break;

		begPoint = endPoint + nextOffset;

		if (isEnterLine)
		{
			yOffset += m_fontHeight;
			xOffset = 0;
		}
		else
		{
			xOffset += widthOffset;
		}
	}
}

// ──────────────────────────────────────────────
//  공통 (양쪽 경로 동일)
// ──────────────────────────────────────────────

void IEUITextBox::Draw()
{
	if (GetRenderer() == nullptr)
		return;

	IERenderer* ieRenderer = GetRenderer();
	uint32_t    curGen     = ieRenderer->GetGeneration();
	if (ieRenderer != m_lastUsedIERenderer || curGen != m_lastRendererGen)
	{
		// SDL이 렌더러 파괴 시 연결된 텍스처도 모두 해제 — dangling 포인터 방지
		for (auto& t : m_textures)
			t->m_texture = nullptr;
		m_textures.clear();
		m_drawHash           = 0;
		m_textChanged        = true;
		m_lastUsedIERenderer = ieRenderer;
		m_lastRendererGen    = curGen;
	}

	if (m_textChanged)
		TransToTexture();

	for (const auto& pTTexture : m_textures)
		GetRenderer()->DrawTexture(pTTexture->m_texture, m_rect.x + pTTexture->m_rect.x, m_rect.y + pTTexture->m_rect.y);

	if (IECore::GetFocusedTextBox() != this)
		return;

	IERenderer* r = GetRenderer();
	if (r == nullptr)
		return;

	constexpr SDL_Color kFocusBorderCol = { 80, 130, 210, 200 };
	r->DrawLine(kFocusBorderCol, m_rect.x,             m_rect.y,             m_rect.x + m_rect.w, m_rect.y);
	r->DrawLine(kFocusBorderCol, m_rect.x,             m_rect.y + m_rect.h,  m_rect.x + m_rect.w, m_rect.y + m_rect.h);
	r->DrawLine(kFocusBorderCol, m_rect.x,             m_rect.y,             m_rect.x,             m_rect.y + m_rect.h);
	r->DrawLine(kFocusBorderCol, m_rect.x + m_rect.w,  m_rect.y,             m_rect.x + m_rect.w,  m_rect.y + m_rect.h);

	if (m_cursorPos != std::string::npos)
	{
		int32_t curX;
		if (m_textures.empty())
			curX = m_rect.x + 2;
		else if (m_cursorPos >= m_text.size())
			curX = m_rect.x + m_textures.back()->m_rect.x + m_textures.back()->m_rect.w;
		else
			curX = m_cursorScreenPos.x;

		constexpr SDL_Color kCursorCol = { 220, 220, 220, 255 };
		r->DrawLine(kCursorCol, curX, m_rect.y + 2, curX, m_rect.y + m_rect.h - 3);
	}
}

void IEUITextBox::Update()
{
	if (!(m_textBoxStyle & dTEXT_BOX_STYLE_EDITABLE))
		return;

	IEWindow* ownerWindow = GetOwnerWindow();
	if (ownerWindow == nullptr)
		return;

	float gx = 0.0f, gy = 0.0f;
	SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gx, &gy);
	bool lmb = (btn & SDL_BUTTON_LMASK) != 0;

	if (IECore::GetMouseOnWindow() != ownerWindow)
	{
		m_prevLMB = lmb;
		return;
	}

	int32_t winX = 0, winY = 0;
	SDL_GetWindowPosition(ownerWindow->GetSDLWindow(), &winX, &winY);
	int32_t mx = static_cast<int32_t>(gx) - winX;
	int32_t my = static_cast<int32_t>(gy) - winY;

	bool clicked = lmb && !m_prevLMB;
	m_prevLMB = lmb;

	if (!clicked)
		return;

	bool onRect = (mx >= m_rect.x && mx < m_rect.x + m_rect.w &&
	               my >= m_rect.y && my < m_rect.y + m_rect.h);

	if (onRect)
	{
		IECore::SetFocusedTextBox(this);
		m_cursorPos     = m_text.length();
		m_overwriteMode = true;
		m_textChanged   = true;
	}
	else if (IECore::GetFocusedTextBox() == this)
	{
		IECore::SetFocusedTextBox(nullptr);
		m_cursorPos   = std::string::npos;
		m_textChanged = true;
	}
}

bool IEUITextBox::OperateStyleCode(const std::string* strText, int32_t* outResult)
{
	size_t cursor = strText->find_first_not_of("<s:");

	if (cursor == std::string::npos || strText->find_first_of("<s:") == std::string::npos)
		return false;

	if (strText->find_last_of(">") == std::string::npos)
		return false;

	int32_t result = IE_FONT_STYLE_NORMAL;
	if (strText->find("BOLD",          cursor) != std::string::npos)
		result |= IE_FONT_STYLE_BOLD;
	if (strText->find("ITALIC",        cursor) != std::string::npos)
		result |= IE_FONT_STYLE_ITALIC;
	if (strText->find("UNDERLINE",     cursor) != std::string::npos)
		result |= IE_FONT_STYLE_UNDERLINE;
	if (strText->find("STRIKETHROUGH", cursor) != std::string::npos)
		result |= IE_FONT_STYLE_STRIKETHROUGH;

	*outResult = result;
	return true;
}

bool IEUITextBox::OperateColorCode(const std::string* strText, SDL_Color* outResult)
{
	size_t endPoint = strText->find_last_of(">");
	if (endPoint == std::string::npos)
		return false;

	if (strText->find_first_of("<c:0x") != std::string::npos)
	{
		size_t pos = strText->find_first_not_of("<c:0x");
		std::string tempString;
		tempString.append(*strText, pos, endPoint - pos);

		std::stringstream strStream;
		Uint32 hexValue = 0;
		strStream << std::hex << tempString;
		strStream >> hexValue;

		outResult->b = hexValue & 0xff; hexValue >>= 8;
		outResult->g = hexValue & 0xff; hexValue >>= 8;
		outResult->r = hexValue & 0xff; hexValue >>= 8;
		outResult->a = hexValue & 0xff;

		return true;
	}
	return false;
}
