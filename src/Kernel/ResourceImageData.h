#pragma once

#include "Kernel/Resource.h"
#include "Kernel/UnknownResourceImageDataInterface.h"

#include "Config/Typedef.h"

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
        const FilePath & getFileName() const;

        void setCodecType( const ConstString & _codecType );
        const ConstString & getCodecType() const;

    public:
        void setImagePath( const FilePath & _imagePath );
        const FilePath & getImagePath() const;

    public:
        void setImageMaxSize( const mt::vec2f & _maxSize );
        const mt::vec2f & getImageMaxSize() const;

    protected:
        uint32_t getImageWidth() const override;
        uint32_t getImageHeight() const override;

        Pointer getImageBuffer() const override;

    public:
        bool isValidNoExist() const;

    protected:
        bool _loader( const Metabuf::Metadata * _parser ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        FilePath m_fileName;
        ConstString m_codecType;

        uint32_t m_width;
        uint32_t m_height;

        mt::vec2f m_maxSize;
        uint8_t * m_buffer;

        bool m_validNoExist;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceImageData> ResourceImageDataPtr;
}