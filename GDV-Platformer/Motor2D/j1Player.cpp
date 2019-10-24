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

Animation* j1Player::LoadAnim(const char* path, const char* animation)
{
	Animation* anim = new Animation();

	bool bAnim;

	pugi::xml_document animFile;
	pugi::xml_parse_result res = animFile.load_file(path);

	if (res == NULL)
	{
		LOG("Cannot load animation file");
	}

	pugi::xml_node objectGroup;
	for (objectGroup = animFile.child("map").child("objectgroup"); objectGroup; objectGroup = objectGroup.next_sibling("objectgroup"))
	{
		p2SString name = objectGroup.attribute("name").as_string();
		if (name == animation)
		{
			bAnim = true;
			
			uint x;
			uint y;
			uint h;
			uint w;

			for (pugi::xml_node sprite = objectGroup.child("object"); sprite; sprite = sprite.next_sibling("object"))
			{
				x = sprite.attribute("x").as_uint();
				y = sprite.attribute("y").as_uint();
				w = sprite.attribute("w").as_uint();
				h = sprite.attribute("h").as_uint();

				anim->PushBack({ x,y,w,h });
			}
		}
	}

	if (bAnim = true)
		return anim;
	else
		return nullptr;
}