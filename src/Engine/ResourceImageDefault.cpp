#include "ResourceImageDefault.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefault::ResourceImageDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefault::~ResourceImageDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefault::_convert()
    {
        bool result = this->convertDefault_( m_converterType, m_filePath, m_filePath, m_codecType, m_codecType );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefault::_compile()
    {
        LOGGER_INFO( "name %s load texture %s"
            , this->getName().c_str()
            , this->getFilePath().c_str()
        );

        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        if( fileGroup == nullptr )
        {
            LOGGER_ERROR( "name '%s' group '%s' file path '%s' invalid setup category"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->loadTexture( fileGroup, m_filePath, m_codecType );

        if( texture == nullptr )
        {
            LOGGER_ERROR( "name '%s' category '%s' group '%s' can't load image file '%s'"
                , this->getName().c_str()
                , this->getFileGroup()->getName().c_str()
                , this->getGroupName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        m_texture = texture;

        this->prepareImageFrame_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::_release()
    {
        ResourceImage::_release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefault::setup( const FilePath & _imagePath, const ConstString & _codecType, const mt::uv4f & _uv_image, const mt::uv4f & _uv_alpha, const mt::vec2f & _maxSize )
    {
        this->setCodecType( _codecType );

        if( m_codecType.empty() == true )
        {
            m_codecType = CODEC_SERVICE()
                ->findCodecType( m_filePath );

            if( m_codecType.empty() == true )
            {
                return false;
            }
        }

        this->setMaxSize( _maxSize );
        this->setSize( m_size );
        this->setOffset( mt::vec2f( 0.f, 0.f ) );

        this->setUVImage( _uv_image );
        this->setUVAlpha( _uv_alpha );

        this->setTexture( nullptr );
        this->setTextureAlpha( nullptr );

        this->setAlpha( true );

        this->correctUVTexture();

        if( this->recompile( [this, _imagePath]() { m_filePath = _imagePath; } ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::prepareImageFrame_()
    {
        const RenderImageInterfacePtr & image = m_texture->getImage();

        uint32_t hwChannels = image->getHWChannels();

        if( hwChannels == 3 )
        {
            m_hasAlpha = false;
        }
        else
        {
            m_hasAlpha = true;
        }

        bool pow2 = m_texture->isPow2();

        this->setPow2( pow2 );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::correctUVTexture()
    {
        uint32_t width = (uint32_t)(m_size.x + 0.5f);
        uint32_t height = (uint32_t)(m_size.y + 0.5f);

        if( Helper::isTexturePOW2( width ) == false )
        {
            uint32_t width_pow2 = Helper::getTexturePOW2( width );

            float scale = m_size.x / float( width_pow2 );

            for( uint32_t i = 0; i != 4; ++i )
            {
                m_uvTextureImage[i].x = m_uvImage[i].x * scale;
            }
        }
        else
        {
            for( uint32_t i = 0; i != 4; ++i )
            {
                m_uvTextureImage[i].x = m_uvImage[i].x;
            }
        }

        if( Helper::isTexturePOW2( height ) == false )
        {
            uint32_t height_pow2 = Helper::getTexturePOW2( height );

            float scale = m_size.y / float( height_pow2 );

            for( uint32_t i = 0; i != 4; ++i )
            {
                m_uvTextureImage[i].y = m_uvImage[i].y * scale;
            }
        }
        else
        {
            for( uint32_t i = 0; i != 4; ++i )
            {
                m_uvTextureImage[i].y = m_uvImage[i].y;
            }
        }
    }
}
