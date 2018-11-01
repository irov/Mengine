#include "ResourceHIT.h"

#include "Interface/PickCodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/Logger.h"

#include <math.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceHIT::ResourceHIT()
        : m_width( 0 )
        , m_height( 0 )
        , m_mipmaplevel( 0 )
        , m_mipmapsize( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceHIT::~ResourceHIT()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceHIT::_convert()
    {
        bool result = this->convertDefault_( m_converterType, m_filePath, m_filePath, m_codecType );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceHIT::_compile()
    {
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, m_filePath, false );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "ResourceHIT::_compile: '%s' - hit file '%s' not found"
                , this->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        PickDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<PickDecoderInterfacePtr>( m_codecType );

        if( decoder == nullptr )
        {
            LOGGER_ERROR( "ResourceHIT::_compile: '%s' - hit file '%s' invalid create decoder '%s'"
                , this->getName().c_str()
                , this->getFilePath().c_str()
                , this->getCodecType().c_str()
            );

            return false;
        }

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "ResourceHIT::_compile: '%s' - hit file '%s' invalid initialize decoder '%s'"
                , this->getName().c_str()
                , this->getFilePath().c_str()
                , this->getCodecType().c_str()
            );

            return false;
        }

        const PickCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        m_width = dataInfo->width;
        m_height = dataInfo->height;
        m_mipmaplevel = dataInfo->mipmaplevel;

        MemoryBufferInterfacePtr mipmap = MEMORY_SERVICE()
            ->createMemoryBuffer();

        if( mipmap == nullptr )
        {
            LOGGER_ERROR( "ResourceHIT::_compile: '%s' - hit file '%s' invalid create memory"
                , this->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        size_t mipmapsize = (size_t)dataInfo->mipmapsize;
        void * buffer = mipmap->newBuffer( mipmapsize, "ResourceHIT", __FILE__, __LINE__ );

        if( buffer == nullptr )
        {
            LOGGER_ERROR( "ResourceHIT::_compile: '%s' - hit file '%s' invalid new memory '%u'"
                , this->getName().c_str()
                , this->getFilePath().c_str()
                , mipmapsize
            );

            return false;
        }

        size_t test_mipmapsize = decoder->decode( buffer, mipmapsize );

        if( test_mipmapsize != mipmapsize )
        {
            LOGGER_ERROR( "ResourceHIT::_compile %s invalid decode hit %s size %d (%d)"
                , this->getName().c_str()
                , this->getFilePath().c_str()
                , (uint32_t)m_mipmapsize
                , (uint32_t)mipmapsize
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
    void ResourceHIT::setPath( const FilePath & _filePath )
    {
        if( m_filePath == _filePath )
        {
            return;
        }

        this->recompile( [this, _filePath]() { m_filePath = _filePath; } );
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
    bool ResourceHIT::testRadius( const mt::vec2f& _point, float _radius, float _minAlpha ) const
    {
        float fi = _point.x;
        float fj = _point.y;

        float w = (float)m_width;
        float h = (float)m_height;

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

        uint32_t level = (uint32_t)(logf( _radius ) / logf( 2.f ));

        if( level >= m_mipmaplevel )
        {
            level = m_mipmaplevel - 1;
        }

        uint8_t * alphaBuffer = this->getHitBuffer_( level );

        if( alphaBuffer == nullptr )
        {
            LOGGER_ERROR( "ResourceHIT::testRadius %s hit file %s invalid get level buffer %d:%d"
                , this->getName().c_str()
                , this->getFilePath().c_str()
                , level
                , m_mipmaplevel
            );

            return false;
        }

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

        if( alpha > minAlpha )
        {
            return true;
        }

        return false;
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
            LOGGER_ERROR( "ResourceHIT::getHitBuffer_ %s hit file %s invalid get level buffer %d:%d"
                , this->getName().c_str()
                , this->getFilePath().c_str()
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
    Pointer ResourceHIT::getImageBuffer() const
    {
        uint8_t * buffer = this->getHitBuffer_( 0 );

        return buffer;
    }
}
