#include "Particle.h"

#include "App.h"
#include "Render.h"

#include "Log.h"

#include <cmath>
#include <assert.h>

void Particle::Init(Properties const& data)
{
	m_Data = data;
	m_TotalFrames = m_Data.m_FramesAlive;
	m_Data.m_FramesAlive = 0;
}

bool Particle::Update()
{
	//assert(m_TotalFrames == 0 && "Total particle frames is 0");

	if (m_Data.m_FramesAlive > m_TotalFrames) [[unlikely]]
	{
		return false;
	}

	float dt = m_Data.m_FramesAlive / m_TotalFrames;

	m_Data.m_FramesAlive++;
	timer++;
	if (timer % 10 == 0) [[unlikely]]
	m_Data.m_Physics.m_Speed.m_Current += m_Data.m_Physics.m_Acceleration;


	m_Data.m_Position += m_Data.m_Physics.m_Speed.m_Current;
	//m_Data.m_Position.y += speed.y;

	app->render->DrawTexture(
		{
			m_Data.m_Texture.m_TextureID,
			{
				static_cast<int>(m_Data.m_Position.x),
				static_cast<int>(m_Data.m_Position.y)
			}
		}
	);

	LOG("Particle pos is x: %f y:%f", m_Data.m_Position.x, m_Data.m_Position.y);

	return true;
}

fPoint Particle::Properties::Physics::Parameters::GetLerp(float dt)
{
	m_Current =
	{
			std::lerp
			(
				m_Start.x,
				m_End.x,
				dt
			),
			std::lerp
			(
				m_Start.y,
				m_End.y,
				dt
			),
	};

	return m_Current;
}
