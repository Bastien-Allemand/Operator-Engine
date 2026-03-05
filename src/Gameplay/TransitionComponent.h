#pragma once
enum State
{
    VERYLOW,
    LOW,
    NORMAL,
    BRIGHT,
    COUNT// toujours en dernier pour connaître le nombre d'états
};

struct TransitionComponent
{


    void Next()
    {
        uint8_t v = static_cast<uint8_t>(state);
        v = (v + 1) % static_cast<uint8_t>(State::COUNT);
        state = static_cast<State>(v);
        if (state == State::COUNT)
        {
            state = State::VERYLOW; // passer de None à FadeIn
		}
    }



    State state = State::NORMAL;
};

