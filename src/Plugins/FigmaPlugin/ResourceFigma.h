#pragma once

#include "FigmaDataInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/FilePath.h"
#include "Kernel/String.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceFigma
        : public Resource
    {
        DECLARE_FACTORABLE( ResourceFigma );
        DECLARE_VISITABLE( Resource );

    public:
        ResourceFigma();
        ~ResourceFigma() override;

    public:
        void setSidecarPath( const FilePath & _sidecarPath );
        const FilePath & getSidecarPath() const;

    public:
        const FigmaDataInterfacePtr & getData() const;
        bool getFrameViewportSize( const String & _startFrameId, mt::vec2f * const _size );

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        bool loadSidecar_( const FigmaDataInterfacePtr & _data );

    protected:
        FigmaDataInterfacePtr m_data;
        FilePath m_sidecarPath;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceFigma> ResourceFigmaPtr;
    //////////////////////////////////////////////////////////////////////////
}
