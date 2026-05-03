#pragma once

#include "IEScene.h"

class IERenderer;

/// <summary>
/// 씬 스택 관리자. IECore::m_Scene 으로 전역 접근.
/// Push/Pop/Replace 로 씬 전환, OnEnter/OnExit 자동 호출.
/// </summary>
class IESceneManager
{
	std::stack<std::unique_ptr<IEScene>> m_scenes;

public:
	~IESceneManager()
	{
		while (!m_scenes.empty())
		{
			m_scenes.top()->OnExit();
			m_scenes.pop();
		}
	}

	/// <summary>
	/// 씬 푸시 — 현재 씬 OnExit → 새 씬 OnEnter
	/// </summary>
	/// <param name="scene">추가할 씬 (소유권 이전)</param>
	void Push(IEScene* scene)
	{
		if (!m_scenes.empty())
			m_scenes.top()->OnExit();
		m_scenes.push(std::unique_ptr<IEScene>(scene));
		m_scenes.top()->OnEnter();
	}

	/// <summary>
	/// 현재 씬 팝 — OnExit → 이전 씬 OnEnter
	/// </summary>
	void Pop()
	{
		if (m_scenes.empty())
			return;
		m_scenes.top()->OnExit();
		m_scenes.pop();
		if (!m_scenes.empty())
			m_scenes.top()->OnEnter();
	}

	/// <summary>
	/// 현재 씬 교체 — OnExit → 새 씬 OnEnter
	/// </summary>
	/// <param name="scene">교체할 씬 (소유권 이전)</param>
	void Replace(IEScene* scene)
	{
		if (!m_scenes.empty())
		{
			m_scenes.top()->OnExit();
			m_scenes.pop();
		}
		m_scenes.push(std::unique_ptr<IEScene>(scene));
		m_scenes.top()->OnEnter();
	}

	/// <summary>
	/// 현재 씬 반환 (없으면 nullptr)
	/// </summary>
	IEScene* GetCurrentScene()
	{
		if (m_scenes.empty())
			return nullptr;
		return m_scenes.top().get();
	}

	/// <summary>
	/// 씬 스택이 비어있는지 여부
	/// </summary>
	bool IsEmpty() const { return m_scenes.empty(); }

	/// <summary>
	/// 현재 씬 Update
	/// </summary>
	/// <param name="deltaTime">경과 시간 (초)</param>
	void Update(float deltaTime)
	{
		if (m_scenes.empty())
			return;
		m_scenes.top()->Update(deltaTime);
	}

	/// <summary>
	/// 현재 씬 Draw
	/// </summary>
	/// <param name="renderer">사용할 렌더러</param>
	void Draw(IERenderer* renderer)
	{
		if (m_scenes.empty())
			return;
		m_scenes.top()->Draw(renderer);
	}
};
