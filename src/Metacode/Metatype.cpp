#   include "Metatype.h"

#   include "Interface/ServiceInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#   include "LoaderEngine.h"

namespace Metabuf
{
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, bool & _value, void * _userData )
	{
		(void)_userData;

		uint8_t bool_value;
		ar.readPOD( bool_value );

		_value = !!bool_value;
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, uint8_t & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, uint16_t & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, uint32_t & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, float & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value );
	}
	//////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Menge::String & _value, void * _userData )
	{
        (void)_userData;

		uint32_t size;
		ar.readSize( size );

        if( size == 0 )
        {
            _value.clear();

            return;
        }

		_value.resize( size );

        ar.readPODs( &_value[0], size );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::ConstString & _value, void * _userData )
	{
		Menge::LoaderEngine * loader = static_cast<Menge::LoaderEngine *>(_userData);
		
		uint32_t index;
		ar.readSize( index );

		_value = loader->getCacheConstString( index );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::FilePath & _value, void * _userData )
	{
		Menge::LoaderEngine * loader = static_cast<Menge::LoaderEngine *>(_userData);

		uint32_t index;
		ar.readSize( index );

		const Menge::ConstString & value = loader->getCacheConstString( index );
				
#	ifdef _DEBUG
		const char * test_value = value.c_str();

		const char * s = strstr( test_value, "\\" );

		if( s != nullptr )
		{
			LOGGER_ERROR( "archive_read read FilePath '%s' has invalid slash"
				, test_value
				);

			MENGINE_THROW_EXCEPTION( "archive_read read FilePath '%s' has invalid slash"
				, test_value
			);
		}
#	endif

		_value = Menge::FilePath( value );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::Tags & _value, void * _userData )
	{
		Menge::LoaderEngine * loader = static_cast<Menge::LoaderEngine *>(_userData);

		uint32_t count;
		ar.readSize( count );

		for( uint32_t i = 0; i != count; ++i )
		{
			uint32_t index;
			ar.readSize( index );

			const Menge::ConstString & tag = loader->getCacheConstString( index );

			_value.addTag( tag );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Menge::WChar & _value, void * _userData )
    {
        (void)_userData;

        uint32_t size;
        ar.readSize( size );

        Menge::Char utf8[8];
        ar.readBuffer( (void *)utf8, size );

        size_t unicodeSize;

        Menge::WChar unicode[2];
        if( UNICODE_SYSTEM()
            ->utf8ToUnicode( utf8, size, unicode, 2, &unicodeSize ) == false )
        {
            stdex::throw_memory_reader_exception();

            return;
        }

        if( unicodeSize == 0 )
        {
            stdex::throw_memory_reader_exception();

            return;
        }
        
        _value = unicode[0];
    }
	//////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Menge::WString & _value, void * _userData )
	{
        static Menge::String utf8;
        archive_read( ar, utf8, _userData );

        Menge::Helper::utf8ToUnicode(  utf8, _value );
	}
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Menge::ColourValue & _value, void * _userData )
    {
        (void)_userData;

        float r;
		float g;
		float b;
		float a;
        ar.readPOD( r );
        ar.readPOD( g );
        ar.readPOD( b );
        ar.readPOD( a );

        float coef = 1.f / 255.f;

        r *= coef;
        g *= coef;
        b *= coef;
        a *= coef;

        _value.setRGBA( r, g, b, a );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Menge::Polygon & _value, void * _userData )
    {
        (void)_userData;

        uint32_t count;
        ar.readSize(count);

        if( count % 2 != 0 )
        {
			stdex::throw_memory_reader_exception();

            return;
        }

        for( uint32_t i = 0; i != count; i += 2 )
        {
            mt::vec2f v;
            archive_read( ar, v, _userData );

			_value.append( v );
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::Viewport & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value.begin.x );
		ar.readPOD( _value.begin.y );
		ar.readPOD( _value.end.x );
		ar.readPOD( _value.end.y );
	}
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Menge::Floats & _value, void * _userData )
    {
        (void)_userData;

        uint32_t count;
        ar.readSize(count);

		if( count == 0 )
		{
			_value.clear();

			return;
		}

        _value.resize( count );
		
        ar.readPODs( &_value[0], count );
    }
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::Int8s & _value, void * _userData )
	{
		(void)_userData;

		uint32_t count;
		ar.readSize( count );

		if( count == 0 )
		{
			_value.clear();

			return;
		}

		_value.resize( count );

		ar.readPODs( &_value[0], count );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::Int16s & _value, void * _userData )
	{
		(void)_userData;

		uint32_t count;
		ar.readSize( count );

		if( count == 0 )
		{
			_value.clear();

			return;
		}

		_value.resize( count );

		ar.readPODs( &_value[0], count );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::Int32s & _value, void * _userData )
	{
		(void)_userData;

		uint32_t count;
		ar.readSize( count );

		if( count == 0 )
		{
			_value.clear();

			return;
		}

		_value.resize( count );

		ar.readPODs( &_value[0], count );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::UInt8s & _value, void * _userData )
	{
		(void)_userData;

		uint32_t count;
		ar.readSize( count );

		if( count == 0 )
		{
			_value.clear();

			return;
		}

		_value.resize( count );

		ar.readPODs( &_value[0], count );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::UInt16s & _value, void * _userData )
	{
		(void)_userData;

		uint32_t count;
		ar.readSize( count );

		if( count == 0 )
		{
			_value.clear();

			return;
		}

		_value.resize( count );

		ar.readPODs( &_value[0], count );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::UInt32s & _value, void * _userData )
	{
		(void)_userData;

		uint32_t count;
		ar.readSize( count );

		if( count == 0 )
		{
			_value.clear();

			return;
		}

		_value.resize( count );

		ar.readPODs( &_value[0], count );
	}
	//////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, mt::vec2f & _value, void * _userData )
	{
        (void)_userData;

		ar.readPOD( _value.x );
		ar.readPOD( _value.y );
	}
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, mt::vec3f & _value, void * _userData )
    {
        (void)_userData;

        ar.readPOD( _value.x );
        ar.readPOD( _value.y );
        ar.readPOD( _value.z );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, mt::vec4f & _value, void * _userData )
    {
        (void)_userData;

        ar.readPOD( _value.x );
        ar.readPOD( _value.y );
        ar.readPOD( _value.z );
        ar.readPOD( _value.w );
    }
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, mt::box2f & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value.minimum.x );
		ar.readPOD( _value.minimum.y );
		ar.readPOD( _value.maximum.x );
		ar.readPOD( _value.maximum.y );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, mt::uv4f & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value.p0.x );
		ar.readPOD( _value.p0.y );
		ar.readPOD( _value.p1.x );
		ar.readPOD( _value.p1.y );
		ar.readPOD( _value.p2.x );
		ar.readPOD( _value.p2.y );
		ar.readPOD( _value.p3.x );
		ar.readPOD( _value.p3.y );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::EBlendFactor & _value, void * _userData )
	{
		(void)_userData;

		uint32_t tmp_value;
		ar.readPOD( tmp_value );

		_value = (Menge::EBlendFactor)tmp_value;
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::EBlendOp & _value, void * _userData )
	{
		(void)_userData;

		uint32_t tmp_value;
		ar.readPOD( tmp_value );

		_value = (Menge::EBlendOp)tmp_value;
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::ETextureOp & _value, void * _userData )
	{
		(void)_userData;

		uint32_t tmp_value;
		ar.readPOD( tmp_value );

		_value = (Menge::ETextureOp)tmp_value;
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::ETextureArgument & _value, void * _userData )
	{
		(void)_userData;

		uint32_t tmp_value;
		ar.readPOD( tmp_value );

		_value = (Menge::ETextureArgument)tmp_value;
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::ETextureAddressMode & _value, void * _userData )
	{
		(void)_userData;

		uint32_t tmp_value;
		ar.readPOD( tmp_value );

		_value = (Menge::ETextureAddressMode)tmp_value;
	}
}