#ifndef __PARTICLE_DB_H__
#define __PARTICLE_DB_H__

#include "Particle.h"

#include <utility>
#include <random>
#include <array>

class ParticleDB
{
public:
	using RandomParam = const std::uniform_real_distribution<float>::param_type;

	enum class BluePrintTypes
	{
		FIRE = 0,
		LAST_BLUEPRINT // For array purposes
	};

	struct BluePrint
	{
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

		Particle::Properties m_BaseParticle;
		RandomValues m_RandomProperties;
	};

	ParticleDB();

	std::pair<Particle::Properties, BluePrint::RandomValues>
		GetParticlePrototypeFromBluePrint(BluePrintTypes name) const;

private:
	void CreateBluePrints();

	std::array<BluePrint,static_cast<int>(BluePrintTypes::LAST_BLUEPRINT)> m_Data;
};

#endif //__PARTICLE_DB_H__
