#include "ThreadTaskPrefetchTexture.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PixelFormatHelper.h"
#include "Config/DynamicCast.h"
#include "Config/StdLimits.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetchTexture::ThreadTaskPrefetchTexture()
        : m_codecFlags( 0 )
        , m_realFileGroup( nullptr )
        , m_streamOpened( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetchTexture::~ThreadTaskPrefetchTexture()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const PrefetchedRenderImageLoaderPtr & ThreadTaskPrefetchTexture::getImage() const
    {
        return m_image;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchTexture::initialize( const ContentInterfacePtr & _content, uint32_t _codecFlags )
    {
        m_content = _content;
        m_codecFlags = _codecFlags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchTexture::_onThreadTaskRun()
    {
        const FileGroupInterfacePtr & fileGroup = m_content->getFileGroup();
        const FilePath & filePath = m_content->getFilePath();
        m_stream = fileGroup->createInputFile( filePath, false, &m_realFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        if( m_stream == nullptr )
        {
            return false;
        }

        if( m_realFileGroup == nullptr )
        {
            return false;
        }

        const ConstString & codecType = m_content->getCodecType();
        DecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );
        DecoderInterface * decoderPtr = decoder.get();
        m_decoder = Helper::dynamicCast<ImageDecoderInterface *>( decoderPtr );

        if( m_decoder == nullptr )
        {
            return false;
        }

        m_encoded = MEMORY_SERVICE()
            ->createMemoryInput( MENGINE_DOCUMENT_FACTORABLE );

        if( m_encoded == nullptr )
        {
            return false;
        }

        m_pixels = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        if( m_pixels == nullptr )
        {
            return false;
        }

        m_image = Helper::makeFactorableUnique<PrefetchedRenderImageLoader>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchTexture::_onThreadTaskProcess()
    {
        const FilePath & filePath = m_content->getFilePath();

        if( m_realFileGroup->openInputFile( filePath, m_stream, 0, MENGINE_UNKNOWN_SIZE, false, false ) == false )
        {
            return false;
        }

        m_streamOpened = true;
        size_t encodedSize = m_stream->size();

        if( encodedSize == 0 )
        {
            return false;
        }

        void * encoded = m_encoded->newBuffer( encodedSize );

        if( encoded == nullptr )
        {
            return false;
        }

        if( m_stream->read( encoded, encodedSize ) != encodedSize )
        {
            return false;
        }

        if( this->isCancel() == true )
        {
            return false;
        }

        if( m_decoder->prepareData( nullptr, m_encoded ) == false )
        {
            return false;
        }

        const ImageCodecDataInfo * info = m_decoder->getCodecDataInfo();

        if( info->width == 0 || info->height == 0 )
        {
            return false;
        }

        if( info->mipmaps != 1 )
        {
            return false;
        }

        EPixelFormat format = info->format;

        if( format == PF_R8G8B8 )
        {
            format = PF_A8R8G8B8;
        }

        if( format != PF_A8R8G8B8 && format != PF_A8 && format != PF_L8 )
        {
            return false;
        }

        uint32_t channels = Helper::getPixelFormatChannels( format );
        size_t maxSize = StdLimits::numeric_limits<size_t>::max();

        if( info->width > maxSize / channels )
        {
            return false;
        }

        size_t rowBytes = size_t( info->width ) * channels;

        if( info->height > maxSize / rowBytes )
        {
            return false;
        }

        size_t byteCount = rowBytes * info->height;
        void * buffer = m_pixels->newBuffer( byteCount );

        if( buffer == nullptr )
        {
            return false;
        }

        ImageDecoderData data;
        data.buffer = buffer;
        data.size = byteCount;
        data.pitch = rowBytes;
        data.format = format;
        data.flags = m_codecFlags;
        data.mipmap = 0;

        if( m_decoder->decode( &data ) == 0 )
        {
            return false;
        }

        if( this->isCancel() == true )
        {
            return false;
        }

        RenderImageDesc desc;
        desc.mipmaps = 1;
        desc.width = info->width;
        desc.height = info->height;
        desc.format = format;
        m_image->initialize( desc, m_pixels, m_codecFlags );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchTexture::_onThreadTaskComplete( bool _successful )
    {
        m_decoder = nullptr;

        if( m_streamOpened == true )
        {
            m_realFileGroup->closeInputFile( m_stream );
            m_streamOpened = false;
        }

        m_stream = nullptr;
        m_realFileGroup = nullptr;
        m_encoded = nullptr;
        m_pixels = nullptr;
        m_content = nullptr;

        if( _successful == false )
        {
            m_image = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
