#ifndef _J1PLAYER_H_
#define _J1PLAYER_H_

#include "j1Module.h"
#include "p2Animation.h"
#include "j1Collision.h"
#include "p2Point.h"

struct SDL_Texture;

class j1Player : public j1Module
{
public:

	j1Player();
	~j1Player();

	bool Awake(pugi::xml_node& config);

	bool Start();
	bool PostUpdate();

	//Save and Load
	bool Save(pugi::xml_node&)const;
	bool Load(pugi::xml_node&);

public:

	Collider * collider;

public:

	SDL_Texture *	graphics = nullptr;
	Animation*		idleRight;
	Animation*		idleLeft;
	Animation*		walkRight;
	Animation*		walkLeft;
	Animation*		jumpRight;
	Animation*		jumpLeft;
	Animation*		die;
};

#endif // !_J1PLAYER_H_
