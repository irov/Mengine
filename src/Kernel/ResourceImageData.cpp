#include "ResourceImageData.h"

#include "Interface/ImageCodecInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstString.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageData::ResourceImageData()
        : m_width( 0 )
        , m_height( 0 )
        , m_widthF( 0.f )
        , m_heightF( 0.f )
        , m_maxSize( 0.f, 0.f )
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

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "image file '%s:%s' was not found"
            , this->getFileGroup()->getName().c_str()
            , this->getFilePath().c_str()
        );

        MENGINE_ASSERTION_FATAL( stream->size() != 0, "empty stream '%s:%s' codec '%s'"
            , this->getFileGroup()->getName().c_str()
            , this->getFilePath().c_str()
            , this->getCodecType().c_str()
        );

        const ConstString & codecType = this->getCodecType();

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( imageDecoder, "image decoder '%s' for file '%s:%s' was not found"
            , this->getCodecType().c_str()
            , this->getFileGroup()->getName().c_str()
            , this->getFilePath().c_str()
        );

        if( imageDecoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "image decoder '%s' for file '%s:%s' was not found"
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
        EPixelFormat format = dataInfo->format;

        uint32_t memorySize = Helper::getTextureMemorySize( width, height, channels, 1, format );

        m_buffer = Helper::allocateArrayT<uint8_t>( memorySize );

        ImageCodecOptions options;
        options.pitch = width * channels;
        options.channels = channels;

        if( imageDecoder->setOptions( &options ) == false )
        {
            LOGGER_ERROR( "image decoder '%s' for file '%s:%s' invalid optionize"
                , this->getCodecType().c_str()
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        if( imageDecoder->decode( m_buffer, memorySize ) == 0 )
        {
            LOGGER_ERROR( "image decoder '%s' for file '%s:%s' invalid decode"
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

        m_widthF = (float)_width;
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

        m_heightF = (float)_height;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceImageData::getImageHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceImageData::getImageWidthF() const
    {
        return m_widthF;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceImageData::getImageHeightF() const
    {
        return m_heightF;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer ResourceImageData::getImageBuffer() const
    {
        return m_buffer;
    }
    //////////////////////////////////////////////////////////////////////////
}
