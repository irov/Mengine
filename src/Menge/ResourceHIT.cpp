#	include "ResourceHIT.h"

#   include "Interface/PickCodecInterface.h"
#	include "Interface/ConverterInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Metacode/Metacode.h"

#	include "Logger/Logger.h"

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceHIT::ResourceHIT()
		: m_width(0)
		, m_height(0)
        , m_mipmaplevel(0)
		, m_mipmapsize(0)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ResourceHIT::setCodecType( const ConstString & _codec )
    {
        m_codecType = _codec;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceHIT::getCodecType() const
    {
        return m_codecType;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceHIT::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceHIT * metadata 
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceHIT *>(_meta);

        m_filePath = metadata->get_File_Path();
        metadata->get_File_Converter( &m_converterType );        
        metadata->get_File_Codec( &m_codecType );

        return true;
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
        const ConstString & category = this->getCategory();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( category, m_filePath, false );

        if( stream == nullptr )
        {
            LOGGER_ERROR("ResourceHIT::_compile: '%s' - hit file '%s' not found"
                , this->getName().c_str()
                , m_filePath.c_str()
                );

            return false;
        }

        PickDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<PickDecoderInterfacePtr>( m_codecType );

        if( decoder == nullptr )
        {
            LOGGER_ERROR("ResourceHIT::_compile: '%s' - hit file '%s' invalid create decoder '%s'"
                , this->getName().c_str()
                , m_filePath.c_str()
                , m_codecType.c_str()
                );

            return false;
        }

		if( decoder->prepareData( stream ) == false )
		{
			LOGGER_ERROR("ResourceHIT::_compile: '%s' - hit file '%s' invalid initialize decoder '%s'"
				, this->getName().c_str()
				, m_filePath.c_str()
				, m_codecType.c_str()
				);

			return false;
		}

        const PickCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        m_width = dataInfo->width;
        m_height = dataInfo->height;
        m_mipmaplevel = dataInfo->mipmaplevel;
        		
		MemoryInterfacePtr mipmap = MEMORY_SERVICE()
			->createMemory();

		if( mipmap == nullptr )
		{
			LOGGER_ERROR("ResourceHIT::_compile: '%s' - hit file '%s' invalid create memory"
				, this->getName().c_str()
				, m_filePath.c_str()
				);

			return false;
		}
		
		size_t mipmapsize = (size_t)dataInfo->mipmapsize;
		void * buffer = mipmap->newMemory( mipmapsize, __FILE__, __LINE__ );

		if( buffer == nullptr )
		{
			LOGGER_ERROR("ResourceHIT::_compile: '%s' - hit file '%s' invalid new memory '%u'"
				, this->getName().c_str()
				, m_filePath.c_str()
				, mipmapsize
				);

			return false;
		}
        
		size_t test_mipmapsize = decoder->decode( buffer, mipmapsize );

		if( test_mipmapsize != mipmapsize )
        {
            LOGGER_ERROR("ResourceHIT::_compile %s invalid decode hit %s size %d (%d)"
                , this->getName().c_str()
                , m_filePath.c_str()
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
    bool ResourceHIT::_isValid() const
    {
        const ConstString & category = this->getCategory();

        bool exist = FILE_SERVICE()
            ->existFile( category, m_filePath, nullptr );

        if( exist == false )
        {
            return false;
        }

		InputStreamInterfacePtr stream = FILE_SERVICE()
			->openInputFile( category, m_filePath, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR("ResourceHIT::_isValid %s invalid open file %s:%s"
				, m_name.c_str()
				, category.c_str()
				, m_filePath.c_str()
				);

			return false;
		}

		PickDecoderInterfacePtr decoder = CODEC_SERVICE()
			->createDecoderT<PickDecoderInterfacePtr>( m_codecType );

		if( decoder == nullptr )
		{
			LOGGER_ERROR("ResourceHIT::_isValid %s file %s:%s invalid decoder %s"
				, m_name.c_str()
				, category.c_str()
				, m_filePath.c_str()
				, m_codecType.c_str()
				);

			return false;
		}

		if( decoder->prepareData( stream ) == false )
		{
			LOGGER_ERROR("ResourceHIT::_isValid %s file %s:%s decoder initialize failed %s"
				, m_name.c_str()
				, category.c_str()
				, m_filePath.c_str()
				, m_codecType.c_str()
				);

			return false;
		}

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void ResourceHIT::setPath( const FilePath & _filePath )
	{
		m_filePath = _filePath;

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

		uint32_t i = (uint32_t)fi;
		uint32_t j = (uint32_t)fj;

		if( i >= m_width || j >= m_height )
		{
			return false;
		}

		uint8_t minAlpha = (uint8_t)(_minAlpha * 255.0f);
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
            LOGGER_ERROR("ResourceHIT::testRadius %s hit file %s invalid get level buffer %d:%d"
                , this->getName().c_str()
                , m_filePath.c_str()
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
		
		uint8_t minAlpha = (uint8_t)(_minAlpha * 255.0f);
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
            LOGGER_ERROR("ResourceHIT::getHitBuffer_ %s hit file %s invalid get level buffer %d:%d"
                , this->getName().c_str()
                , m_filePath.c_str()
                , _level
                , m_mipmaplevel
                );

            return nullptr;
        }

		uint8_t * buffer_memory = m_mipmap->getMemory();

		uint8_t * buffer = buffer_memory + bufferOffset;
        
        return buffer;
    }
	//////////////////////////////////////////////////////////////////////////
    uint32_t ResourceHIT::getWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceHIT::getHeight() const
    {
        return m_height;
    }
	//////////////////////////////////////////////////////////////////////////
	uint8_t * ResourceHIT::getBuffer() const
    {
		uint8_t * buffer = this->getHitBuffer_( 0 );

        return buffer;
    }
}	// namespace Menge
