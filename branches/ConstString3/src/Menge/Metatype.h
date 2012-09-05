#   pragma once

#	include "ServiceProvider.h"
#	include "Interface/UnicodeInterface.h"

#	include "Utils/Core/ConstString.h"
#	include "Utils/Core/ColourValue.h"

#	include "Config/Typedef.h"

#   include "Math/vec2.h"
#   include "Math/vec3.h"
#   include "Math/vec4.h"

#   include <string>

namespace Metabuf
{
	//////////////////////////////////////////////////////////////////////////
	inline void operator >> ( ArchiveReader & ar, Menge::String & _value )
	{
		size_t size;
		ar.readPOD( size );

		if( size == 0 )
		{
			return;
		}

		_value.resize( size );

		//Archive::value_type * buff = reinterpret_cast<Archive::value_type *>(str);
		ar.readBuffer( &_value[0], size );
	}
	//////////////////////////////////////////////////////////////////////////
	inline void operator >> ( ArchiveReader & ar, Menge::ConstString & _value )
	{
		Menge::String str;
		ar >> str;

		_value = Menge::ConstString(str);
	}
	//////////////////////////////////////////////////////////////////////////
	inline void operator >> ( ArchiveReader & ar, Menge::WString & _value )
	{
		size_t size;
		ar.readPOD( size );

		if( size == 0 )
		{
			return;
		}

		_value.resize( size );

		//Archive::value_type * buff = reinterpret_cast<Archive::value_type *>(str);
        ar.readBuffer( (char *)&_value[0], size * sizeof(Menge::WString::value_type) );
	}
    //////////////////////////////////////////////////////////////////////////
    inline void operator >> ( ArchiveReader & ar, Menge::ColourValue & _value )
    {
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
	inline void operator >> ( ArchiveReader & ar, mt::vec2f & _value )
	{
		ar.readPOD( _value.x );
		ar.readPOD( _value.y );
	}
    //////////////////////////////////////////////////////////////////////////
    inline void operator >> ( ArchiveReader & ar, mt::vec3f & _value )
    {
        ar.readPOD( _value.x );
        ar.readPOD( _value.y );
        ar.readPOD( _value.z );
    }
    //////////////////////////////////////////////////////////////////////////
    inline void operator >> ( ArchiveReader & ar, mt::vec4f & _value )
    {
        ar.readPOD( _value.x );
        ar.readPOD( _value.y );
        ar.readPOD( _value.z );
        ar.readPOD( _value.w );
    }
}