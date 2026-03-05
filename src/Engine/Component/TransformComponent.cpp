#include "pch.h"
#include "TransformComponent.h"



Transform TransformComponent::BuildworldTransfrom() const
{
	Transform transform = localTransform;
	if (parent != nullptr)
	{
		Transform parentTransform = parent->BuildworldTransfrom();
		transform.position += parentTransform.position;
		transform.scale = transform.scale * parentTransform.scale;
		DirectX::XMStoreFloat4(&transform.quaternion,DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&transform.quaternion), DirectX::XMLoadFloat4(&parentTransform.quaternion)));
	}
	return transform;
}

Matrix TransformComponent::BuildWorldMatrix() const
{
	Matrix matrix = localTransform.BuildMatrix();
	if (parent != nullptr)
	{
		matrix *= parent->BuildWorldMatrix();
	}
	return matrix;
}