#pragma once

#include "Kernel/Node.h"

#include "Kernel/Soundable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class SurfaceSound> SurfaceSoundPtr;
    //////////////////////////////////////////////////////////////////////////
    class SoundEmitter
        : public Node
    {
        DECLARE_VISITABLE( Node );

    public:
        SoundEmitter();
        ~SoundEmitter() override;

    public:
        void setSurfaceSound( const SurfaceSoundPtr & _surfaceSound );
        const SurfaceSoundPtr & getSurfaceSound() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        SurfaceSoundPtr m_surfaceSound;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundEmitter> SoundEmitterPtr;
    //////////////////////////////////////////////////////////////////////////
};
