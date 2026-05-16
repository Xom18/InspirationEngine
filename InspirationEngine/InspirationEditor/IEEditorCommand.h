#pragma once
#include "../InspirationEngine/InspirationEngine.h"
#include <vector>
#include <memory>
#include <string>

// ─────────────────────────────────────────
// Base
// ─────────────────────────────────────────

class IECommand
{
public:
    virtual ~IECommand() = default;

    /// <summary>커맨드 실행 / 재실행</summary>
    virtual void Execute() = 0;

    /// <summary>커맨드 취소</summary>
    virtual void Undo() = 0;
};

// ─────────────────────────────────────────
// History
// ─────────────────────────────────────────

class IECommandHistory
{
public:
    /// <summary>커맨드를 실행 후 히스토리에 추가 (redo 스택 클리어)</summary>
    void Push(std::unique_ptr<IECommand> cmd)
    {
        cmd->Execute();
        m_undo.push_back(std::move(cmd));
        if (static_cast<int32_t>(m_undo.size()) > kMaxHistory)
            m_undo.erase(m_undo.begin());
        m_redo.clear();
    }

    void Undo()
    {
        if (m_undo.empty())
            return;
        m_undo.back()->Undo();
        m_redo.push_back(std::move(m_undo.back()));
        m_undo.pop_back();
    }

    void Redo()
    {
        if (m_redo.empty())
            return;
        m_redo.back()->Execute();
        m_undo.push_back(std::move(m_redo.back()));
        m_redo.pop_back();
    }

    /// <summary>Execute 없이 히스토리에만 추가 (이미 적용된 동작 기록용)</summary>
    void PushNoExec(std::unique_ptr<IECommand> cmd)
    {
        m_undo.push_back(std::move(cmd));
        if (static_cast<int32_t>(m_undo.size()) > kMaxHistory)
            m_undo.erase(m_undo.begin());
        m_redo.clear();
    }

    void Clear()
    {
        m_undo.clear();
        m_redo.clear();
    }

private:
    static constexpr int32_t kMaxHistory = 50;
    std::vector<std::unique_ptr<IECommand>> m_undo;
    std::vector<std::unique_ptr<IECommand>> m_redo;
};

// ─────────────────────────────────────────
// CmdCreateObject
// ─────────────────────────────────────────

class CmdCreateObject : public IECommand
{
public:
    CmdCreateObject(IEScene* scene, const std::string& type,
                    float wx, float wy, IEGameObject** selRef)
        : m_scene(scene), m_type(type), m_wx(wx), m_wy(wy), m_selRef(selRef)
    {}

    void Execute() override
    {
        IEGameObject* obj = nullptr;
        if (m_type == "Entity")
            obj = m_scene->AddObject<IEEntity>();
        else
            obj = m_scene->AddObject<IEStaticObject>();

        auto* t = obj->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            t->SetX(m_wx);
            t->SetY(m_wy);
        }

        if (m_name.empty())
            m_name = m_type + " " + std::to_string(m_scene->GetObjects().size());
        obj->SetName(m_name);

        m_ptr = obj;
        if (m_selRef != nullptr)
            *m_selRef = m_ptr;
    }

    void Undo() override
    {
        if (m_selRef != nullptr && *m_selRef == m_ptr)
            *m_selRef = nullptr;
        m_scene->RemoveObject(m_ptr);
        m_ptr = nullptr;
    }

private:
    IEScene*      m_scene  = nullptr;
    std::string   m_type;
    std::string   m_name;
    float         m_wx     = 0.0f;
    float         m_wy     = 0.0f;
    IEGameObject* m_ptr    = nullptr;
    IEGameObject**m_selRef = nullptr;
};

// ─────────────────────────────────────────
// CmdDeleteObject
// ─────────────────────────────────────────

class CmdDeleteObject : public IECommand
{
public:
    CmdDeleteObject(IEScene* scene, IEGameObject* obj, IEGameObject** selRef)
        : m_scene(scene), m_ptr(obj), m_selRef(selRef)
    {
        m_type   = obj->GetTypeName();
        m_name   = obj->GetName();
        m_active = obj->IsActive();
        auto* t  = obj->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            m_x = t->GetX();
            m_y = t->GetY();
            m_z = t->GetZ();
        }
    }

    void Execute() override
    {
        if (m_selRef != nullptr && *m_selRef == m_ptr)
            *m_selRef = nullptr;
        m_scene->RemoveObject(m_ptr);
        m_ptr = nullptr;
    }

    void Undo() override
    {
        IEGameObject* obj = nullptr;
        if (m_type == std::string("Entity"))
            obj = m_scene->AddObject<IEEntity>();
        else
            obj = m_scene->AddObject<IEStaticObject>();

        obj->SetActive(m_active);
        obj->SetName(m_name);
        auto* t = obj->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            t->SetX(m_x);
            t->SetY(m_y);
            t->SetZ(m_z);
        }

        m_ptr = obj;
        if (m_selRef != nullptr)
            *m_selRef = m_ptr;
    }

private:
    IEScene*      m_scene  = nullptr;
    IEGameObject* m_ptr    = nullptr;
    IEGameObject**m_selRef = nullptr;
    std::string   m_type;
    std::string   m_name;
    bool          m_active = true;
    float         m_x = 0.0f, m_y = 0.0f, m_z = 0.0f;
};

// ─────────────────────────────────────────
// CmdMoveObject
// ─────────────────────────────────────────

class CmdMoveObject : public IECommand
{
public:
    CmdMoveObject(IEGameObject* obj, float fromX, float fromY, float toX, float toY,
                  IEGameObject** selRef)
        : m_obj(obj), m_fromX(fromX), m_fromY(fromY),
          m_toX(toX), m_toY(toY), m_selRef(selRef)
    {}

    void Execute() override
    {
        auto* t = m_obj->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            t->SetX(m_toX);
            t->SetY(m_toY);
        }
        if (m_selRef != nullptr)
            *m_selRef = m_obj;
    }

    void Undo() override
    {
        auto* t = m_obj->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            t->SetX(m_fromX);
            t->SetY(m_fromY);
        }
        if (m_selRef != nullptr)
            *m_selRef = m_obj;
    }

private:
    IEGameObject* m_obj    = nullptr;
    IEGameObject**m_selRef = nullptr;
    float m_fromX = 0.0f, m_fromY = 0.0f;
    float m_toX   = 0.0f, m_toY   = 0.0f;
};
