#include "pch.h"
#include "ShootEnemySystem.h"
#include "Component/TransformComponent.h"
#include "EnemyIAComponent.h"
#include "ShootComponent.h"
#include "ProjectilePrefab.h"
#include "HealthComponent.h"
#include "Geometry.h"
#include "IRenderer.h"
#include "Mesh.h"
#include "BulletComponent.h"
#include "TypeProjectileComponent.h"

#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"

static inline float Vec3Length(const Vector3f& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline Vector3f Vec3Sub(const Vector3f& a, const Vector3f& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

static inline Vector3f Vec3Normalize(const Vector3f& v)
{
    float len = Vec3Length(v);
    if (len <= 1e-6f) return { 0.0f, 0.0f, 0.0f };
    return { v.x / len, v.y / len, v.z / len };
}

ShootEnemySystem::ShootEnemySystem()
{
    m_hashCode = typeid(EnemyIAComponent).hash_code();
}

void ShootEnemySystem::Init(GameManager* _gameManager)
{
    IRenderer* irenderer = _gameManager->GRenderEngine();
    m_mesh = new Mesh();
    
    m_projectileTexture = irenderer->LoadTexture(L"../../res/Render/bullet.dds");
    Geometry geometry;
    geometry.BuildSphere(20, 20);
    irenderer->InitMesh(*m_mesh, geometry);
}

void ShootEnemySystem::Update(GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	for (uint32 entityId : m_entities)
	{
		TransformComponent* transform = componentManager->GComponent<TransformComponent>(entityId);
		EnemyIAComponent* enemyIA = componentManager->GComponent<EnemyIAComponent>(entityId);
		ShootComponent* shootComp = componentManager->GComponent<ShootComponent>(entityId);
        HealthComponent* heal = componentManager->GComponent<HealthComponent>(entityId);
		if (enemyIA == nullptr || shootComp == nullptr)	continue;
		if (shootComp->canShoot && heal->health>0)
		{
            shootComp->canShoot = false;

            ProjectileType type = ProjectileType::ENEMY_PROJECTILE;
            ProjectilePrefab newProjectile(m_mesh, m_projectileTexture,type ) ;
            // position the projectile at the enemy position
            newProjectile.transform->localTransform.position = transform->localTransform.position; // projectile will exist for 5 seconds before being destroyed
            // compute direction towards target point (player) if available
                Vector3f target = enemyIA->playerPos;
                Vector3f dir = Vec3Normalize(Vec3Sub(target, newProjectile.transform->localTransform.position));
                // if valid direction, use it, otherwise fallback to enemy forward
                if (Vec3Length(dir) > 1e-6f)
                {
                    newProjectile.transform->localTransform.forward = dir;
                }
                else
                {
                    newProjectile.transform->localTransform.forward = transform->localTransform.forward;
                }

            // give the projectile a little spawn offset forward so it doesn't immediately collide with the shooter
            newProjectile.transform->localTransform.MoveFoward(2.0f);
		}
	}
}
