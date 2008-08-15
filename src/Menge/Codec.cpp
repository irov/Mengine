
#	include "Codec.h"

#	include "ImageCodecPNG.h"
#	include "ImageCodecJPEG.h"
#	include "ImageCodecMNE.h"

#	include <cassert>
#	include <algorithm>
#	include <cctype>

#	define REGISTER_CODEC( _type_, _class_ )	( Codec::registerCodec( (_type_), new (_class_) ) )

namespace Menge
{
	std::map< String, Codec* > Codec::ms_mapCodecs;
	//////////////////////////////////////////////////////////////////////////
	Codec::~Codec() 
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Codec * Codec::getCodec( const String& _extension )
	{
		String lwrcase = _extension;
		
		std::transform(
			lwrcase.begin(),
			lwrcase.end(),
			lwrcase.begin(),
			std::tolower);

		TCodecMap::const_iterator i = ms_mapCodecs.find( lwrcase );
		
		if( i == ms_mapCodecs.end() )
		{
			return NULL;
		}

		return i->second;

	}
	//////////////////////////////////////////////////////////////////////////
	void Codec::initialize()
	{
		REGISTER_CODEC( "png", ImageCodecPNG );
		REGISTER_CODEC( "jpg", ImageCodecJPEG );
		REGISTER_CODEC( "jpeg", ImageCodecJPEG );
		REGISTER_CODEC( "jfif", ImageCodecJPEG );
		REGISTER_CODEC( "mne", ImageCodecMNE );
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