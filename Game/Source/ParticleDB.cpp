#include "ParticleDB.h"

#include "App.h"

#include "TextureManager.h"

ParticleDB::ParticleDB()
{
	CreateBluePrints();
}

std::pair<Particle::Properties, ParticleDB::RandomValues> ParticleDB::GetBluePrint(BluePrint name) const
{
	return m_Data.at(name);
}

void ParticleDB::CreateBluePrints()
{
	int textureID = app->tex->Load("Assets/Textures/Particles/Particle.png");
	auto textureSize = app->tex->GetSize(textureID);

	Particle::Properties p
	{
		.m_Position = {0, 0},
		.m_Physics =
		{
			.m_Speed =
			{
				.m_Start = {0, 0},
				.m_Current = {0, 0},
				.m_End = {0, 40}
			},
			.m_Size =
			{
				.m_Start = {6, 6},
				.m_Current = {0, 0},
				.m_End = {0, 0}
			},
			.m_RotationSpeed =
			{
				.m_Start = {0, 0},
				.m_Current = {0, 0},
				.m_End = {0, 0}
			},
			.m_Acceleration = { 0, 100 }
		},
		.m_Texture =
		{
			.m_TextureID = textureID,
			.m_DstRect = {0, 0, textureSize.x, textureSize.y},
			.m_Color = 
			{
				.m_Start = {255, 255, 0, 255},
				.m_Current = {0, 0, 0, 0},
				.m_End = {255, 0, 0, 0},
			}
		},
		.m_FramesLeft = 25
	};

	RandomValues r =
	{
		.m_MinPosition = { 0, 0 },
		.m_MaxPosition = {1, 1},
		.m_MinStartingSpeed = { -100, 100 },
		.m_MaxStartingSpeed = { 100, 300 },
		.m_MinEndingSpeed = { 0, 0 },
		.m_MaxEndingSpeed = { 0, 0 },
		.m_MinSize = { 0, 0 },
		.m_MaxSize = { 1 , 1}
	};

	m_Data.try_emplace(
		BluePrint::FIRE,
		p,
		r
	);
}
