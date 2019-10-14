#ifndef _J1COLLISION_H_
#define _J1COLLISION_H_

#define MAX_COLLIDERS 1000

#include "SDL/include/SDL.h"
#include "SDL_image/include/SDL_image.h"
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )

enum COLLIDER_TYPE
{
	COLLIDER_NONE,
	COLLIDER_WALL,
	COLLIDER_PLAYER,
	COLLIDER_DEAD,
	MAX_COLLIDER

};

struct Collider
{

};

class j1Collision : public j1Module
{
public:

private:

};
#endif // !_J1COLLISION_H_

