#include "Particle.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(iPoint position, ParticleDB::BluePrint const& blueprint)
	: m_TotalParticles(blueprint.GetEmitterProperties().m_AmountOfParticles)
	, m_Position(position)
	, m_Particles(m_Position, m_TotalParticles, blueprint.GetParticlePrototype())
{}

void ParticleEmitter::SetEmitterProperties(Emission const& emissionPerFrame, bool constant, Age const& age)
{
	m_EmissionPerFrame = emissionPerFrame;
	bConstant = constant;
	m_Age = age;

	m_RemainingParticles = m_TotalParticles;

	m_ParticlesPerEmission = (m_EmissionPerFrame.m_EmissionRate) 
		? m_TotalParticles / m_EmissionPerFrame.m_EmissionRate
		: m_TotalParticles;
	SetNextEmissionTimer();
}

bool ParticleEmitter::Update()
{
	if (m_RemainingParticles <= 0)
	{
		return false;
	}
	else if (m_NextEmissionTimer > 0)
	{
		m_NextEmissionTimer--;
	}
	else if (m_NextEmissionTimer == 0) [[likely]]
	{
		m_Particles.CreateParticle(m_ParticlesPerEmission);
		SetNextEmissionTimer();
		
		if(!bConstant)
			m_RemainingParticles -= m_ParticlesPerEmission;
	}

	m_Particles.Update();

	return true;
}

void ParticleEmitter::SetNextEmissionTimer()
{
	m_NextEmissionTimer = m_EmissionPerFrame.m_EmissionRate;
}
