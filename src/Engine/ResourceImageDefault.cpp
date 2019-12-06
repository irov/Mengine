#include "ResourceImageDefault.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/ConstString.h"
#include "Kernel/AssertionMemoryPanic.h"

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
    bool ResourceImageDefault::_compile()
    {
        if( ResourceImage::_compile() == false )
        {
            LOGGER_ERROR( "'%s' invalid compile base ResourceImage class"
                , this->getName().c_str()
            );

            return false;
        }

        LOGGER_INFO( "name '%s' load texture '%s'"
            , this->getName().c_str()
            , this->getFilePath().c_str()
        );

        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, false, "name '%s' group '%s' file path '%s' invalid setup category"
            , this->getName().c_str()
            , this->getGroupName().c_str()
            , this->getFilePath().c_str()
        );

        const FilePath & filePath = this->getFilePath();
        const ConstString & codecType = this->getCodecType();

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->loadTexture( fileGroup, filePath, codecType, DF_NONE, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( texture, false, "name '%s' category '%s' group '%s' can't load image file '%s'"
            , this->getName().c_str()
            , this->getFileGroup()->getName().c_str()
            , this->getGroupName().c_str()
            , this->getFilePath().c_str()
        );

        m_texture = texture;

        this->prepareImageFrame_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefault::setup( const FilePath & _imagePath, const ConstString & _codecType, const mt::uv4f & _uv_image, const mt::uv4f & _uv_alpha, const mt::vec2f & _maxSize )
    {
        if( _codecType.empty() == true )
        {
            const FilePath & filePath = this->getFilePath();

            const ConstString & newCodecType = CODEC_SERVICE()
                ->findCodecType( filePath );

            if( newCodecType.empty() == true )
            {
                return false;
            }

            this->setCodecType( newCodecType );
        }
        else
        {
            this->setCodecType( _codecType );
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

        bool successful = this->recompile( [this, &_imagePath]()
        {
            this->setFilePath( _imagePath );

            if( _imagePath.empty() == true )
            {
                return false;
            }

            return true;
        } );

        return successful;
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
                m_uvTextureAlpha[i].x = m_uvAlpha[i].x * scale;
            }
        }
        else
        {
            for( uint32_t i = 0; i != 4; ++i )
            {
                m_uvTextureImage[i].x = m_uvImage[i].x;
                m_uvTextureAlpha[i].x = m_uvAlpha[i].x;
            }
        }

        if( Helper::isTexturePOW2( height ) == false )
        {
            uint32_t height_pow2 = Helper::getTexturePOW2( height );

            float scale = m_size.y / float( height_pow2 );

            for( uint32_t i = 0; i != 4; ++i )
            {
                m_uvTextureImage[i].y = m_uvImage[i].y * scale;
                m_uvTextureAlpha[i].y = m_uvAlpha[i].y * scale;
            }
        }
        else
        {
            for( uint32_t i = 0; i != 4; ++i )
            {
                m_uvTextureImage[i].y = m_uvImage[i].y;
                m_uvTextureAlpha[i].y = m_uvAlpha[i].y;
            }
        }
    }
}
