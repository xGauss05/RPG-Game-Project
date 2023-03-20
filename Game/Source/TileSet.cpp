#include "TileSet.h"
#include "App.h"

#include "TextureManager.h"

#include "Log.h"

TileSet::TileSet(const pugi::xml_node& node, const std::string& directory) :
	firstGid(node.attribute("firstgid").as_uint()),
	source(node.attribute("source").as_string())
{
	pugi::xml_document tileSetFile;
	if (pugi::xml_parse_result parseResult = tileSetFile.load_file((directory + source).c_str());
		!parseResult)
	{
		LOG("TileSet file for %s parsing error: %s", source, parseResult.description());
		return;
	}
	
	pugi::xml_node currentNode = tileSetFile.child("tileset");

	name = currentNode.attribute("name").as_string();
	tileSize = { currentNode.attribute("tilewidth").as_uint(), currentNode.attribute("tileheight").as_uint() };
	tileCount = currentNode.attribute("tilecount").as_uint();
	columns = currentNode.attribute("columns").as_uint();

	if (auto gridNode = currentNode.child("grid"); gridNode.empty())
	{
		// Default values if grid child doesn't exist
		orientation = MapOrientation::ORTHOGONAL;
		size = tileSize;
	}
	else
	{
		// TODO read XML attribute
		orientation = MapOrientation::ORTHOGONAL;
		size = { gridNode.attribute("width").as_uint(), gridNode.attribute("height").as_uint() };
	}

	auto imgNode = currentNode.child("image");
	imageSource = imgNode.attribute("source").as_string();
	sourceSize = { imgNode.attribute("width").as_uint(), imgNode.attribute("height").as_uint() };
	textureID = app->tex->Load((directory + imageSource).c_str());

	for (auto const& tile : currentNode.children("tile"))
	{
		tileWalkability.push_back(
			tile
			.child("properties")
			.child("property")
			.attribute("value")
			.as_bool()
		);
	}
}

void TileSet::Unload() const
{
	app->tex->Unload(textureID);
}

uint TileSet::ContainsGid(uint gid) const
{
	return gid >= firstGid && gid < firstGid + tileCount;
}

SDL_Rect TileSet::GetTileRect(uint gid) const
{
	SDL_Rect rect = { 0 };
	uint relativeIndex = gid - firstGid;

	rect.w = tileSize.x;
	rect.h = tileSize.y;
	rect.x = margin + (tileSize.x + spacing) * (relativeIndex % columns);
	rect.y = margin + (tileSize.y + spacing) * (relativeIndex / columns);

	return rect;
}

int TileSet::GetTextureID() const
{
	return textureID;
}

bool TileSet::IsWalkable(uint gid) const
{
	return tileWalkability[gid - firstGid];
}