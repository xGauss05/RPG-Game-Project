#ifndef __PARTICLE_POOL_H__
#define __PARTICLE_POOL_H__

#include "ParticleDB.h"
#include "Particle.h"

#include <vector>
#include <random>

class ParticlePool
{
public:
	explicit ParticlePool(
		size_t numberOfParticles,
		ParticleDB const* atlas,
		ParticleDB::BluePrint name
	);

	void CreateParticle(size_t amount = 1);

private:
	struct ParticlePrototype
	{
		explicit ParticlePrototype(std::pair<Particle::Properties, ParticleDB::RandomValues>  const&name);
		Particle::Properties GenerateProperties();

		Particle::Properties m_Original;
		ParticleDB::RandomValues m_Modifiers;

		std::mt19937 m_RandomEngine{ (std::random_device())() };

		fPoint GetRandomValue(fPoint min, fPoint max);
	};

	void KillParticle(size_t index);

	std::vector<Particle> m_Particles;
	size_t m_AliveParticles = 0;

	ParticlePrototype m_ParticlePrototype;
};

#endif //__PARTICLE_POOL_H__
