#pragma once

/// <summary>
/// 모든 컴포넌트의 추상 기반 클래스
/// </summary>
class IEComponent
{
public:
	virtual ~IEComponent() = default;

	static size_t AllocTypeId()
	{
		static size_t counter = 0;
		return counter++;
	}

	template<typename T>
	static size_t TypeId()
	{
		static size_t id = AllocTypeId();
		return id;
	}
};

/// <summary>
/// 월드 좌표 및 변환 컴포넌트 — 모든 좌표는 float, z = 높이축
/// </summary>
class IETransformComponent : public IEComponent
{
	float m_x        = 0.0f;
	float m_y        = 0.0f;
	float m_z        = 0.0f;
	float m_rotation = 0.0f;
	float m_scaleX   = 1.0f;
	float m_scaleY   = 1.0f;

public:
	float GetX()        const { return m_x; }
	float GetY()        const { return m_y; }
	float GetZ()        const { return m_z; }
	float GetRotation() const { return m_rotation; }
	float GetScaleX()   const { return m_scaleX; }
	float GetScaleY()   const { return m_scaleY; }

	void SetX(float x)               { m_x = x; }
	void SetY(float y)               { m_y = y; }
	void SetZ(float z)               { m_z = z; }
	void SetRotation(float rotation) { m_rotation = rotation; }
	void SetScaleX(float scaleX)     { m_scaleX = scaleX; }
	void SetScaleY(float scaleY)     { m_scaleY = scaleY; }
};

/// <summary>
/// 속도 컴포넌트 — 매 프레임 transform 에 적용
/// </summary>
class IEVelocityComponent : public IEComponent
{
	float m_vx = 0.0f;
	float m_vy = 0.0f;
	float m_vz = 0.0f;

public:
	float GetVx() const { return m_vx; }
	float GetVy() const { return m_vy; }
	float GetVz() const { return m_vz; }

	void SetVx(float vx) { m_vx = vx; }
	void SetVy(float vy) { m_vy = vy; }
	void SetVz(float vz) { m_vz = vz; }
};

/// <summary>
/// 타일 컴포넌트 — atlas 이름 + 타일 이름으로 IEAtlasManager에서 타일 조회
/// </summary>
class IETileComponent : public IEComponent
{
	std::string m_atlas;
	std::string m_tile;

public:
	const std::string& GetAtlas() const { return m_atlas; }
	const std::string& GetTile()  const { return m_tile; }

	void SetAtlas(const std::string& atlas) { m_atlas = atlas; }
	void SetTile(const std::string& tile)   { m_tile = tile; }
};
