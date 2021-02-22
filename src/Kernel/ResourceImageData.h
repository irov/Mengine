#pragma once

#include "Kernel/Resource.h"
#include "Kernel/UnknownResourceImageDataInterface.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceImageData
        : public Resource
        , public UnknownResourceImageDataInterface
    {
        DECLARE_UNKNOWABLE();
        DECLARE_VISITABLE( Resource );

    public:
        ResourceImageData();
        ~ResourceImageData() override;

    public:
        void setImageMaxSize( const mt::vec2f & _maxSize );
        const mt::vec2f & getImageMaxSize() const;

    public:
        void setImageWidth( uint32_t _width );
        uint32_t getImageWidth() const override;

    public:
        void setImageHeight( uint32_t _height );
        uint32_t getImageHeight() const override;

    public:
        float getImageWidthF() const override;
        float getImageHeightF() const override;

    public:
        Pointer getImageBuffer() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        uint32_t m_width;
        uint32_t m_height;

        float m_widthF;
        float m_heightF;

        mt::vec2f m_maxSize;
        uint8_t * m_buffer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceImageData> ResourceImageDataPtr;
    //////////////////////////////////////////////////////////////////////////
}