#include "ResourceImageData.h"

#include "Interface/FileSystemInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Kernel/MemoryAllocator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageData::ResourceImageData()
        : m_maxSize( 0.f, 0.f )
        , m_width( 0 )
        , m_height( 0 )
        , m_buffer( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageData::~ResourceImageData()
    {
    }	
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageData::_compile()
    {
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();
		const FilePath & filePath = this->getFilePath();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, filePath, false );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "ResourceImageData::_compile: Image file '%s:%s' was not found"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( m_codecType );

        if( imageDecoder == nullptr )
        {
            LOGGER_ERROR( "ResourceImageData::_compile: Image decoder '%s' for file '%s:%s' was not found"
                , this->getCodecType().c_str()
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        if( imageDecoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "ResourceImageData::_compile: Image decoder '%s' for file '%s:%s' was not found"
                , this->getCodecType().c_str()
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

        uint32_t width = dataInfo->width;
        uint32_t height = dataInfo->height;
        uint32_t channels = dataInfo->channels;
        PixelFormat format = dataInfo->format;

        size_t memorySize = Helper::getTextureMemorySize( width, height, channels, 1, format );

        m_buffer = Helper::allocateArrayT<uint8_t>( memorySize );

        ImageCodecOptions options;
        options.pitch = width * channels;
        options.channels = channels;

        if( imageDecoder->setOptions( &options ) == false )
        {
            LOGGER_ERROR( "ResourceImageData::_compile: Image decoder '%s' for file '%s:%s' invalid optionize"
                , this->getCodecType().c_str()
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        if( imageDecoder->decode( m_buffer, memorySize ) == 0 )
        {
            LOGGER_ERROR( "ResourceImageData::_compile: Image decoder '%s' for file '%s:%s' invalid decode"
                , this->getCodecType().c_str()
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageData::_release()
    {
        Helper::freeArrayT( m_buffer );
        m_buffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageData::setImageMaxSize( const mt::vec2f & _maxSize )
    {
        m_maxSize = _maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ResourceImageData::getImageMaxSize() const
    {
        return m_maxSize;
    }
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageData::setImageWidth( uint32_t _width )
	{
		m_width = _width;
	}
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceImageData::getImageWidth() const
    {
        return m_width;
    }
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageData::setImageHeight( uint32_t _height )
	{
		m_height = _height;
	}
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceImageData::getImageHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer ResourceImageData::getImageBuffer() const
    {
        return m_buffer;
    }
}
