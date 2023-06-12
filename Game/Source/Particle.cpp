#include "Particle.h"

#include "App.h"
#include "Render.h"

#include "Log.h"

void Particle::Init(Properties const& data)
{
	m_Data = data;
}

bool Particle::Update()
{
	LOG("XD");
	return true;
}
