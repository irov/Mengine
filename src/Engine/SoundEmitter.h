#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/Surface.h"
#include "Kernel/Soundable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveSurfacePtr<class SurfaceSound> SurfaceSoundPtr;
    //////////////////////////////////////////////////////////////////////////
    class SoundEmitter
        : public Node
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( SoundEmitter );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

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
        void _dispose() override;

    protected:
        SurfaceSoundPtr m_surfaceSound;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<SoundEmitter> SoundEmitterPtr;
    //////////////////////////////////////////////////////////////////////////
};
