#include "j1App.h"
#include"j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include"j1Map.h"
#include "p2Log.h"
#include "j1Audio.h"
#include "j1Scene.h"

j1Player::j1Player()
{
	name.create("player");
}

j1Player::~j1Player()
{
	App->tex->UnLoad(graphics);
}

bool j1Player::Awake(pugi::xml_node& config)
{
	return true;
}

bool j1Player::Start()
{
	LOG("Loading player textures");
	bool ret = true;

	graphics = App->tex->Load("textures/spritesheet.png");
}

bool j1Player::PostUpdate()
{
	//Here goes the player control and drawing
}

//Save game state of the player
bool j1Player::Save(pugi::xml_node& data)const
{
	pugi::xml_node player = data.append_child("playerPosition");

	return true;
}

//Load game state of the player
bool j1Player::Load(pugi::xml_node& data)
{
	return true;
}