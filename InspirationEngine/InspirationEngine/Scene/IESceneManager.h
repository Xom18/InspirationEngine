#pragma once

#include "IEScene.h"

class IERenderer;

/// <summary>
/// 씬 스택 관리자. IECore::m_Scene 으로 전역 접근.
/// push/pop/replace 로 씬 전환, onEnter/onExit 자동 호출.
/// </summary>
class IESceneManager
{
	std::stack<std::unique_ptr<IEScene>> m_scenes;

public:
	~IESceneManager()
	{
		while (!m_scenes.empty())
		{
			m_scenes.top()->onExit();
			m_scenes.pop();
		}
	}

	/// <summary>
	/// 씬 푸시 — 현재 씬 onExit → 새 씬 onEnter
	/// </summary>
	void push(IEScene* scene)
	{
		if (!m_scenes.empty()) m_scenes.top()->onExit();
		m_scenes.push(std::unique_ptr<IEScene>(scene));
		m_scenes.top()->onEnter();
	}

	/// <summary>
	/// 현재 씬 팝 — onExit → 이전 씬 onEnter
	/// </summary>
	void pop()
	{
		if (m_scenes.empty()) return;
		m_scenes.top()->onExit();
		m_scenes.pop();
		if (!m_scenes.empty()) m_scenes.top()->onEnter();
	}

	/// <summary>
	/// 현재 씬 교체 — onExit → 새 씬 onEnter
	/// </summary>
	void replace(IEScene* scene)
	{
		if (!m_scenes.empty())
		{
			m_scenes.top()->onExit();
			m_scenes.pop();
		}
		m_scenes.push(std::unique_ptr<IEScene>(scene));
		m_scenes.top()->onEnter();
	}

	/// <summary>
	/// 현재 씬 반환 (없으면 nullptr)
	/// </summary>
	IEScene* getCurrentScene()
	{
		if (m_scenes.empty()) return nullptr;
		return m_scenes.top().get();
	}

	bool isEmpty() const { return m_scenes.empty(); }

	void update(float deltaTime)
	{
		if (m_scenes.empty()) return;
		m_scenes.top()->update(deltaTime);
	}

	void draw(IERenderer* renderer)
	{
		if (m_scenes.empty()) return;
		m_scenes.top()->draw(renderer);
	}
};
