
#	include "Codec.h"

#	include "ImageCodecPNG.h"
#	include "ImageCodecJPEG.h"
#	include "ImageCodecMNE.h"

#	include <cassert>
#	include <algorithm>
#	include <cctype>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Codec::TCodecMap Codec::ms_mapCodecs;
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
	void Codec::registerCodec( const String& _type, Codec* _codec )
	{
		TCodecMap::iterator it = ms_mapCodecs.find( _type );

		if(it != ms_mapCodecs.end())
		{
			return;
		}

		ms_mapCodecs.insert(std::make_pair(_type, _codec));
	}
	//////////////////////////////////////////////////////////////////////////
	void Codec::unregisterCodec( const String& _type )
	{
		ms_mapCodecs.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	void Codec::initialize()
	{
		registerCodec( MENGE_TEXT("png"), new ImageCodecPNG() );
		registerCodec( MENGE_TEXT("jpg"), new ImageCodecJPEG() );
		registerCodec( MENGE_TEXT("jpeg"), new ImageCodecJPEG() );
		registerCodec( MENGE_TEXT("jfif"), new ImageCodecJPEG() );
		registerCodec( MENGE_TEXT("mne"), new ImageCodecMNE() );
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