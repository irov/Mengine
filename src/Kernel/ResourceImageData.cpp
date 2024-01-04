#include "ResourceImageData.h"

#include "Interface/ImageCodecInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstString.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"

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
        const ContentInterfacePtr & content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "image file '%s' was not found"
            , Helper::getContentFullPath( this->getContent() )
        );

        MENGINE_ASSERTION_FATAL( stream->size() != 0, "empty file '%s' codec '%s'"
            , Helper::getContentFullPath( this->getContent() )
            , this->getContent()->getCodecType().c_str()
        );

        const ConstString & codecType = content->getCodecType();

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( imageDecoder, "image decoder '%s' for file '%s' was not found"
            , this->getContent()->getCodecType().c_str()
            , Helper::getContentFullPath( this->getContent() )
        );

        if( imageDecoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "image decoder '%s' for file '%s' was not found"
                , this->getContent()->getCodecType().c_str()
                , Helper::getContentFullPath( this->getContent() )
            );

            return false;
        }

        const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

        uint32_t width = dataInfo->width;
        uint32_t height = dataInfo->height;
        EPixelFormat format = dataInfo->format;

        uint32_t memorySize = Helper::getTextureMemorySize( width, height, format );

        m_buffer = Helper::allocateMemoryNT<uint8_t>( memorySize, "image_data" );

        uint32_t channels = Helper::getPixelFormatChannels( format );

        ImageDecoderData data;
        data.buffer = m_buffer;
        data.size = memorySize;
        data.pitch = width * channels;
        data.format = format;

        if( imageDecoder->decode( &data ) == 0 )
        {
            LOGGER_ERROR( "image decoder '%s' for file '%s' invalid decode"
                , this->getContent()->getCodecType().c_str()
                , Helper::getContentFullPath( this->getContent() )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageData::_release()
    {
        Helper::deallocateMemory( m_buffer, "image_data" );
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
