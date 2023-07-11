#include "ResourceHIT.h"

#include "Interface/PickCodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceHIT::ResourceHIT()
        : m_width( 0 )
        , m_height( 0 )
        , m_mipmaplevel( 0 )
        , m_widthF( 0.f )
        , m_heightF( 0.f )
        , m_mipmapsize( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceHIT::~ResourceHIT()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceHIT::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "name '%s' - hit file '%s' not found"
            , this->getName().c_str()
            , this->getContent()->getFilePath().c_str()
        );

        const ConstString & codecType = content->getCodecType();

        PickDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, "name '%s' - hit file '%s' invalid create decoder '%s'"
            , this->getName().c_str()
            , this->getContent()->getFilePath().c_str()
            , this->getContent()->getCodecType().c_str()
        );

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "resource HIT '%s' file '%s' invalid initialize decoder '%s'"
                , this->getName().c_str()
                , this->getContent()->getFilePath().c_str()
                , this->getContent()->getCodecType().c_str()
            );

            return false;
        }

        const PickCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        m_width = dataInfo->width;
        m_height = dataInfo->height;
        m_mipmaplevel = dataInfo->mipmaplevel;

        m_widthF = (float)m_width;
        m_heightF = (float)m_height;

        MemoryBufferInterfacePtr mipmap = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mipmap, "name '%s' - hit file '%s' invalid create memory"
            , this->getName().c_str()
            , this->getContent()->getFilePath().c_str()
        );

        size_t mipmapsize = (size_t)dataInfo->mipmapsize;
        void * buffer = mipmap->newBuffer( mipmapsize );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "name '%s' - hit file '%s' invalid new memory '%zu'"
            , this->getName().c_str()
            , this->getContent()->getFilePath().c_str()
            , mipmapsize
        );

        DecoderData data;
        data.buffer = buffer;
        data.size = mipmapsize;

        size_t test_mipmapsize = decoder->decode( &data );

        if( test_mipmapsize != mipmapsize )
        {
            LOGGER_ERROR( "resource HIT '%s' invalid decode hit '%s' size %u (%zu)"
                , this->getName().c_str()
                , this->getContent()->getFilePath().c_str()
                , m_mipmapsize
                , mipmapsize
            );

            return false;
        }

        m_mipmap = mipmap;
        m_mipmapsize = (uint32_t)mipmapsize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceHIT::_release()
    {
        m_mipmap = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceHIT::testPoint( const mt::vec2f & _point, float _minAlpha ) const
    {
        float fi = _point.x;
        float fj = _point.y;

        if( fi < 0.f || fj < 0.f )
        {
            return false;
        }

        uint32_t i = (uint32_t)fi;
        uint32_t j = (uint32_t)fj;

        if( i >= m_width || j >= m_height )
        {
            return false;
        }

        uint8_t minAlpha = (uint8_t)(_minAlpha * 255.f);
        uint32_t index = j * m_width + i;

        uint8_t * alphaBuffer = this->getHitBuffer_( 0 );
        uint8_t alpha = alphaBuffer[index];

        if( alpha <= minAlpha )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceHIT::testRadius( const mt::vec2f & _point, float _radius, float _minAlpha ) const
    {
        float fi = _point.x;
        float fj = _point.y;

        float w = m_widthF;
        float h = m_heightF;

        if( fi < -_radius || fj < -_radius )
        {
            return false;
        }

        if( fi > w + _radius || fj > h + _radius )
        {
            return false;
        }

        if( fi < 0.f )
        {
            fi = 0.f;
        }

        if( fj < 0.f )
        {
            fj = 0.f;
        }

        if( fi > w )
        {
            fi = w;
        }

        if( fj > h )
        {
            fj = h;
        }

        uint32_t i = (uint32_t)fi;
        uint32_t j = (uint32_t)fj;

        float radiuslog = MENGINE_LOGF( _radius );

        float levelF = radiuslog * mt::constant::inv_log2;
        uint32_t level = (uint32_t)levelF;

        if( level >= m_mipmaplevel )
        {
            level = m_mipmaplevel - 1;
        }

        uint8_t * alphaBuffer = this->getHitBuffer_( level );

        MENGINE_ASSERTION_MEMORY_PANIC( alphaBuffer, "'%s' hit file '%s' invalid get level buffer %u:%u"
            , this->getName().c_str()
            , this->getContent()->getFilePath().c_str()
            , level
            , m_mipmaplevel
        );

        i >>= level;
        j >>= level;

        uint32_t alphaWidth = m_width >> level;
        uint32_t alphaHeight = m_height >> level;

        if( i == alphaWidth )
        {
            --i;
        }

        if( j == alphaHeight )
        {
            --j;
        }

        uint8_t minAlpha = (uint8_t)(_minAlpha * 255.f);
        uint32_t index = i + j * alphaWidth;

        uint8_t alpha = alphaBuffer[index];

        if( alpha <= minAlpha )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t * ResourceHIT::getHitBuffer_( uint32_t _level ) const
    {
        uint32_t bufferOffset = 0;

        for( uint32_t l = 0; l != _level; ++l )
        {
            uint32_t offset = (m_width >> l) * (m_height >> l);
            bufferOffset += offset;
        }

        if( bufferOffset >= m_mipmapsize )
        {
            LOGGER_ERROR( "resource HIT '%s' file '%s' invalid get level buffer %u:%u"
                , this->getName().c_str()
                , this->getContent()->getFilePath().c_str()
                , _level
                , m_mipmaplevel
            );

            return nullptr;
        }

        uint8_t * buffer_memory = m_mipmap->getBuffer();

        uint8_t * buffer = buffer_memory + bufferOffset;

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceHIT::getImageWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceHIT::getImageHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceHIT::getImageWidthF() const
    {
        return m_widthF;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceHIT::getImageHeightF() const
    {
        return m_heightF;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer ResourceHIT::getImageBuffer() const
    {
        uint8_t * buffer = this->getHitBuffer_( 0 );

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
}
