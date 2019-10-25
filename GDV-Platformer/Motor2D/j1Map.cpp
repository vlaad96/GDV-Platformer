#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	//if (map_loaded == false)
		LOG("Cannot load the map");
		//return;

	//Background draw
	for (int i = 0; i < data.images.count(); ++i)
	{
		App->render->Blit(data.images[i]->texture, data.backgroundOffset.x*data.parallaxSpeed, data.backgroundOffset.y, &data.images[i]->GetImageRect());
	}

	//Map draw
	MapLayer* layer;// = this->data.layers.start->data;

	for (int i = 0; i < data.tilesets.count(); ++i)
	{
		layer = data.layers[i];//data.layers[i] es un nullptr
		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tile_id = layer->Get(x, y);
				if (tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);
					if (tileset != nullptr)
					{
						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToWorld(x, y);

						App->render->Blit(tileset->texture, pos.x, pos.y, &r);
					}
				}
			}
		}
	}
}

TileSet* j1Map::GetTilesetFromTileId(int id)const
{
	for (int i = 0; i < data.tilesets.count() - 1; ++i)
	{
		if (data.tilesets[i + 1]->firstgid > id)
			return data.tilesets[i];
	}

	return data.tilesets.end->data;
}

iPoint j1Map::MapToWorld(int x, int y)const
{
	iPoint ret;

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}

	else
	{
		LOG("Unkown map type");
		ret.x = x;
		ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y)const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}

	else
	{
		LOG("Unkown map type");
		ret.x = x;
		ret.y = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id)const
{
	int relativeId = id - firstgid;

	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relativeId % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relativeId / num_tiles_height));

	return rect;
}

SDL_Rect ImageLayer::GetImageRect()const
{
	SDL_Rect rect;

	rect.w = width;
	rect.h = height;

	rect.x = 0;
	rect.y = 0;

	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* itemL;
	itemL = data.layers.start;

	while (itemL != NULL)
	{
		RELEASE(itemL->data);
		itemL = itemL->next;
	}
	data.layers.clear();

	// Remove all image layers
	p2List_item<ImageLayer*>* itemIL;
	itemIL = data.images.start;

	while (itemIL != NULL)
	{
		RELEASE(itemIL->data);
		itemIL = itemIL->next;
	}
	data.images.clear();


	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node imgLayer;
	for (imgLayer = map_file.child("map").child("imagelayer"); imgLayer && ret; imgLayer = imgLayer.next_sibling("imagelayer"))
	{
		ImageLayer* imageLay = new ImageLayer();

		ret = LoadBackgroundImg(imgLayer, imageLay);

		if (ret == true)
			data.images.add(imageLay);
	}

	// Load image layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* mapLayer = new MapLayer();

		ret = LoadLayer(layer, mapLayer);

		if (ret == true)
			data.layers.add(mapLayer);
	}


	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		
		p2List_item<MapLayer*>* item_layer = data.layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}
	//Player starting pos
	//App->player->pos.x = data.playerStartingPos.x;
	//App->player->pos.y = data.playerStartingPos.y;

	//Load Properties ----------------------------
	LoadMapProperties(map_file);

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_uint();
	layer->height = node.attribute("height").as_uint();
	pugi::xml_node layerData = node.child("data");

	if (layerData == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for (pugi::xml_node tile = layerData.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;

}

bool j1Map::LoadBackgroundImg(pugi::xml_node& node, ImageLayer* imagelayer)
{
	bool ret = true;

	imagelayer->name = node.attribute("name").as_string();
	imagelayer->width = node.child("image").attribute("width").as_int();
	imagelayer->height = node.child("image").attribute("height").as_int();
	imagelayer->texture = App->tex->Load(PATH(folder.GetString(), node.child("image").attribute("source").as_string()));
	data.backgroundOffset.x = node.attribute("offsetx").as_float();
	data.backgroundOffset.y = node.attribute("offsety").as_float();
	return ret;
}

bool j1Map::LoadMapProperties(pugi::xml_node& node)
{
	for (pugi::xml_node iterator = node.child("map").child("properties").child("property"); iterator != nullptr; iterator = iterator.next_sibling())
	{
		p2SString name = iterator.attribute("name").as_string();

		if (name == "Parallax_Speed")
		{
			data.parallaxSpeed = iterator.attribute("value").as_float();
		}

		if (name == "Starting_Pos_X")
		{
			data.playerStartingPos.x = iterator.attribute("value").as_float();
		}

		if (name == "Starting_Pos_Y")
		{
			data.playerStartingPos.y = iterator.attribute("value").as_float();
		}
	}

	return true;
}