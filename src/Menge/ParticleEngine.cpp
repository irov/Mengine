#	include "ParticleEngine.h"
#	include "FileEngine.h"
#	include "LogEngine.h"

#	include <vector>

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
	//////////////////////////////////////////////////////////////////////////
	EmitterContainerInterface * ParticleEngine::createEmitterContainerFromFile( const std::string & _filename )
	{
		FileDataInterface * fileData = Holder<FileEngine>::hostage()->openFile( _filename );

		if( fileData == 0 )
		{
			MENGE_LOG( "Error: Emitter can't open resource file '%s'\n", _filename.c_str() );
			return 0;
		}

		size_t buff_size = fileData->size();

		std::vector<char> buffer( buff_size );
		fileData->read( &buffer[0], buff_size );

		EmitterContainerInterface * container = m_interface->createEmitterContainerFromMemory( &buffer[0] );

		Holder<FileEngine>::hostage()->closeFile( fileData );

		return container;
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterInterface * ParticleEngine::createEmitterFromContainer( const char * _name, const EmitterContainerInterface * _container )
	{
		return m_interface->createEmitterFromContainer( _name, _container );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::lockEmitter( EmitterInterface * _emitter, int _typeParticle )
	{
		return m_interface->lockEmitter( _emitter, _typeParticle );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::releaseEmitter( EmitterInterface * _emitter )
	{
		return m_interface->releaseEmitter( _emitter );
	}
	//////////////////////////////////////////////////////////////////////////
	const char * ParticleEngine::getTextureName() const
	{
		return m_interface->getTextureName();
	}
	//////////////////////////////////////////////////////////////////////////
	RenderParticle * ParticleEngine::nextParticle()
	{
		return m_interface->nextParticle();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::unlockEmitter( EmitterInterface * _emitter )
	{
		return m_interface->unlockEmitter( _emitter );
	}
	//////////////////////////////////////////////////////////////////////////
}