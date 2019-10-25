#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

struct Properties
{
	bool Draw;
};

struct MapLayer
{
	p2SString name;
	
	uint width;
	uint height;

	uint* data;
	Properties* properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}

};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id)const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

struct ImageLayer //For parallaax
{
	SDL_Rect GetImageRect()const;

	p2SString name;
	uint width;
	uint height;
	SDL_Texture* texture;
	
};
enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	uint					width;
	uint					height;
	uint					tile_width;
	uint					tile_height;
	SDL_Color				background_color;

	MapTypes				type;

	p2List<TileSet*>		tilesets;
	p2List<MapLayer*>		layers;
	p2List<ImageLayer*>		images;

	fPoint					playerStartingPos;
	fPoint					playerEndPoint;
	float					parallaxSpeed;
	int						cameraVertLimit;
	fPoint					backgroundOffset;

};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y)const;
	iPoint WorldToMap(int x, int y)const;

	int level = 0;

	TileSet* GetTilesetFromTileId(int id)const;


private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadBackgroundImg(pugi::xml_node& node, ImageLayer* imgLayer);
	bool LoadMapProperties(pugi::xml_node &node);



public:

	MapData				data;
	pugi::xml_document	map_file;
	p2SString			folder;

	int					offset = 0;

private:

	
	bool				map_loaded;
};

#endif // __j1MAP_H__