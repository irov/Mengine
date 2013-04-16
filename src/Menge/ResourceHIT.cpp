#	include "ResourceHIT.h"

#	include "Kernel/ResourceImplement.h"

#   include "Interface/PickCodecInterface.h"
#	include "Interface/ConverterInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Metacode.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceHIT );
	//////////////////////////////////////////////////////////////////////////
	ResourceHIT::ResourceHIT()
		: m_width(0)
		, m_height(0)
        , m_mipmaplevel(0)
		, m_mipmap(NULL)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ResourceHIT::setCodec( const ConstString & _codec )
    {
        m_codec = _codec;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceHIT::getCodec() const
    {
        return m_codec;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceHIT::setConverterName( const ConstString & _converterName )
    {
        m_converter = _converterName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceHIT::getConverterName() const
    {
        return m_converter;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceHIT::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceHIT * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceHIT *>(_meta);

        metadata->swap_File_Path( m_path );
        metadata->swap_File_Converter( m_converter );
        
        metadata->swap_File_Codec( m_codec );

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceHIT::_convert()
    {
        if( m_path.empty() == true )
        {
            return false;
        }

        if( m_converter.empty() == false )
        {
            if( this->convert_() == false )
            {
                LOGGER_ERROR(m_serviceProvider)("ResourceHIT::_compile: '%s' - hit file convert '%s' not found"
                    , this->getName().c_str()
                    , m_converter.c_str()
                    );

                return false;
            }
        }

        if( m_codec.empty() == true )
        {
            m_codec = this->getCodec_( m_path );
        }

        if( m_codec.empty() == true )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceHIT: '%s' you must determine codec for file '%s'"
                , this->getName().c_str()
                , m_path.c_str()
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceHIT::convert_()
    {
        const ConstString & category = this->getCategory();

        if( CONVERTER_SERVICE(m_serviceProvider)
            ->convert( m_converter, category, m_path, m_path ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceHIT::convert: '%s' can't convert '%s':'%s'"
                , this->getName().c_str() 
                , m_path.c_str()
                , m_converter.c_str()
                );

            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceHIT::_compile()
	{
        const ConstString & category = this->getCategory();

        InputStreamInterface * stream = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( category, m_path );

        if( stream == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceHIT::_compile: '%s' - hit file '%s' not found"
                , this->getName().c_str()
                , m_path.c_str()
                );

            return false;
        }

        PickDecoderInterface * decoder = 
            CODEC_SERVICE(m_serviceProvider)->createDecoderT<PickDecoderInterface>( m_codec, stream );

        if( decoder == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceHIT::_compile: '%s' - hit file '%s' invalid create decoder '%s'"
                , this->getName().c_str()
                , m_path.c_str()
                , m_codec.c_str()
                );

            stream->destroy();

            return NULL;
        }

        const PickCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        m_width = dataInfo->width;
        m_height = dataInfo->height;
        m_mipmaplevel = dataInfo->mipmaplevel;
        
        size_t mipmapsize = dataInfo->mipmapsize;
        m_mipmap = new unsigned char [mipmapsize];
        
        m_mipmapsize = decoder->decode( m_mipmap, mipmapsize );

        decoder->destroy();
        stream->destroy();

        if( m_mipmapsize != mipmapsize )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceHIT::_compile %s invalid decode hit %s size %d (%d)"
                , this->getName().c_str()
                , m_path.c_str()
                , m_mipmapsize
                , mipmapsize
                );

            return false;                
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceHIT::_release()
	{
        delete [] m_mipmap;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceHIT::_isValid() const
    {
        const ConstString & category = this->getCategory();

        bool exist = FILE_SERVICE(m_serviceProvider)
            ->existFile( category, m_path, NULL );

        if( exist == false )
        {
            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void ResourceHIT::setPath( const FilePath& _path )
	{
		m_path = _path;

        this->recompile();
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

		size_t i = (size_t)fi;
		size_t j = (size_t)fj;

		if( i >= m_width || j >= m_height )
		{
			return false;
		}

		unsigned char minAlpha = (unsigned char)(_minAlpha * 255.0f);
		size_t index = j * m_width + i;
		
        unsigned char * alphaBuffer = this->getHitBuffer_( 0 );
		unsigned char alpha = alphaBuffer[index];

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

		size_t i = (size_t)fi;
		size_t j = (size_t)fj;

        size_t level = (size_t)(logf( _radius ) / logf( 2.f ));

        if( level >= m_mipmaplevel )
        {
            level = m_mipmaplevel - 1;
        }

        unsigned char * alphaBuffer = this->getHitBuffer_( level );

        if( alphaBuffer == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceHIT::testRadius %s hit file %s invalid get level buffer %d:%d"
                , this->getName().c_str()
                , m_path.c_str()
                , level
                , m_mipmaplevel
                );

            return false;
        }

        i >>= level;
        j >>= level;

        size_t alphaWidth = m_width >> level;
        size_t alphaHeight = m_height >> level;

        if( i == alphaWidth )
        {
            --i;
        }

        if( j == alphaHeight )
        {
            --j;
        }
		
		unsigned char minAlpha = (unsigned char)(_minAlpha * 255.0f);
		size_t index = i + j * alphaWidth;
				
        unsigned char alpha = alphaBuffer[index];

		if( alpha > minAlpha )
		{
			return true;
		}

		return false;
	}
    //////////////////////////////////////////////////////////////////////////
    unsigned char * ResourceHIT::getHitBuffer_( size_t _level ) const
    {
        size_t bufferOffset = 0;

        for( size_t l = 0; l != _level; ++l )
        {
            size_t offset = (m_width >> l) * (m_height >> l);
            bufferOffset += offset;
        }

        if( bufferOffset >= m_mipmapsize )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceHIT::getHitBuffer_ %s hit file %s invalid get level buffer %d:%d"
                , this->getName().c_str()
                , m_path.c_str()
                , _level
                , m_mipmaplevel
                );

            return NULL;
        }
        
        return m_mipmap + bufferOffset;
    }
	//////////////////////////////////////////////////////////////////////////
    size_t ResourceHIT::getWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ResourceHIT::getHeight() const
    {
        return m_height;
    }
	//////////////////////////////////////////////////////////////////////////
    unsigned char * ResourceHIT::getBuffer() const
    {
        unsigned char * buffer = this->getHitBuffer_( 0 );

        return buffer;
    }
}	// namespace Menge
