#pragma once
class GameManager;

struct Condition
{
	enum type
	{
		DOWN,
		PRESSED,
		RELEASED,
		UP
	};
	type actionType;
	uint32 key;
	uint32 actionIndex;

};
typedef void (*Action)(uint32 _entity,GameManager* _gameManager);

/// <summary>
/// Interface for InputComponent
/// <use> Create a child of IInputComponent
/// 	- Define your own actions and conditions
///		- Actions Must be void functions with 1 param the entity ID & and must not be defined in a class
/// </summary>
struct IInputComponent
{
public:
	Vector<Condition> conditions;
	Vector<Action> actions;
};