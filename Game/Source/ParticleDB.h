#ifndef __PARTICLE_DB_H__
#define __PARTICLE_DB_H__

#include "Particle.h"

#include <utility>
#include <random>
#include <unordered_map>

class ParticleDB
{
public:
	using RandomParam = const std::uniform_real_distribution<float>::param_type;

	struct RandomValues
	{
		fPoint m_MinPosition;
		fPoint m_MaxPosition;
		fPoint m_MinStartingSpeed;
		fPoint m_MaxStartingSpeed;
		fPoint m_MinEndingSpeed;
		fPoint m_MaxEndingSpeed;
		fPoint m_MinSize;
		fPoint m_MaxSize;
	};

	enum class BluePrint
	{
		FIRE
	};

	ParticleDB();

	std::pair<Particle::Properties, RandomValues> GetBluePrint(BluePrint name) const;

private:
	void CreateBluePrints();

	std::unordered_map<BluePrint, std::pair<Particle::Properties, RandomValues>> m_Data;
};

#endif //__PARTICLE_DB_H__
