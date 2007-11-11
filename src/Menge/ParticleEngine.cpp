#	include "ParticleEngine.h"

namespace Menge
{
	ParticleEngine::ParticleEngine( ParticleSystemInterface * _interface )
	: m_interface( _interface )
	{
		Holder<ParticleEngine>::keep( this );
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEngine::~ParticleEngine()
	{
	}
}