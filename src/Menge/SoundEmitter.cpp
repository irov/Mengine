#	include "SoundEmitter.h"

#	include "ResourceSound.h"

#	include "Interface/ResourceInterface.h"

#	include "Logger/Logger.h"

#	include <pybind/pybind.hpp>

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundEmitter::SoundEmitter()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundEmitter::~SoundEmitter()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setSurfaceSound( const SurfaceSoundPtr & _surfaceSound )
	{
		m_surfaceSound = _surfaceSound;
	}
	//////////////////////////////////////////////////////////////////////////
	const SurfaceSoundPtr & SoundEmitter::getSurfaceSound() const
	{
		return m_surfaceSound;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::_compile()
	{
		if( m_surfaceSound == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEmitter::_compile: '%s' can`t setup sound surface"
				, this->getName().c_str()				
				);

			return false;
		}

		if( m_surfaceSound->compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEmitter::_compile: '%s' can`t compile sound surface '%s'"
                , this->getName().c_str()
				, m_surfaceSound->getName().c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_release()
	{ 
		Node::_release();

		m_surfaceSound->release();
	}
}
