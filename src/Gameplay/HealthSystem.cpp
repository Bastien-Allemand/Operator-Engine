#include "pch.h"
#include "HealthSystem.h"

#include "HealthComponent.h"
#include "Component/ColliderComponent.h"
#include "Component/MeshComponent.h"
#include "Component/TextComponent.h"
#include "Component/UITransformComponent.h"
#include "BulletComponent.h"
#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "ScoreComponent.h"
#include "MoveComponent.h"
#include "TypeProjectileComponent.h"
#include "Tag.h"

HealthSystem::HealthSystem()
{
	m_hashCode = typeid(HealthComponent).hash_code();
}

void HealthSystem::Init(GameManager* _gameManager)
{
}

void HealthSystem::Update(GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	for (uint32 entityId : m_entities)
	{
		HealthComponent* health = componentManager->GComponent<HealthComponent>(entityId);
		ColliderComponent* collider = componentManager->GComponent<ColliderComponent>(entityId);
		if (collider == nullptr || health == nullptr) continue;

		if (collider->flag == PLAYER )
		{
			m_entityPlayerID = entityId;
		}
		for (uint32 collided : collider->collideWith)
		{
			ColliderComponent* collider2 = componentManager->GComponent<ColliderComponent>(collided);
			TypeProjectileComponent* typeProjectile = componentManager->GComponent<TypeProjectileComponent>(collided);
			if (collider2 == nullptr) continue;
			if (collider2->flag == PROJECTILE)
			{
				if ((typeProjectile->type == ENEMY_PROJECTILE && collider->flag == ENEMY) || (typeProjectile->type == PLAYER_PROJECTILE && collider->flag == PLAYER)) continue;

				BulletComponent* bullet = componentManager->GComponent<BulletComponent>(collided);
				if (bullet == nullptr) continue;

				health->health -= bullet->damage;

				if (collider->flag == PLAYER) m_updateText = true;
				
				_gameManager->DestroyEntity(collided);
			}
		}
		if (health->health <= 0 && collider->isTrigger == true)
		{
			if (collider->flag == ENEMY && m_entityPlayerID != -1)
			{
				ScoreComponent* score = componentManager->GComponent<ScoreComponent>(m_entityPlayerID);
				score->score += 100;
				_Log("Score: " + std::to_string(score->score));
				m_updateText = true;
			}
			if (collider->flag == PLAYER)
			{
				_Log("Player is dead");
				_gameManager->SSpeedTime(0);
				MoveComponent* moveComp = componentManager->GComponent<MoveComponent>(entityId);
				moveComp->isActive = false;
				moveComp->lockCursor = false;
				m_playerDead = true;
				m_updateText = true;
			}
			MeshComponent* mesh = componentManager->GComponent<MeshComponent>(entityId);
			collider->isTrigger = false;
			mesh->visible = false;
			//_gameManager->DestroyEntity(entityId);
		}

		if (m_updateText == true && m_entityPlayerID != -1)
		{

			TextComponent* text = componentManager->GComponent<TextComponent>(m_entityPlayerID);
			HealthComponent* healthPlayer = componentManager->GComponent<HealthComponent>(m_entityPlayerID);
			ScoreComponent* score = componentManager->GComponent<ScoreComponent>(m_entityPlayerID);
			UITransformComponent* uiTransform = componentManager->GComponent<UITransformComponent>(m_entityPlayerID);
			if (m_playerDead)
			{
				text->text = L"You are dead, your score " + std::to_wstring(score->score);
				uiTransform->anchoredPosition = Vector2f(-200, 0);
				return;
			}
			text->text = L"Score: " + std::to_wstring(score->score) + L" / Life: " + std::to_wstring(healthPlayer->health) + L"/" + std::to_wstring(healthPlayer->maxHealth);
			m_updateText = false;
		}


	}
}
