#pragma once

#include "Interface/MemoryInterface.h"

#include "Engine/ResourceTestPick.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceHIT
        : public ResourceTestPick
    {
        DECLARE_FACTORABLE( ResourceHIT );
        DECLARE_VISITABLE( ResourceTestPick );

    public:
        ResourceHIT();
        ~ResourceHIT() override;

    protected:
        bool _compile() override;
        void _release() override;

    public:
        bool testPoint( const mt::vec2f & _point, float _minAlpha ) const override;
        bool testRadius( const mt::vec2f & _point, float _radius, float _minAlpha ) const override;

    protected:
        uint32_t getImageWidth() const override;
        uint32_t getImageHeight() const override;

    protected:
        float getImageWidthF() const override;
        float getImageHeightF() const override;

    protected:
        Pointer getImageBuffer() const override;

    protected:
        uint8_t * getHitBuffer_( uint32_t _level ) const;

    protected:
        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_mipmaplevel;

        float m_widthF;
        float m_heightF;

        MemoryInterfacePtr m_mipmap;
        uint32_t m_mipmapsize;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceHIT> ResourceHITPtr;
    //////////////////////////////////////////////////////////////////////////
}
