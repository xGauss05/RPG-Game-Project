#include "Particle.h"

#include "App.h"
#include "Render.h"

#include "Log.h"

#include <cmath>
#include <assert.h>

void Particle::Init(Properties const& data)
{
	m_Data = data;
}

bool Particle::Update()
{
	assert(m_TotalFrames != 0 && "Total particle frames is 0");

	if (m_Data.m_FramesAlive > m_TotalFrames) [[unlikely]]
	{
		return false;
	}

	float dt = m_Data.m_FramesAlive / m_TotalFrames;

	m_Data.m_FramesAlive++;

	fPoint speed = m_Data.m_Physics.m_Speed.GetLerp(dt);

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
