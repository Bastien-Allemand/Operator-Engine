#pragma once
struct PhysicComponent
{
public:
	bool isKinetic = true;
	Vector3f mainForce = { 0.0f, 0.0f, 0.0f }; //this is for gravity and other forces that are applied to the object constantly
	Vector<Vector4f> targets; //this is for Positions that the object need to go to x y z speed
};

