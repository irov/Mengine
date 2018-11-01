#include "ResourceImageDefault.h"

#include "Interface/FileSystemInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefault::ResourceImageDefault()
        : m_validNoExist( false )
    {
    }
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault::~ResourceImageDefault()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::setFilePath( const FilePath & _filePath )
    {
        m_filePath = _filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceImageDefault::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::setCodecType( const ConstString & _codecType )
    {
        m_codecType = _codecType;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceImageDefault::getCodecType() const
    {
        return m_codecType;
    }
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::setConverterType( const ConstString & _converterType )
	{
		m_converterType = _converterType;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDefault::getConverterType() const
	{
		return m_converterType;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefault::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageDefault * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageDefault *>(_meta);

        m_filePath = metadata->get_File_Path();

        if( metadata->get_File_Codec( &m_codecType ) == false )
        {
            m_codecType = CODEC_SERVICE()
                ->findCodecType( m_filePath );
        }

        metadata->get_File_Converter( &m_converterType );

        m_hasAlpha = true;
        metadata->get_File_Alpha( &m_hasAlpha );

        m_isPremultiply = false;
        metadata->get_File_Premultiply( &m_isPremultiply );

        m_maxSize = metadata->get_File_MaxSize();

        m_size = m_maxSize;
        metadata->get_File_Size( &m_size );
        metadata->get_File_Offset( &m_offset );

        m_validNoExist = false;
        metadata->get_File_NoExist( &m_validNoExist );

        return true;
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
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefault::isValidNoExist() const
    {
        return m_validNoExist;
    }
}
