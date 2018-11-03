#include "ResourceImageDefault.h"

#include "Interface/FileSystemInterface.h"
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
        bool result = this->convertDefault_( m_converterType, m_filePath, m_filePath, m_codecType );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefault::_compile()
    {
        LOGGER_INFO( "ResourceImageDefault::loadImageFrame_ %s load texture %s"
            , this->getName().c_str()
            , this->getFilePath().c_str()
        );

        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        if( fileGroup == nullptr )
        {
            LOGGER_ERROR( "ResourceImageDefault::_compile: '%s' group '%s' file path '%s' invalid setup category"
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
            LOGGER_ERROR( "ResourceImageDefault::_compile: '%s' category '%s' group '%s' can't load image file '%s'"
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
        float width_inv = m_texture->getWidthInv();
        float height_inv = m_texture->getHeightInv();

        const RenderImageInterfacePtr & image = m_texture->getImage();

        float hwWidth = (float)image->getHWWidth();
        float hwHeight = (float)image->getHWHeight();

        mt::vec2f uv_unscale;
        uv_unscale.x = hwWidth * width_inv;
        uv_unscale.y = hwHeight * height_inv;

        for( uint32_t i = 0; i != 4; ++i )
        {
            m_uvImage[i] *= uv_unscale;
        }

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

        this->setUVImage( _uv_image );
        this->setUVAlpha( _uv_alpha );

        this->setMaxSize( _maxSize );
        this->setSize( m_size );
        this->setOffset( mt::vec2f( 0.f, 0.f ) );

        this->setTexture( nullptr );
        this->setTextureAlpha( nullptr );

        this->setAlpha( true );

        if( this->recompile( [this, _imagePath]() { m_filePath = _imagePath; } ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::prepareImageFrame_()
    {
        float width = (float)m_texture->getWidth();
        float height = (float)m_texture->getHeight();

        const RenderImageInterfacePtr & image = m_texture->getImage();

        float hwWidthInv = image->getHWWidthInv();
        float hwHeightInv = image->getHWHeightInv();

        mt::vec2f uv_scale;
        uv_scale.x = width * hwWidthInv;
        uv_scale.y = height * hwHeightInv;

        for( uint32_t i = 0; i != 4; ++i )
        {
            m_uvImage[i] *= uv_scale;
        }

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
}
