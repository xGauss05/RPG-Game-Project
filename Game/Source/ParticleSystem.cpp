#include "Particle.h"
#include "ParticleSystem.h"

void ParticleSystem::AddEmitter(ParticleDB const &db, ParticleDB::BluePrintTypes type, iPoint position)
{
	auto const &blueprint = db.GetBluePrint(type);

	auto &newEmitter = m_Emitters.emplace_back(
		position,
		blueprint
	);

	m_ActiveEmitters.emplace_back(true);

	auto const &bluePrintEmitter = blueprint.GetEmitterProperties();

	if (!bluePrintEmitter.m_MaxAge)
	{
		newEmitter.SetEmitterProperties(
			{
			.m_EmissionRate = bluePrintEmitter.m_EmissionRate,
			.m_MinVariance = bluePrintEmitter.m_MinVariance,
			.m_MaxVariance = bluePrintEmitter.m_MaxVariance
			}
		);
	}
	else
	{
		newEmitter.SetEmitterProperties(
			{
			.m_EmissionRate = bluePrintEmitter.m_EmissionRate,
			.m_MinVariance = bluePrintEmitter.m_MinVariance,
			.m_MaxVariance = bluePrintEmitter.m_MaxVariance
			},
			false,
			{ .m_Current = 0, .m_Max = bluePrintEmitter.m_MaxAge }
		);
	}
}

bool ParticleSystem::IsValid() const
{
	return !m_Emitters.empty();
}

void ParticleSystem::Update()
{
	for (int i = 0; i < m_ActiveEmitters.size(); i++)
	{
		if (!m_ActiveEmitters[i])
			continue;
		
		m_ActiveEmitters[i] = m_Emitters[i].Update();
	}
}
