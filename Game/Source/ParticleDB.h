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

	class BluePrint
	{
	public:
		struct RandomValues
		{
			fPoint m_MinPosition = { 0, 0 };
			fPoint m_MaxPosition = { 0, 0 };
			fPoint m_MinStartingSpeed = { 0, 0 };
			fPoint m_MaxStartingSpeed = { 0, 0 };
			fPoint m_MinEndingSpeed = { 0, 0 };
			fPoint m_MaxEndingSpeed = { 0, 0 };
			fPoint m_MinSize = { 0, 0 };
			fPoint m_MaxSize = { 0, 0 };
		};

		struct EmitterProperties
		{
			int m_EmissionRate = 0;
			int m_MinVariance = 0;
			int m_MaxVariance = 0;
			int m_MaxAge = 0;
			int m_AmountOfParticles = 0;
		};

		void SetParticle(Particle::Properties const&, RandomValues const&);
		void SetEmitter(EmitterProperties const&);

		std::pair<Particle::Properties, RandomValues>
			GetParticlePrototype() const;

		EmitterProperties GetEmitterProperties() const;

	private:
		Particle::Properties m_BaseParticle;
		RandomValues m_RandomProperties;
		EmitterProperties m_EmitterProperties;
	};

	ParticleDB();

	BluePrint GetBluePrint(BluePrintTypes type) const;

private:
	void CreateBluePrints();

	std::array<BluePrint,static_cast<int>(BluePrintTypes::LAST_BLUEPRINT)> m_Data;
};

#endif //__PARTICLE_DB_H__
