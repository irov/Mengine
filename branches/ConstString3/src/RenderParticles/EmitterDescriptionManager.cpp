#	include "EmitterDescriptionManager.h"
#	include "EmitterDescription.h"

namespace Menge
{
	/////////////////////////////////////////////////////////////////////
	EmitterDescription * Menge::EmitterDescriptionManager::createEmitterDescription()
	{
		EmitterDescription * descr = new EmitterDescription();
		m_emitters.push_back(descr);
		return descr;
	}
	////////////////////////////////////////////////////////////////////
	ParticlesAtlas * EmitterDescriptionManager::createAtlas(  const String & _file, const String & _path )
	{
		ParticlesAtlas * atlas = new ParticlesAtlas(  _file, _path );
		m_atlasses.push_back(atlas);
		return atlas;
	}
	////////////////////////////////////////////////////////////////////
	void EmitterDescriptionManager::destroy()
	{
		for( TVectorEmitterDescription::iterator
			it = m_emitters.begin(),
			it_end = m_emitters.end();
		it != it_end;
		++it )
		{
			delete (*it);
		}

		m_emitters.clear();

		for( TVectorParticlesAtlas::iterator
			it = m_atlasses.begin(),
			it_end = m_atlasses.end();
		it != it_end;
		++it )
		{
			delete (*it);
		}

		m_atlasses.clear();
	}
	////////////////////////////////////////////////////////////////////
}

