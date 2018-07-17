#include "SoundEmitter.h"

#include "SurfaceSound.h"

#include "Kernel/Logger.h"

namespace Mengine
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
        if( m_surfaceSound == _surfaceSound )
        {
            return;
        }

        m_surfaceSound = _surfaceSound;

        this->recompile();
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
			LOGGER_ERROR("SoundEmitter::_compile: '%s' can`t setup sound surface"
				, this->getName().c_str()				
				);

			return false;
		}

		if( m_surfaceSound->compile() == false )
        {
            LOGGER_ERROR("SoundEmitter::_compile: '%s' can`t compile sound surface '%s'"
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
