#	include "ParticleEngine.h"
#	include "FileEngine.h"
#	include "LogEngine.h"

#	include <vector>

namespace Menge
{
	ParticleEngine::ParticleEngine( ParticleSystemInterface * _interface )
	: m_interface( _interface )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEngine::~ParticleEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterContainerInterface * ParticleEngine::createEmitterContainerFromFile( const std::string & _filename )
	{
		FileDataInterface * file = Holder<FileEngine>::hostage()->openFile( _filename );

		if( file == 0 )
		{
			MENGE_LOG( "Error: ParticleEngine can't open file '%s'\n", _filename.c_str() );
			return 0;
		}

		unsigned int length = file->size();

		std::vector<char> buffer;
		buffer.resize( length );

		file->read( &buffer[0], length );

		Holder<FileEngine>::hostage()->closeFile( file );

		EmitterContainerInterface * container = m_interface->createEmitterContainerFromMemory( &buffer[0] );

		if( container == 0 )
		{
			MENGE_LOG( "Error: ParticleEngine can't create emitter container '%s'\n", _filename.c_str() );
			return 0;
		}

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
	const std::string& ParticleEngine::getTextureName() const
	{
		static std::string name;
		name = m_interface->getTextureName() ? m_interface->getTextureName() : "";
		return name;
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
}