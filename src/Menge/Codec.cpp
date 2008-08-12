
#	include "Codec.h"

#	include "ImageCodecPNG.h"
//#	include "ImageCodecJPEG.h"

#	include <cassert>
#	include <algorithm>
#	include <cctype>

#	define REGISTER_CODEC( _class_ )	( Codec::registerCodec( new (_class_) ) )

namespace Menge
{
	std::map< String, Codec* > Codec::ms_mapCodecs;
	//////////////////////////////////////////////////////////////////////////
	Codec::~Codec() 
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TStringVector Codec::getExtensions()
	{
		TStringVector result;
		result.reserve(ms_mapCodecs.size());
		TCodecMap::const_iterator i;
		for (i = ms_mapCodecs.begin(); i != ms_mapCodecs.end(); ++i)
		{
			result.push_back(i->first);
		}
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	Codec * Codec::getCodec( const String& _extension )
	{
		String lwrcase = _extension;
		
		//StringUtil::toLowerCase( lwrcase );
		std::transform(
			lwrcase.begin(),
			lwrcase.end(),
			lwrcase.begin(),
			std::tolower);

		TCodecMap::const_iterator i = ms_mapCodecs.find( lwrcase );
		//assert( ( i != ms_mapCodecs.end() ) &&
		//	String( "Codec::getCodec -> Cannot find codec for extension " + _extension ).c_str() );
		if( i == ms_mapCodecs.end() )
		{
			return NULL;
		}

		return i->second;

	}
	//////////////////////////////////////////////////////////////////////////
	void Codec::initialize()
	{
		REGISTER_CODEC( ImageCodecPNG );
		//REGISTER_CODEC( ImageCodecJPEG );
	}
	//////////////////////////////////////////////////////////////////////////
	void Codec::cleanup()
	{
		for( TCodecMap::iterator it = ms_mapCodecs.begin(), it_end = ms_mapCodecs.end();
			it != it_end;
			it++ )
		{
			delete it->second;
		}
		ms_mapCodecs.clear();
	}
	//////////////////////////////////////////////////////////////////////////
}