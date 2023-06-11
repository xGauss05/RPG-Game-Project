#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Point.h"

#include "SDL/include/SDL.h"

#include <variant>

class Particle
{
public:
	struct Properties
	{
		struct Physics
		{
			struct Parameters
			{
				fPoint m_Start = { 0, 0 };
				fPoint m_Current = { 0, 0 };
				fPoint m_End = { 0, 0 };
			};

			Parameters m_Speed;
			Parameters m_Size;
			Parameters m_RotationSpeed;
			fPoint m_Acceleration;
		};
		struct Rendering
		{
			struct Parameters
			{
				SDL_Color m_Start = { 0, 0, 0, 0};
				SDL_Color m_Current = { 0, 0, 0, 0 };
				SDL_Color m_End = { 0, 0, 0, 0 };
			};

			int m_TextureID = -1;
			SDL_Rect m_DstRect = { 0, 0, 0, 0 };
			Parameters m_Color;
		};

		fPoint m_Position = { 0, 0 };
		Physics m_Physics;
		Rendering m_Texture;
		float m_FramesLeft = 0;
	};

	void Init(Properties const& data);

private:
	Properties m_Data;
};

#endif //__PARTICLE_H__
