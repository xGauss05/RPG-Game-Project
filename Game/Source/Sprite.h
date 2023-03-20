#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "App.h"

#include "TextureManager.h"

#include "Point.h"

#include "PugiXml/src/pugixml.hpp"

class Sprite
{
public:
	int GetGid() const { return gid; };
	int GetTextureID() const { return textureID; };
	iPoint GetTextureIndex() const { return textureIndex; };

protected:
	void Initialize(std::string const &texturePath, int index)
	{
		textureID = app->tex->GetTextureID(texturePath);
		textureIndex = (index < 4)
			? iPoint(index * 3, 0)
			: iPoint((index - 4) * 3, 4);
	}
	void Initialize(pugi::xml_node const& node)
	{
		textureID = node.attribute("id").as_int();
		gid = node.attribute("gid").as_uint();
	};

private:
	int textureID = -1;
	int gid = -1;
	iPoint textureIndex = { 0, 0 };
};

#endif //__SPRITE_H__
