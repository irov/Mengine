#   include "Metatype.h"

#   include "Interface/UnicodeInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "LoaderEngine.h"

namespace Metabuf
{
	//////////////////////////////////////////////////////////////////////////
    void archive_read( ArchiveReader & ar, Menge::String & _value, void * _userData )
	{
        (void)_userData;

		size_t size;
		ar.readPOD( size );

        if( size == 0 )
        {
            return;
        }

		_value.resize( size );

        ar.readCount( &_value[0], size );
	}
	//////////////////////////////////////////////////////////////////////////
    void archive_read( ArchiveReader & ar, Menge::ConstString & _value, void * _userData )
	{
        Menge::LoaderEngine * loader = static_cast<Menge::LoaderEngine *>(_userData);

		size_t index;
		ar.read( index );

        _value = loader->getCacheConstString( index );
	}
    //////////////////////////////////////////////////////////////////////////
    void archive_read( ArchiveReader & ar, Menge::WChar & _value, void * _userData )
    {
        static Menge::String utf8;
        ar.read( utf8 );

        Menge::LoaderEngine * loader = static_cast<Menge::LoaderEngine *>(_userData);
        Menge::ServiceProviderInterface * serviceProvider = loader->getServiceProvider();

        static Menge::WString unicode;
        Menge::Helper::utf8ToUnicode( serviceProvider, utf8, unicode );

        if( unicode.empty() == true )
        {
            return;
        }

        _value = unicode[0];
    }
	//////////////////////////////////////////////////////////////////////////
    void archive_read( ArchiveReader & ar, Menge::WString & _value, void * _userData )
	{
        static Menge::String utf8;
        ar.read( utf8 );

        Menge::LoaderEngine * loader = static_cast<Menge::LoaderEngine *>(_userData);
        Menge::ServiceProviderInterface * serviceProvider = loader->getServiceProvider();

        Menge::Helper::utf8ToUnicode( serviceProvider, utf8, _value );
	}
    //////////////////////////////////////////////////////////////////////////
    void archive_read( ArchiveReader & ar, Menge::ColourValue & _value, void * _userData )
    {
        (void)_userData;

        float rgba[4];
        ar.readPOD( rgba[0] );
        ar.readPOD( rgba[1] );
        ar.readPOD( rgba[2] );
        ar.readPOD( rgba[3] );

        float coef = 1.f / 255.f;

        rgba[0] *= coef;
        rgba[1] *= coef;
        rgba[2] *= coef;
        rgba[3] *= coef;

        _value.setR( rgba[0] );
        _value.setG( rgba[1] );
        _value.setB( rgba[2] );
        _value.setA( rgba[3] );

        _value.invalidate();
    }
	//////////////////////////////////////////////////////////////////////////
    void archive_read( ArchiveReader & ar, mt::vec2f & _value, void * _userData )
	{
        (void)_userData;

		ar.readPOD( _value.x );
		ar.readPOD( _value.y );
	}
    //////////////////////////////////////////////////////////////////////////
    void archive_read( ArchiveReader & ar, mt::vec3f & _value, void * _userData )
    {
        (void)_userData;

        ar.readPOD( _value.x );
        ar.readPOD( _value.y );
        ar.readPOD( _value.z );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( ArchiveReader & ar, mt::vec4f & _value, void * _userData )
    {
        (void)_userData;

        ar.readPOD( _value.x );
        ar.readPOD( _value.y );
        ar.readPOD( _value.z );
        ar.readPOD( _value.w );
    }
}