#include "ParticleDB.h"

#include "App.h"

#include "TextureManager.h"

ParticleDB::ParticleDB()
{
	CreateBluePrints();
}

auto ParticleDB::GetParticlePrototypeFromBluePrint(BluePrintTypes name) const
	-> std::pair<Particle::Properties, ParticleDB::BluePrint::RandomValues>
{
	return { m_Data[static_cast<int>(name)].m_BaseParticle, m_Data[static_cast<int>(name)].m_RandomProperties };
}

void ParticleDB::CreateBluePrints()
{
	int textureID = app->tex->Load("Assets/Textures/Particles/Particle.png");
	auto textureSize = app->tex->GetSize(textureID);

	m_Data[static_cast<int>(BluePrintTypes::FIRE)].m_BaseParticle = 
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

	m_Data[static_cast<int>(BluePrintTypes::FIRE)].m_RandomProperties =
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
}
