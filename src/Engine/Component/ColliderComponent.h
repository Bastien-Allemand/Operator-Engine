#pragma once

struct TransformComponent;

struct ColliderComponent 
{
public:
	bool isTrigger = false;
	int flag = -1;
	Vector<uint32> collideWith;
	Vector<Vector3f> normals; //points towards the opposite of the other object

	enum type
	{
		DEFAULT,
		BOX,
		SPHERE
	};
	type colliderType = DEFAULT;
	DirectX::BoundingBox* boundBox = nullptr;
	DirectX::BoundingSphere* boundSphere = nullptr;

	void Updatebound(const TransformComponent& _transform);
	bool Intersect(ColliderComponent _other);

	Vector3f GetPenetrationVector(ColliderComponent* _other);
	Vector3f GetCollisionNormal(ColliderComponent* _other);

private:
	Vector3f BoxBoxCollisionNormal(ColliderComponent* _other); //BB
	Vector3f SphereSphereCollisionNormal(ColliderComponent* _other); //SS
	Vector3f BoxSphereCollisionNormal(ColliderComponent* _other); //BS
	Vector3f ResolveSphereSphereCollision(ColliderComponent* _other);
	Vector3f ResolveSphereBoxCollision(const DirectX::BoundingSphere& _sphere, const DirectX::BoundingBox& _box);
	Vector3f ResolveBoxBoxCollision(ColliderComponent* _other);
};

