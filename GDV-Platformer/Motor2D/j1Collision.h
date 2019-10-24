#ifndef _J1COLLISION_H_
#define _J1COLLISION_H_

#define MAX_COLLIDERS 1000

#include "SDL/include/SDL.h"
#include "SDL_image/include/SDL_image.h"
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )

enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_FLOOR,
	COLLIDER_PLAYER,
	COLLIDER_OBSTACLE,
	COLLIDER_MAX,

};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;

	Collider(SDL_Rect rect, COLLIDER_TYPE type) :
		rect(rect),
		type(type)
	{}

	void setPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	void setSize(int w, int h)
	{
		rect.w = w;
		rect.h = h;
	}

};

class j1Collision : public j1Module
{
public:

	j1Collision();
	~j1Collision();

	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	bool CheckCollision(const SDL_Rect& r)const;

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);
	void DebugDraw();

private:

	Collider * colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	bool debug = false;

};
#endif // !_J1COLLISION_H_

