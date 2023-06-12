#ifndef __PARTICLE_SYSTEM_H__
#define __PARTICLE_SYSTEM_H__

#include "ParticleEmitter.h"
#include "ParticleDB.h"

#include <vector>

class ParticleSystem
{
public:
	void AddEmitter(ParticleDB const& db, ParticleDB::BluePrintTypes type, iPoint position);
	bool IsValid() const;

	void Update();

private:
	std::vector<ParticleEmitter> m_Emitters;
	std::vector<bool> m_ActiveEmitters;
};

#endif //__PARTICLE_SYSTEM_H__
