#pragma once

#include "Kernel/Surface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ESurfaceTrackMatteMode
    {
        ESTM_MODE_NONE = 0,
        ESTM_MODE_ALPHA = 1,
        ESTM_MODE_ALPHA_INVERTED = 2,
        ESTM_MODE_LUMA = 3,
        ESTM_MODE_LUMA_INVERTED = 4,
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ResourceImage, class Resource> ResourceImagePtr;
    typedef IntrusivePtr<class RenderProgramVariableInterface> RenderProgramVariableInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SurfaceTrackMatte
        : public Surface
    {
    public:
        SurfaceTrackMatte();
        ~SurfaceTrackMatte() override;

    public:
        bool _initialize() override;

    public:
        void setResourceImage( const ResourceImagePtr & _resourceImage );
        const ResourceImagePtr & getResourceImage() const;

    public:
        void setResourceTrackMatteImage( const ResourceImagePtr & _resourceTrackMatteImage );
        const ResourceImagePtr & getResourceTrackMatteImage() const;

    public:
        const RenderProgramVariableInterfacePtr & getProgramVariable() const;

    public:
        void setTrackMatteMode( ESurfaceTrackMatteMode _trackMatteMode );
        ESurfaceTrackMatteMode getTrackMatteMode() const;

    public:
        const mt::vec2f & getMaxSize() const override;
        const mt::vec2f & getSize() const override;
        const mt::vec2f & getOffset() const override;

        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        void correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const override;

        const Color & getColor() const override;

    protected:
        bool update( const UpdateContext * _context ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        ResourceImagePtr m_resourceImage;
        ResourceImagePtr m_resourceTrackMatteImage;

        RenderProgramVariableInterfacePtr m_programVariable;

        ESurfaceTrackMatteMode m_trackMatteMode;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveSurfacePtr<SurfaceTrackMatte> SurfaceTrackMattePtr;
    //////////////////////////////////////////////////////////////////////////
}
