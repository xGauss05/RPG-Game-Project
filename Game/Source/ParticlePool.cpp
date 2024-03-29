#include "ParticlePool.h"

#include <assert.h>

ParticlePool::ParticlePool(iPoint position, size_t amountOfParticles, Pair_PropertiesAndRandom const& properties)
	: m_ParticlePrototype(properties)
{
	m_ParticlePrototype.m_Original.m_Position 
		+=  {
			static_cast<float>(position.x),
			static_cast<float>(position.y) 
			};
	m_Particles.reserve(amountOfParticles);
}

void ParticlePool::SetNewBluePrint(size_t amountOfParticles, Pair_PropertiesAndRandom const& properties)
{
	m_AliveParticles = 0;
	m_Particles.reserve(amountOfParticles);
	m_ParticlePrototype.m_Original = properties.first;
	m_ParticlePrototype.m_Modifiers = properties.second;
}

void ParticlePool::CreateParticle(size_t amount)
{
	if (m_AliveParticles + amount > m_Particles.capacity())
	{
		amount = m_Particles.capacity() - m_AliveParticles;
	}

	while(amount > 0)
	{
		m_Particles[m_AliveParticles].Init(m_ParticlePrototype.GenerateProperties());
		m_AliveParticles++;
		amount--;
	}
}

void ParticlePool::Update()
{
	for (int i = 0; i < m_AliveParticles; i++)
	{
		if (!m_Particles[i].Update())
		{
			KillParticle(i);
		}
	}
}

void ParticlePool::KillParticle(size_t index)
{
	assert(index < m_AliveParticles && "Trying to kill dead particle.");

	m_AliveParticles--;

	std::swap(m_Particles[index], m_Particles[m_AliveParticles]);
}

ParticlePool::ParticlePrototype::ParticlePrototype(Pair_PropertiesAndRandom const& name)
	: m_Original(name.first)
	, m_Modifiers(name.second)
{
}

Particle::Properties ParticlePool::ParticlePrototype::GenerateProperties()
{
	Particle::Properties newProperties = m_Original;

	newProperties.m_Position += GetRandomValue(m_Modifiers.m_MinPosition, m_Modifiers.m_MaxPosition);
	newProperties.m_Physics.m_Speed.m_Start += GetRandomValue(m_Modifiers.m_MinStartingSpeed, m_Modifiers.m_MaxStartingSpeed);
	newProperties.m_Physics.m_Speed.m_End += GetRandomValue(m_Modifiers.m_MinEndingSpeed, m_Modifiers.m_MaxEndingSpeed);
	newProperties.m_Physics.m_Size.m_Start += GetRandomValue(m_Modifiers.m_MinSize, m_Modifiers.m_MaxSize);

	return newProperties;
}

fPoint ParticlePool::ParticlePrototype::GetRandomValue(fPoint min, fPoint max)
{
	std::uniform_real_distribution<float> x(min.x, max.x);
	std::uniform_real_distribution<float> y(min.y, max.y);

	return { x(m_RandomEngine), y(m_RandomEngine) };
}

