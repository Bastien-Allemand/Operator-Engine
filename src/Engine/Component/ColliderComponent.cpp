#include "pch.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"

void ColliderComponent::Updatebound(const TransformComponent& _transform)
{
	if (boundBox != nullptr)
		boundBox->Center = _transform.localTransform.position;
	if (boundSphere != nullptr)
		boundSphere->Center = _transform.localTransform.position;
}
bool ColliderComponent::Intersect(ColliderComponent _other)
{
	bool intersect = false;
	switch (colliderType)
	{
	case ColliderComponent::DEFAULT:
		break;
	case ColliderComponent::BOX:
		if (_other.colliderType == DEFAULT)
			break;

		if (_other.colliderType == BOX)
		{
			intersect = boundBox->Intersects(*_other.boundBox);
		}
		else
		{
			intersect = boundBox->Intersects(*_other.boundSphere);
		}
		break;
	case ColliderComponent::SPHERE:
		if (_other.colliderType == DEFAULT)
			break;

		if (_other.colliderType == SPHERE)
		{
			intersect = boundSphere->Intersects(*_other.boundSphere); 
		}
		else
		{
			intersect = boundSphere->Intersects(*_other.boundBox);
		}
		break;
	default:

		break;
	}

	return intersect;
}

Vector3f ColliderComponent::GetPenetrationVector(ColliderComponent* _other)
{
	switch (colliderType)
	{
	case BOX:
		if (_other->colliderType == BOX)
			return ResolveBoxBoxCollision(_other);
		else
			return ResolveSphereBoxCollision(*_other->boundSphere, *boundBox);
	case SPHERE:
		if (_other->colliderType == BOX)
			return ResolveSphereBoxCollision(*boundSphere, *_other->boundBox);
		else
			return ResolveSphereSphereCollision(_other);
	default:
		return Vector3f(0, 0, 0);
	}
}

Vector3f ColliderComponent::ResolveSphereSphereCollision(ColliderComponent* _other)
{
	// Both must have valid spheres
	if (!boundSphere || !_other->boundSphere)
		return Vector3f(0, 0, 0);

	const DirectX::BoundingSphere& A = *boundSphere;
	const DirectX::BoundingSphere& B = *_other->boundSphere;

	// Vector from other sphere to this sphere
	Vector3f delta = A.Center - B.Center;
	float distSqr = Dot(delta, delta);
	float radiusSum = A.Radius + B.Radius;

	if (distSqr >= radiusSum * radiusSum)
	{
		// No collision
		return Vector3f(0, 0, 0);
	}

	float dist = sqrtf(distSqr);
	float penetration = radiusSum - dist;

	// Avoid division by zero if centers coincide
	if (dist == 0.f)
	{
		// Arbitrary push direction (up)
		delta = Vector3f(0, 1, 0);
	}

	Vector3f normal = Normalize(delta);

	// Return exact displacement to move this sphere out of other
	return normal * penetration;
}
Vector3f ColliderComponent::ResolveSphereBoxCollision(const DirectX::BoundingSphere& _sphere,const DirectX::BoundingBox& _box)
{
	Vector3f closest;
	closest.x = (std::max)(_box.Center.x - _box.Extents.x, (std::min)(_sphere.Center.x, _box.Center.x + _box.Extents.x));
	closest.y = (std::max)(_box.Center.y - _box.Extents.y, (std::min)(_sphere.Center.y, _box.Center.y + _box.Extents.y));
	closest.z = (std::max)(_box.Center.z - _box.Extents.z, (std::min)(_sphere.Center.z, _box.Center.z + _box.Extents.z));

	Vector3f delta = _sphere.Center - closest;
	float distSqr = Dot(delta, delta);

	if (distSqr >= _sphere.Radius * _sphere.Radius)
		return Vector3f(0, 0, 0); // no collision

	float dist = sqrtf(distSqr);
	float penetration = _sphere.Radius - dist;
	if (dist == 0.f) // sphere center exactly at closest point
		delta = Vector3f(0, 1, 0); // push up

	Vector3f normal = Normalize(delta);
	return normal * penetration; // move sphere out along this vector
}
Vector3f ColliderComponent::ResolveBoxBoxCollision(ColliderComponent* _other)
{

	Vector3f d = boundBox->Center - _other->boundBox->Center;

	float px = (_other->boundBox->Extents.x + boundBox->Extents.x) - std::abs(d.x);
	if (px <= 0) return Vector3f();		  
	float py = (_other->boundBox->Extents.y + boundBox->Extents.y) - std::abs(d.y);
	if (py <= 0) return Vector3f();		  
	float pz = (_other->boundBox->Extents.z + boundBox->Extents.z) - std::abs(d.z);
	if (pz <= 0) return Vector3f();

	// find min penetration
	float minPen = px;
	Vector3f axis = (d.x < 0.f) ? Vector3f(-1, 0, 0) : Vector3f(1, 0, 0);

	if (py < minPen) { minPen = py; axis = (d.y < 0.f) ? Vector3f(0, -1, 0) : Vector3f(0, 1, 0); }
	if (pz < minPen) { minPen = pz; axis = (d.z < 0.f) ? Vector3f(0, 0, -1) : Vector3f(0, 0, 1); }

	return axis * minPen;
}


