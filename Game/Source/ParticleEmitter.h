#ifndef __PARTICLE_EMITTER_H__
#define __PARTICLE_EMITTER_H__

#include "ParticleDB.h"
#include "ParticlePool.h"

class ParticleEmitter
{
public:
	struct Emission
	{
		int m_EmissionRate = 0;
		int m_MinVariance = 0;
		int m_MaxVariance = 0;
	};

	struct Age
	{
		int m_Current = 0;
		int m_Max = 0;
	};

	explicit ParticleEmitter(iPoint position, ParticleDB::BluePrint const& blueprint);

	void SetEmitterProperties(Emission const& emissionPerFrame, bool constant = true, Age const& age = Age());

	bool Update();

private:
	void SetNextEmissionTimer();

	Emission m_EmissionPerFrame;

	int m_TotalParticles = 0;
	int m_RemainingParticles = 0;
	int m_ParticlesPerEmission = 0;
	int m_NextEmissionTimer = 0;

	bool bConstant = false;
	Age m_Age;

	iPoint m_Position = { 0, 0 };

	ParticlePool m_Particles;
};

#endif //__PARTICLE_EMITTER_H__
