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
		void setFilePath( const FilePath & _filePath );
        const FilePath & getFilePath() const;

        void setCodecType( const ConstString & _codecType );
        const ConstString & getCodecType() const;

    public:
        void setImagePath( const FilePath & _imagePath );
        const FilePath & getImagePath() const;

    public:
        void setImageMaxSize( const mt::vec2f & _maxSize );
        const mt::vec2f & getImageMaxSize() const;

	public:
		void setImageWidth( uint32_t _width );
        uint32_t getImageWidth() const override;
		
		void setImageHeight( uint32_t _height );
        uint32_t getImageHeight() const override;

        Pointer getImageBuffer() const override;

    public:
		void setValidNoExist( bool _validNoExist );
        bool isValidNoExist() const;

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
	//////////////////////////////////////////////////////////////////////////
}