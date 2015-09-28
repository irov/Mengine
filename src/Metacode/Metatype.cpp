#   include "Metatype.h"

#   include "Interface/ServiceInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "LoaderEngine.h"

namespace Metabuf
{
	//////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, bool & _value, void * _userData )
	{
		(void)_userData;

		uint8_t bool_value;
		ar.readPOD( bool_value );

		_value = !!bool_value;
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, uint8_t & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, uint16_t & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, uint32_t & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, float & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value );
	}
	//////////////////////////////////////////////////////////////////////////
    void archive_read( stdex::memory_reader & ar, Menge::String & _value, void * _userData )
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
	void archive_read( stdex::memory_reader & ar, Menge::ConstString & _value, void * _userData )
	{
		Menge::LoaderEngine * loader = static_cast<Menge::LoaderEngine *>(_userData);
		
		uint32_t index;
		ar.readSize( index );

		_value = loader->getCacheConstString( index );
	}
    //////////////////////////////////////////////////////////////////////////
    void archive_read( stdex::memory_reader & ar, Menge::WChar & _value, void * _userData )
    {   
        uint32_t size;
        ar.readSize( size );

        char utf8[32];
        ar.readBuffer( (unsigned char *)utf8, size );

        Menge::LoaderEngine * loader = static_cast<Menge::LoaderEngine *>(_userData);
        Menge::ServiceProviderInterface * serviceProvider = loader->getServiceProvider();

        size_t unicodeSize;

        Menge::WChar unicode[2];
        if( UNICODE_SERVICE(serviceProvider)
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
    void archive_read( stdex::memory_reader & ar, Menge::WString & _value, void * _userData )
	{
        static Menge::String utf8;
        archive_read( ar, utf8, _userData );

        Menge::LoaderEngine * loader = static_cast<Menge::LoaderEngine *>(_userData);
        Menge::ServiceProviderInterface * serviceProvider = loader->getServiceProvider();

        Menge::Helper::utf8ToUnicode( serviceProvider, utf8, _value );
	}
    //////////////////////////////////////////////////////////////////////////
    void archive_read( stdex::memory_reader & ar, Menge::ColourValue & _value, void * _userData )
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

        _value.setARGB( a, r, g, b );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( stdex::memory_reader & ar, Menge::Polygon & _value, void * _userData )
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

            boost::geometry::append( _value, v );
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, Menge::Viewport & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value.begin.x );
		ar.readPOD( _value.begin.y );
		ar.readPOD( _value.end.x );
		ar.readPOD( _value.end.y );
	}
    //////////////////////////////////////////////////////////////////////////
    void archive_read( stdex::memory_reader & ar, Menge::Floats & _value, void * _userData )
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
    void archive_read( stdex::memory_reader & ar, mt::vec2f & _value, void * _userData )
	{
        (void)_userData;

		ar.readPOD( _value.x );
		ar.readPOD( _value.y );
	}
    //////////////////////////////////////////////////////////////////////////
    void archive_read( stdex::memory_reader & ar, mt::vec3f & _value, void * _userData )
    {
        (void)_userData;

        ar.readPOD( _value.x );
        ar.readPOD( _value.y );
        ar.readPOD( _value.z );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( stdex::memory_reader & ar, mt::vec4f & _value, void * _userData )
    {
        (void)_userData;

        ar.readPOD( _value.x );
        ar.readPOD( _value.y );
        ar.readPOD( _value.z );
        ar.readPOD( _value.w );
    }
	//////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, mt::box2f & _value, void * _userData )
	{
		(void)_userData;

		ar.readPOD( _value.minimum.x );
		ar.readPOD( _value.minimum.y );
		ar.readPOD( _value.maximum.x );
		ar.readPOD( _value.maximum.y );
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, mt::uv4f & _value, void * _userData )
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
	void archive_read( stdex::memory_reader & ar, Menge::EBlendFactor & _value, void * _userData )
	{
		(void)_userData;

		uint32_t tmp_value;
		ar.readPOD( tmp_value );

		_value = (Menge::EBlendFactor)tmp_value;
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, Menge::ETextureOp & _value, void * _userData )
	{
		(void)_userData;

		uint32_t tmp_value;
		ar.readPOD( tmp_value );

		_value = (Menge::ETextureOp)tmp_value;
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, Menge::ETextureArgument & _value, void * _userData )
	{
		(void)_userData;

		uint32_t tmp_value;
		ar.readPOD( tmp_value );

		_value = (Menge::ETextureArgument)tmp_value;
	}
	//////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, Menge::ETextureAddressMode & _value, void * _userData )
	{
		(void)_userData;

		uint32_t tmp_value;
		ar.readPOD( tmp_value );

		_value = (Menge::ETextureAddressMode)tmp_value;
	}
}