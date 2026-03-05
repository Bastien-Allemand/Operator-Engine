#pragma once
enum ProjectileType
{
	ENEMY_PROJECTILE,
	PLAYER_PROJECTILE
};
struct TypeProjectileComponent
{
	ProjectileType type = PLAYER_PROJECTILE;
};

