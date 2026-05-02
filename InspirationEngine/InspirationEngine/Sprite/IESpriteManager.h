#pragma once

class IERenderer;

/// <summary>
/// 스프라이트 시트 클리핑 정보를 포함한 텍스쳐 뷰 객체
/// </summary>
class IESprite
{
	SDL_Texture* m_texture = nullptr;	// non-owning (IESpriteManager 소유)
	SDL_Rect m_srcRect = { 0, 0, 0, 0 };
	bool m_hasClip = false;

public:

	/// <summary>
	/// 텍스쳐 설정 — 전체 크기로 srcRect 초기화
	/// </summary>
	/// <param name="texture">연결할 텍스쳐 (소유권 이전 없음)</param>
	void setTexture(SDL_Texture* texture);

	/// <summary>
	/// 스프라이트 시트 클리핑 영역 설정
	/// </summary>
	/// <param name="x">클리핑 X</param>
	/// <param name="y">클리핑 Y</param>
	/// <param name="w">클리핑 너비</param>
	/// <param name="h">클리핑 높이</param>
	void setClip(int32_t x, int32_t y, int32_t w, int32_t h)
	{
		m_srcRect = { x, y, w, h };
		m_hasClip = true;
	}

	/// <summary>
	/// 클리핑 해제 — 전체 텍스쳐 사용
	/// </summary>
	void clearClip()
	{
		m_hasClip = false;
	}

	/// <summary>
	/// 텍스쳐 반환
	/// </summary>
	SDL_Texture* getTexture() const
	{
		return m_texture;
	}

	/// <summary>
	/// 클리핑 Rect 반환 — clip 없으면 nullptr
	/// </summary>
	const SDL_Rect* getSrcRect() const
	{
		return m_hasClip ? &m_srcRect : nullptr;
	}

	/// <summary>
	/// 스프라이트 그리기
	/// </summary>
	/// <param name="renderer">사용할 렌더러</param>
	/// <param name="x">화면 X</param>
	/// <param name="y">화면 Y</param>
	/// <param name="scaleX">X 스케일 (1.0 = 원본)</param>
	/// <param name="scaleY">Y 스케일 (1.0 = 원본)</param>
	/// <param name="angle">회전 각도 (도)</param>
	/// <param name="pivot">회전 기준점 (nullptr이면 중앙)</param>
	/// <param name="flip">반전 플래그</param>
	void draw(IERenderer* renderer, int32_t x, int32_t y,
		double scaleX = 1.0, double scaleY = 1.0,
		double angle = 0, SDL_Point* pivot = nullptr,
		SDL_RendererFlip flip = SDL_FLIP_NONE);
};

/// <summary>
/// ID 기반 SDL_Texture 캐시 관리자
/// </summary>
class IESpriteManager
{
	std::map<int32_t, SDL_Texture*> m_textures;				// 텍스쳐 소유
	std::map<int32_t, std::unique_ptr<IESprite>> m_sprites;	// IESprite 소유

public:
	IESpriteManager();
	~IESpriteManager();

	/// <summary>
	/// 이미지 파일 로드 후 ID로 등록
	/// </summary>
	/// <param name="id">스프라이트 ID</param>
	/// <param name="path">이미지 파일 경로</param>
	/// <param name="renderer">텍스쳐 생성에 사용할 렌더러</param>
	bool addNewSprite(int32_t id, const char* path, IERenderer* renderer);

	/// <summary>
	/// ID로 IESprite 반환 — 없으면 nullptr
	/// </summary>
	/// <param name="id">스프라이트 ID</param>
	IESprite* getSprite(int32_t id)
	{
		auto ite = m_sprites.find(id);
		if (ite == m_sprites.end())
			return nullptr;
		return ite->second.get();
	}

	/// <summary>
	/// 해당 ID의 텍스쳐 및 스프라이트 해제
	/// </summary>
	/// <param name="id">해제할 스프라이트 ID</param>
	void unload(int32_t id);

	/// <summary>
	/// 모든 텍스쳐 및 스프라이트 해제
	/// </summary>
	void unloadAll();
};
