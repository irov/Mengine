
#	include "Codec.h"

#	include <cassert>
#	include <algorithm>
#	include <cctype>

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
	Codec* Codec::getCodec( const String& _extension )
	{
		String lwrcase = _extension;
		
		//StringUtil::toLowerCase( lwrcase );
		std::transform(
			lwrcase.begin(),
			lwrcase.end(),
			lwrcase.begin(),
			std::tolower);

		TCodecMap::const_iterator i = ms_mapCodecs.find( lwrcase );
		assert( ( i != ms_mapCodecs.end() ) &&
			String( "Codec::getCodec -> Cannot find codec for extension " + _extension ).c_str() );

		return i->second;

	}
	//////////////////////////////////////////////////////////////////////////
}