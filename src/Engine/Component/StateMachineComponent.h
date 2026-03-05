#pragma once
#include "../Engine/StateMachine.h"

struct StateMachineComponent
{
public:
    StateMachine* machine = nullptr;

    void Init()
    {
        if (machine == nullptr) machine = new StateMachine();
    }
};