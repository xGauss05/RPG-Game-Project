#ifndef __PARTICLE_EMITTER_H__
#define __PARTICLE_EMITTER_H__

#include "ParticleDB.h"
#include "ParticlePool.h"

class ParticleEmitter
{
public:
	struct Emission
	{
		int rate = 0;
		int min_variance = 0;
		int max_variance = 0;
	};

	struct Age
	{
		int current = 0;
		int max = 0;
	};

	explicit ParticleEmitter(iPoint position, ParticleDB::BluePrint const& type);

	void SetEmitterProperties(Emission const& emissionPerFrame, bool constant = true, Age const& age = Age());
	ParticleEmitter* GetNextEmitter() const;
	void SetNextEmitter(ParticleEmitter* next);

private:
	bool bIsActive = false;

	Emission m_EmissionPerFrame;

	bool bConstant = false;
	Age m_Age;

	iPoint m_Position = { 0, 0 };

	ParticlePool m_Particles;

	ParticleEmitter* m_Next = nullptr;
};

#endif //__PARTICLE_EMITTER_H__
