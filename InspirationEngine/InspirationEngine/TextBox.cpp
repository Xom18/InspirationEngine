#include "InspirationEngine.h"

void cTextBox::transToTexture()
{
	if(!m_lpRenderer)
		return;

	if(m_lpFont == nullptr)
		return;
	if(m_lpFont->get() == nullptr)
		return;

	//이전에 이미 처리한적있는지 해시 비교로 확인
	m_bTextChanged = false;
	size_t szHash = std::hash<std::string>{}(m_strText);
	if(m_szDrawHash == szHash)
		return;
	m_szDrawHash = szHash;

	//텍스쳐 해제
	resetTexture();

	//해시 갱신

	//컬러코드 처리용
	std::stack<SDL_Color> stkColor;
	stkColor.push(m_Color);

	//폰트 스타일 처리용
	std::stack<TTF_Font*> stkFont;
	stkFont.push(m_lpFont->get());

	size_t szTextLength = m_strText.length();

	//궂이 해 줄 필요가 없는거임
	if(szTextLength == 0)
		return;

	size_t szBegPoint = 0;

	int iXOffset = 0;
	int iYOffset = 0;
	//텍스트를 기반으로 텍스쳐화
	while(szTextLength > szBegPoint)
	{
		size_t szEndPoint = szTextLength;
		//엔터를 통해 개행이 처리되었는지
		bool bIsEnterLine = false;
		int iNextOffset = 0;//부호 오프셋

		{
			//특수 문자 처리(개행, 스타일, 컬러)
			size_t szEnterPoint = m_strText.find("\n", szBegPoint);
			if(szEnterPoint != std::string::npos)
			{
				bIsEnterLine = true;
				iNextOffset = 1;
				szEndPoint = szEnterPoint;
			}
		}
		
		{
			//스타일 코드 처리
			size_t szCodeBegPoint = m_strText.find("<s:", szBegPoint);
			if(szCodeBegPoint == szBegPoint)
			{
				size_t szCodeEndPoint = m_strText.find(">", szBegPoint);
				//코드 시작인대 끝을 못찾겠다고 함
				if(szCodeEndPoint != std::string::npos)
				{
					++szCodeEndPoint;
					std::string strStyleCode;
					strStyleCode.append(m_strText, szCodeBegPoint, szCodeEndPoint - szCodeBegPoint);
					int iStyle = TTF_STYLE_NORMAL;
					if(operateStyleCode(&strStyleCode, &iStyle))
					{
						stkFont.push(m_lpFont->get(iStyle));

						szBegPoint = szCodeEndPoint;
						continue;
					}
				}
				szCodeBegPoint = std::string::npos;
			}

			if(szCodeBegPoint < szEndPoint)
			{
				bIsEnterLine = false;
				szEndPoint = szCodeBegPoint;
			}
		}

		{
			//스타일 종료 코드 처리
			const char* c_csCloseCode = "</s>";
			size_t szClosePoint = m_strText.find(c_csCloseCode, szBegPoint);
			if(szClosePoint == szBegPoint)
			{
				if(stkFont.size() > 1)
					stkFont.pop();

				szBegPoint += std::strlen(c_csCloseCode);
				continue;
			}

			if(szClosePoint < szEndPoint)
			{
				bIsEnterLine = false;
				szEndPoint = szClosePoint;
			}
		}

		{
			//컬러 코드 처리
			size_t szCodeBegPoint = m_strText.find("<c:", szBegPoint);
			if(szCodeBegPoint == szBegPoint)
			{
				size_t szCodeEndPoint = m_strText.find(">", szBegPoint);
				//코드 시작인대 끝을 못찾겠다고 함
				if(szCodeEndPoint != std::string::npos)
				{
					++szCodeEndPoint;
					std::string strStyleCode;
					strStyleCode.append(m_strText, szCodeBegPoint, szCodeEndPoint - szCodeBegPoint);
					SDL_Color FontColor;
					if(operateColorCode(&strStyleCode, &FontColor))
					{
						stkColor.push(FontColor);

						szBegPoint = szCodeEndPoint;
						continue;
					}
				}
				szCodeBegPoint = std::string::npos;
			}

			if(szCodeBegPoint < szEndPoint)
			{
				bIsEnterLine = false;
				szEndPoint = szCodeBegPoint;
			}
		}

		{
			//컬러 종료 코드 처리
			const char* c_csCloseCode = "</c>";
			size_t szClosePoint = m_strText.find(c_csCloseCode, szBegPoint);
			if(szClosePoint == szBegPoint)
			{
				if(stkColor.size() > 1)
					stkColor.pop();

				szBegPoint += std::strlen(c_csCloseCode);
				continue;
			}

			if(szClosePoint < szEndPoint)
			{
				bIsEnterLine = false;
				szEndPoint = szClosePoint;
			}
		}

		std::string strTargetText;
		strTargetText.append(m_strText, szBegPoint, szEndPoint - szBegPoint);

		//너비가 제한되있으면 제한된 너비만큼 처리
		if(m_rtRect.w != 0 && m_rtRect.h != 0 && (m_iTextBoxStyle & dTEXT_BOX_AUTO_NEXTLINE))
		{
			int iTextWidth = 0;
			int iTextHeight = 0;

			TTF_SizeUTF8(stkFont.top(), strTargetText.c_str(), &iTextWidth, &iTextHeight);

			//너비가 설정되있는 한계를 초과
			if(iXOffset + iTextWidth > m_rtRect.w)
			{
				bool bIsSpaceCut = true;
				bool bIsCorrectSpace = false;

				std::string strResultText;
				//너비에 맞게 줄여나감
				while(strTargetText.length())
				{
					if(bIsSpaceCut)
					{
						size_t szSpacePos = strTargetText.find_first_of(" ");

						//스페이스바로 안잘린다 스페이스바로 자르는건 포기한다
						if(szSpacePos == std::string::npos)
						{
							//스페이스바 단위로 끊어주는건대 스페이스바를 본적이 있다
							if((m_iTextBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE) && bIsCorrectSpace)
							{
								break;
							}

							bIsSpaceCut = false;
							continue;
						}

						//제일 뒤쪽에 스페이스바 앞쪽 문자를 이어붙여줌
						size_t szResultLength = strResultText.length();
						strResultText.append(strTargetText, 0, szSpacePos + 1);

						int iTempWidth = 0;
						int iTempHeight = 0;
						TTF_SizeUTF8(stkFont.top(), strResultText.c_str(), &iTempWidth, &iTempHeight);

						//스페이스바까지 잘랐는대 너비가 초과다
						if(iXOffset + iTempWidth > m_rtRect.w)
						{
							//넣은건 취소해준다
							strResultText.erase(szResultLength);

							//스페이스바 단위로 끊어주는건대 스페이스바를 본적이 있다
							if((m_iTextBoxStyle & dTEXT_BOX_AUTO_SPACE_NEXTLINE) && bIsCorrectSpace)
								break;

							//스페이스바를 본적이 없다. 얘는 한정된 너비 안에 스페이스바가 없는 경우다
							//또는 스페이스바 단위로 안끊는애다
							bIsSpaceCut = false;
							continue;
						}

						//앞에 문자 지우기(스페이스바도 같이 지움)
						strTargetText.erase(0, szSpacePos + 1);

						//스페이스바 찾은거 체크
						bIsCorrectSpace = true;
					}
					else
					{
						//제일 앞에문자를 제일 뒤에다 이어붙인다
						size_t szSecondPos = cStrUTF8::getMemoryPoint(&strTargetText, 1);
						size_t szResultLength = strResultText.length();
						strResultText.append(strTargetText, 0, szSecondPos);

						int iTempWidth = 0;
						int iTempHeight = 0;
						TTF_SizeUTF8(stkFont.top(), strResultText.c_str(), &iTempWidth, &iTempHeight);

						//너비 재봤더니 초과했다 넣은건 취소하고 중단
						if(iXOffset + iTempWidth > m_rtRect.w)
						{
							strResultText.erase(szResultLength);
							break;
						}

						//앞에 문자 지우기
						strTargetText.erase(0, szSecondPos);
					}
				}
				strTargetText = strResultText;

				//텍스트를 띄울 공간도 없다
				if(strTargetText.length() == 0)
					break;

				//엔터로 개행했거나 스페이스로 개행했을 때 말곤 오프셋이 없다
				if(bIsSpaceCut == false)
					iNextOffset = 0;

				//end 포인트 찾기
				szEndPoint = szBegPoint + strTargetText.length();
				bIsEnterLine = true;
			}
		}
		
		int iWidthOffset = 0;

		//텍스트가 없으면 오프셋만 더하고 텍스쳐 생성 안함
		if (strTargetText.length())
		{
			//텍스쳐 생성하고 위치잡고 등등
			SDL_Surface* pSurface = nullptr;
			pSurface = TTF_RenderUTF8_Solid(stkFont.top(), strTargetText.c_str(), stkColor.top());

			cTextTexture* pTTexture = new cTextTexture();
			pTTexture->m_pTexture = SDL_CreateTextureFromSurface(m_lpRenderer->m_pRenderer, pSurface);
			SDL_FreeSurface(pSurface);

			pTTexture->m_iBufferPos = static_cast<int>(szBegPoint);
			pTTexture->m_rtRect.x = iXOffset;
			pTTexture->m_rtRect.y = iYOffset;
			SDL_QueryTexture(pTTexture->m_pTexture, NULL, NULL, &pTTexture->m_rtRect.w, &pTTexture->m_rtRect.h);

			//텍스쳐 리스트에 추가
			m_listTexture.push_back(pTTexture);

			iWidthOffset = pTTexture->m_rtRect.w;
		}

		if(szTextLength <= szEndPoint + iNextOffset
		|| szEndPoint == std::string::npos)
			break;

		szBegPoint = szEndPoint + iNextOffset;

		//줄 바꿈 처리 되었을 때 / 안되었을 때
		if(bIsEnterLine)
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

void cTextBox::draw()
{
	if(m_lpRenderer == nullptr)
		return;

	if(m_bTextChanged)
		transToTexture();

	std::list<cTextTexture*>::iterator ite = m_listTexture.begin();
	for(; ite != m_listTexture.end(); ++ite)
	{
		cTextTexture* pTTexture = *ite;
		m_lpRenderer->drawTexture(pTTexture->m_pTexture, m_rtRect.x + pTTexture->m_rtRect.x, m_rtRect.y + pTTexture->m_rtRect.y);
	}

//	SDL_Color Color;
//	Color.a = 0xaf;
//	Color.r = 0xff;
//	Color.g = 0xff;
//	Color.b = 0xff;
//	m_lpRenderer->drawRect(Color, 0, 0, 500, 500, SDL_BLENDMODE_BLEND);
}

void cTextBox::update()
{

}

bool cTextBox::operateStyleCode(const std::string* _lpStrText, int* _lpOutResult)
{
	size_t szCursor = _lpStrText->find_first_not_of("<s:");

	//올바른 양식의 코드가 아님
	if(szCursor == std::string::npos || _lpStrText->find_first_of("<s:") == std::string::npos)
		return false;

	if(_lpStrText->find_last_of(">") == std::string::npos)
		return false;

	int _iResult = TTF_STYLE_NORMAL;

	if(_lpStrText->find("BOLD", szCursor) != std::string::npos)
		_iResult |= TTF_STYLE_BOLD;
	if(_lpStrText->find("ITALIC", szCursor) != std::string::npos)
		_iResult |= TTF_STYLE_ITALIC;
	if(_lpStrText->find("UNDERLINE", szCursor) != std::string::npos)
		_iResult |= TTF_STYLE_UNDERLINE;
	if(_lpStrText->find("STRIKETHROUGH", szCursor) != std::string::npos)
		_iResult |= TTF_STYLE_STRIKETHROUGH;

	*_lpOutResult = _iResult;

	return true;
}

bool cTextBox::operateColorCode(const std::string* _lpStrText, SDL_Color* _lpOutResult)
{
	//끝나는데가 없다
	size_t szEndPoint = _lpStrText->find_last_of(">");
	if(_lpStrText->find_last_of(">") == std::string::npos)
		return false;

	//hex만 취급함
	if(_lpStrText->find_first_of("<c:0x") != std::string::npos)
	{
		size_t szPos = _lpStrText->find_first_not_of("<c:0x");
		std::string strTempString;
		strTempString.append(*_lpStrText, szPos, szEndPoint - szPos);

		std::stringstream StrStream;
		Uint32 uiHexValue = 0;
		StrStream << std::hex << strTempString;
		StrStream >> uiHexValue;

		//memcpy해서 넣으면abgr이여서 argb로 변환
		_lpOutResult->b = uiHexValue & 0xff;
		uiHexValue >>= 8;
		_lpOutResult->g = uiHexValue & 0xff;
		uiHexValue >>= 8;
		_lpOutResult->r = uiHexValue & 0xff;
		uiHexValue >>= 8;
		_lpOutResult->a = uiHexValue & 0xff;

		return true;
	}
	return false;
}