Vector3f ColliderComponent::GetCollisionNormal(ColliderComponent* _other)
{
	Vector3f normal = VEC3_ZERO;

	switch (colliderType)
	{
	case ColliderComponent::BOX:
		if (_other->colliderType == BOX)
		{
			normal = BoxBoxCollisionNormal(_other);
		}
		else
		{
			normal = BoxSphereCollisionNormal(_other);
		}

		break;
	case ColliderComponent::SPHERE:
		if (_other->colliderType == BOX)
			normal = BoxSphereCollisionNormal(_other);
		else
			normal = SphereSphereCollisionNormal(_other);
		break;
	default:
		break;
	}

	return normal;
}

Vector3f ColliderComponent::BoxBoxCollisionNormal(ColliderComponent* _other)
{
	const auto& A = *boundBox;
	const auto& B = *_other->boundBox;

	Vector3f d = Vector3f(
		B.Center.x - A.Center.x,
		B.Center.y - A.Center.y,
		B.Center.z - A.Center.z
	);

	float px = (A.Extents.x + B.Extents.x) - abs(d.x);
	if (px <= 0) return Vector3f();

	float py = (A.Extents.y + B.Extents.y) - abs(d.y);
	if (py <= 0) return Vector3f();

	float pz = (A.Extents.z + B.Extents.z) - abs(d.z);
	if (pz <= 0) return Vector3f();

	float minPen = px;
	Vector3f normal = { (d.x < 0) ? -1.f : 1.f, 0, 0 };

	if (py < minPen)
	{
		minPen = py;
		normal = { 0, (d.y < 0) ? -1.f : 1.f, 0 };
	}

	if (pz < minPen)
	{
		normal = { 0, 0, (d.z < 0) ? -1.f : 1.f };
	}

	return normal;
}

Vector3f ColliderComponent::SphereSphereCollisionNormal(ColliderComponent* _other)
{
	const auto& A = *boundSphere;
	const auto& B = *_other->boundSphere;

	Vector3f d = Vector3f(
		B.Center.x - A.Center.x,
		B.Center.y - A.Center.y,
		B.Center.z - A.Center.z
	);

	float r = A.Radius + B.Radius;
	
	if (Dot(d, d) >= r * r)
		return Vector3f();

	return Normalize(d);
}

Vector3f ColliderComponent::BoxSphereCollisionNormal(ColliderComponent* _other)
{
	DirectX::BoundingBox box;
	DirectX::BoundingSphere sphere;
	if (colliderType == BOX)
	{
		box = *boundBox;
		sphere = *_other->boundSphere;
	}
	else
	{
		box = *_other->boundBox;
		sphere = *boundSphere;
	}

	Vector3f closest;

	closest.x = (std::max)(box.Center.x - box.Extents.x,
		(std::min)(sphere.Center.x, box.Center.x + box.Extents.x));

	closest.y = (std::max)(box.Center.y - box.Extents.y,
		(std::min)(sphere.Center.y, box.Center.y + box.Extents.y));

	closest.z = (std::max)(box.Center.z - box.Extents.z,
		(std::min)(sphere.Center.z, box.Center.z + box.Extents.z));

	Vector3f d;

	if (colliderType == BOX)
	{
		// A = box
		d = Vector3f(
			sphere.Center.x - closest.x,
			sphere.Center.y - closest.y,
			sphere.Center.z - closest.z
		);
	}
	else
	{
		// A = sphere
		d = Vector3f(
			closest.x - sphere.Center.x,
			closest.y - sphere.Center.y,
			closest.z - sphere.Center.z
		);
	}

	if (Dot(d, d) > sphere.Radius * sphere.Radius)
		return Vector3f();

	return Normalize(d);
}