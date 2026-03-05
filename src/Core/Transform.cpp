#include "pch.h"
#include "Transform.h"

void Transform::MoveFoward(float _distance)
{
	position.x += forward.x * _distance;
	position.y += forward.y * _distance;
	position.z += forward.z * _distance;
}

void Transform::MoveRight(float _distance)
{
	position.x += right.x * _distance;
	position.y += right.y * _distance;
	position.z += right.z * _distance;
}

void Transform::MoveUp(float _distance)
{
	position.x += up.x * _distance;
	position.y += up.y * _distance;
	position.z += up.z * _distance;
}

void Transform::AddYPR(float _yaw, float _pitch, float _roll)
{
	// 1. Charger les données actuelles
	XVector q = XMLoadFloat4(&quaternion);
	XVector xAxis = XMLoadFloat3(&right);
	XVector yAxis = XMLoadFloat3(&up);
	XVector zAxis = XMLoadFloat3(&forward);

	if (_yaw != 0.0f)   q = DirectX::XMQuaternionMultiply(q, DirectX::XMQuaternionRotationAxis(yAxis,_yaw));
	if (_pitch != 0.0f) q = DirectX::XMQuaternionMultiply(q, DirectX::XMQuaternionRotationAxis(xAxis,_pitch));
	if (_roll != 0.0f)  q = DirectX::XMQuaternionMultiply(q, DirectX::XMQuaternionRotationAxis(zAxis,_roll));

	q = DirectX::XMQuaternionNormalize(q);
	XMStoreFloat4(&quaternion, q);

	Matrix rot = DirectX::XMMatrixRotationQuaternion(q);
	XMStoreFloat4x4(&rotmatrix, rot);

	XMStoreFloat3(&right, rot.r[0]); // Ligne 0
	XMStoreFloat3(&up, rot.r[1]); // Ligne 1
	XMStoreFloat3(&forward, rot.r[2]); // Ligne 2
}

void Transform::SYPR(float _yaw, float _pitch, float _roll)
{
	forward = { 0.0f, 0.0f, 1.0f };
	right = { 1.0f, 0.0f, 0.0f };
	up = { 0.0f, 1.0f, 0.0f };
	quaternion = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMStoreFloat4x4(&rotmatrix, DirectX::XMMatrixIdentity());

	AddYPR(_yaw, _pitch, _roll);
}

Matrix Transform::BuildMatrix() const
{

	// Charger les composants en types SIMD (XMMATRIX / XMVECTOR)
	Matrix mScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	Matrix mRot = XMLoadFloat4x4(&rotmatrix);
	Matrix mTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	Matrix world = XMMatrixMultiply(mScale, mRot);

	world = XMMatrixMultiply(world, mTrans);

	return world;
}

void Transform::SPosition(float _x, float _y, float _z)
{
	position.x = _x;
	position.y = _y;
	position.z = _z;
}

void Transform::SScale(float _x, float _y, float _z)
{
	scale.x = _x;
	scale.y = _y;
	scale.z = _z;
}