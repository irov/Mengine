#pragma once

#include "Interface/MemoryInterface.h"

#include "ResourceTestPick.h"

#include "Kernel/BaseContent.h"

namespace Mengine
{
    class ResourceHIT
        : public ResourceTestPick
        , private BaseContent
    {
        DECLARE_VISITABLE( ResourceTestPick );
        DECLARE_CONTENTABLE();

    public:
        ResourceHIT();
        ~ResourceHIT() override;

    protected:
        bool _compile() override;
        void _release() override;

    public:
        void setPath( const FilePath & _filePath );

    public:
        bool testPoint( const mt::vec2f & _point, float _minAlpha ) const override;
        bool testRadius( const mt::vec2f & _point, float _radius, float _minAlpha ) const override;

    protected:
        uint32_t getImageWidth() const override;
        uint32_t getImageHeight() const override;

        Pointer getImageBuffer() const override;

    protected:
        uint8_t * getHitBuffer_( uint32_t _level ) const;

    protected:
        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_mipmaplevel;

        MemoryInterfacePtr m_mipmap;
        uint32_t m_mipmapsize;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceHIT> ResourceHITPtr;
    //////////////////////////////////////////////////////////////////////////
}
