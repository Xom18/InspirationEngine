#include "InspirationEngine.h"

void cTextBox::transToTexture()
{
	if(!m_lpRenderer)
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

	//컬러코드 처리용 아직 사용 안함
	std::stack<SDL_Color> stkColor;

	size_t szTextLength = m_strText.length();

	//궂이 해 줄 필요가 없는거임
	if(szTextLength == 0)
		return;

	size_t szBegPoint = 0;
	size_t szEndPoint = 0;

	int iXOffset = 0;
	int iYOffset = 0;
	//텍스트를 기반으로 텍스쳐화
	while(true)
	{
		szEndPoint = m_strText.find("\n", szBegPoint);
		std::string strTargetText;
		strTargetText.append(m_strText, szBegPoint, szEndPoint - szBegPoint);

		int iNextOffset = 1;//부호 오프셋
		bool bIsEnterLine = szEndPoint != std::string::npos ? true : false;//이게 엔터를 통해 개행된건지

		//너비가 제한되있으면 제한된 너비만큼 처리
		if(m_rtRect.w != 0 && m_rtRect.h != 0 && (m_iTextBoxStyle & dTEXT_BOX_AUTO_NEXTLINE))
		{
			int iTextWidth = 0;
			int iTextHeight = 0;

			TTF_SizeUTF8(m_lpFont, strTargetText.c_str(), &iTextWidth, &iTextHeight);

			//너비가 설정되있는 한계를 초과
			if(iTextWidth > m_rtRect.w)
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
								break;

							bIsSpaceCut = false;
							continue;
						}

						//제일 뒤쪽에 스페이스바 앞쪽 문자를 이어붙여줌
						size_t szResultLength = strResultText.length();
						strResultText.append(strTargetText, 0, szSpacePos);

						int iTempWidth = 0;
						int iTempHeight = 0;
						TTF_SizeUTF8(m_lpFont, strResultText.c_str(), &iTempWidth, &iTempHeight);

						//스페이스바까지 잘랐는대 너비가 초과다
						if(iTempWidth > m_rtRect.w)
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
						TTF_SizeUTF8(m_lpFont, strResultText.c_str(), &iTempWidth, &iTempHeight);

						//너비 재봤더니 초과했다 넣은건 취소하고 중단
						if(iTempWidth > m_rtRect.w)
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
		
		//텍스쳐 생성하고 위치잡고 등등
		SDL_Surface* pSurface = nullptr;
		pSurface = TTF_RenderUTF8_Solid(m_lpFont, strTargetText.c_str(), m_Color);

		cTextTexture* pTTexture = new cTextTexture();
		pTTexture->m_pTexture = SDL_CreateTextureFromSurface(m_lpRenderer->m_pRenderer, pSurface);
		SDL_FreeSurface(pSurface);

		pTTexture->m_iBufferPos = static_cast<int>(szBegPoint);
		pTTexture->m_rtRect.x = 0;
		pTTexture->m_rtRect.y = iYOffset;
		SDL_QueryTexture(pTTexture->m_pTexture, NULL, NULL, &pTTexture->m_rtRect.w, &pTTexture->m_rtRect.h);

		//텍스쳐 리스트에 추가
		m_listTexture.push_back(pTTexture);

		if(szTextLength <= szEndPoint + iNextOffset
		|| szEndPoint == std::string::npos)
			break;

		szBegPoint = szEndPoint + iNextOffset;
		if(bIsEnterLine)
			iYOffset += m_iFontHeight;
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
}

void cTextBox::update()
{

}