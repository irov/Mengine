#include "SoundEmitter.h"

#include "Kernel/SurfaceSound.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

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

        this->recompile( [this, &_surfaceSound]()
        {
            m_surfaceSound = _surfaceSound;

            if( m_surfaceSound == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const SurfaceSoundPtr & SoundEmitter::getSurfaceSound() const
    {
        return m_surfaceSound;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundEmitter::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_surfaceSound, "'%s' can`t setup sound surface"
            , this->getName().c_str()
        );

        if( m_surfaceSound->compile() == false )
        {
            LOGGER_ERROR( "sound emitter '%s' can`t compile sound surface '%s'"
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
        m_surfaceSound->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundEmitter::_dispose()
    {
        m_surfaceSound = nullptr;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
}
