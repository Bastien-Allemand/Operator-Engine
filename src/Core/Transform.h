#pragma once
class Transform
{
public:

	Transform() = default;

	void MoveFoward(float _distance);
	void MoveRight(float _distance);
	void MoveUp(float _distance);

	void AddYPR(float _yaw, float _pitch, float _roll);
	void SYPR(float _yaw, float _pitch, float _roll);

	Matrix BuildMatrix() const;

	void SPosition(float _x, float _y, float _z);
	void SScale(float _x, float _y, float _z);

	Vector3f position = {0,0,0};

	Vector3f scale = {1,1,1};

	Vector3f up = VEC3_UP;
	Vector3f right = VEC3_RIGHT;
	Vector3f forward = VEC3_FOWARD;
	Vector4f quaternion = QUAT_IDENTITY;
		
	Matrix4x4f rotmatrix = MATRIX_IDENTITY;
};

