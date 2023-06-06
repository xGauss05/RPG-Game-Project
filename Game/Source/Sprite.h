#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "App.h"

#include "Render.h"
#include "TextureManager.h"

#include "Point.h"

#include "PugiXml/src/pugixml.hpp"

class Sprite
{
public:
	int GetGid(bool interacted = false) const
	{ 
		return interacted ? interactedGid : gid;
	};
	iPoint GetTextureIndex() const { return textureIndex; };

	void Draw(iPoint position) const
	{
		if(auto result = animation.find(currentAnim);
			result != animation.end())
		{
			app->render->DrawTexture(
				DrawParameters(
					result->second.textureID,
					position
				).Section(&(result->second.currentAnimFrame))
			);
		}
	};
	virtual void UpdateAnim() const {};

	void LoadTextures(pugi::xml_node const& node)
	{
		Animation baseAnim;

		auto imageNode = node.child("image");

		std::string path = imageNode.attribute("source").as_string();

		baseAnim.textureID = app->tex->Load("Assets/Textures/Events/Static/" + path);
		baseAnim.currentAnimFrame =
		{
			0,
			0,
			imageNode.attribute("width").as_int(),
			imageNode.attribute("height").as_int()
		};

		animation[AnimationState::BASE] = baseAnim;

		auto animationNode = node.child("properties").find_child_by_attribute("propertytype", "Event Animations");

		for (auto const& stateAnim : animationNode.child("properties").children("property"))
		{
			Animation anim;

			for (auto const& stateProp : stateAnim.child("properties").children("property"))
			{
				if (StrEquals(stateProp.attribute("name").as_string(), "LoopTimer"))
				{
					anim.maxTimer = stateProp.attribute("value").as_int();
				}
				else if (StrEquals(stateProp.attribute("name").as_string(), "Path"))
				{
					std::string_view p = stateProp.attribute("value").as_string();

					if (p.starts_with("../"))
					{
						p.remove_prefix(3);
					}

					anim.textureID = app->tex->Load(std::format("Assets/Textures/Events/{}", p));
				}
			}

			std::string animName = stateAnim.attribute("name").as_string();

			if (StrEquals(animName, "Off"))
			{
				animation[AnimationState::STATE_OFF] = anim;
			}
			else if (StrEquals(animName, "On"))
			{
				animation[AnimationState::STATE_ON] = anim;
			}
			else if (StrEquals(animName, "Triggered"))
			{
				animation[AnimationState::TRIGGERED] = anim;
			}
		}
	}
protected:

	void Initialize(int index)
	{
		textureIndex = (index < 4)
			? iPoint(index * 3, 0)
			: iPoint((index - 4) * 3, 4);
	}

	void Initialize(pugi::xml_node const& node)
	{
		gid = node.attribute("gid").as_uint();

	};

	void SetInteractedGid()
	{
		interactedGid = gid + 1;
	}

private:
	int gid = -1;
	int interactedGid = -1;
	iPoint textureIndex = { 0, 0 };

	enum class AnimationState
	{
		BASE,
		STATE_OFF,
		STATE_ON,
		TRIGGERED
	};

	struct Animation
	{
		int textureID = -1;
		int maxTimer = 0;
		SDL_Rect currentAnimFrame = { 0, 0, 0, 0 };
	};

	std::unordered_map<AnimationState, Animation> animation;

	const int tileSize = 48;
	AnimationState currentAnim = AnimationState::BASE;
	int animTimer = 0;

};

#endif //__SPRITE_H__
