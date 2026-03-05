#pragma once
#include <unordered_map>
#include <string>
#include "Define.h" 

class IState
{
public:
    virtual ~IState() = default;
    virtual void OnEnter(uint32 _entityId) = 0;
    virtual void OnUpdate(uint32 _entityId) = 0;
    virtual void OnExit(uint32 _entityId) = 0;
};

class StateMachine
{
public:
    StateMachine();
    ~StateMachine();

    void RegisterState(const std::string& _name, IState* _state);
    void ChangeState(const std::string& _name, uint32 _entityId);
    void Update(uint32 _entityId);

    std::string GetCurrentStateName() const { return m_currentStateName; }

private:
    std::unordered_map<std::string, IState*> m_states;
    IState* m_currentState = nullptr;
    std::string m_currentStateName = "";
};