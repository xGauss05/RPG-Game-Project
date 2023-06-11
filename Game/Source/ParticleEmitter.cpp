#include "Particle.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(iPoint position, ParticleDB::BluePrint const& blueprint)
	: m_Position(position), m_Particles(m_Position, 20, blueprint.GetParticlePrototype())
{}

void ParticleEmitter::SetEmitterProperties(Emission const& emissionPerFrame, bool constant, Age const& age)
{
	m_EmissionPerFrame = emissionPerFrame;
	bConstant = constant;
	m_Age = age;
}

ParticleEmitter* ParticleEmitter::GetNextEmitter() const
{
	return m_Next;
}

void ParticleEmitter::SetNextEmitter(ParticleEmitter* next)
{
	m_Next = next;
}
