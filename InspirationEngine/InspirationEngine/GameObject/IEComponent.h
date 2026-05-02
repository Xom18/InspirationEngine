#pragma once

/// <summary>
/// 모든 컴포넌트의 추상 기반 클래스
/// </summary>
class IEComponent
{
public:
	virtual ~IEComponent() = default;
};

/// <summary>
/// 월드 좌표 및 변환 컴포넌트 — 모든 좌표는 float, z = 높이축
/// </summary>
class IETransformComponent : public IEComponent
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;			// 높이축 (isometric / side view 에서 사용)
	float rotation = 0.0f;	// 회전 각도 (도)
	float scaleX = 1.0f;
	float scaleY = 1.0f;
};

/// <summary>
/// 속도 컴포넌트 — 매 프레임 transform 에 적용
/// </summary>
class IEVelocityComponent : public IEComponent
{
public:
	float vx = 0.0f;
	float vy = 0.0f;
	float vz = 0.0f;	// 높이 방향 속도 (점프 등)
};
