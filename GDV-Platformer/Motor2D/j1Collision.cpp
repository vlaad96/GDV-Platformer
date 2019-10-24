#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Player.h"
#include "j1Map.h"

j1Collision::j1Collision()
{
	name.create("collision");

	matrix[COLLIDER_FLOOR][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_OBSTACLE] = false;

	matrix[COLLIDER_OBSTACLE][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_OBSTACLE][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_OBSTACLE][COLLIDER_OBSTACLE] = false;

	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_OBSTACLE] = true;

}

j1Collision::~j1Collision()
{

}

bool j1Collision::PreUpdate()
{

}

bool j1Collision::Update(float dt)
{

}

bool j1Collision::PostUpdate(float dt)
{

}

bool j1Collision::CleanUp()
{

}

void j1Collision::DebugDraw()
{

}

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr)
{

}

bool j1Collision::CheckCollision(const SDL_Rect& r)const
{

}