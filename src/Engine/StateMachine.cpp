#include "pch.h"
#include "../Engine/StateMachine.h"

StateMachine::StateMachine()
{
}

StateMachine::~StateMachine()
{
    for (auto& pair : m_states)
    {
        if (pair.second)
        {
            delete pair.second;
            pair.second = nullptr;
        }
    }
    m_states.clear();
}

void StateMachine::RegisterState(const std::string& _name, IState* _state)
{
    if (m_states.find(_name) != m_states.end())
    {
        delete m_states[_name];
    }
    m_states[_name] = _state;
}

void StateMachine::ChangeState(const std::string& _name, uint32 _entityId)
{
    auto it = m_states.find(_name);
    if (it == m_states.end()) return;

    if (m_currentState != nullptr)
    {
        m_currentState->OnExit(_entityId);
    }

    m_currentState = it->second;
    m_currentStateName = _name;

    if (m_currentState != nullptr)
    {
        m_currentState->OnEnter(_entityId);
    }
}

void StateMachine::Update(uint32 _entityId)
{
    if (m_currentState != nullptr)
    {
        m_currentState->OnUpdate(_entityId);
    }
}