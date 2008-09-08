
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
	Codec::~Codec() 
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CodecManager::TCodecMap CodecManager::ms_mapCodecs;
	//////////////////////////////////////////////////////////////////////////
	Codec* CodecManager::getCodec( const StringA& _type )
	{
		StringA lwrcase = _type;
		
		std::transform(
			lwrcase.begin(),
			lwrcase.end(),
			lwrcase.begin(),
			std::tolower);

		TCodecMap::const_iterator i = ms_mapCodecs.find( lwrcase );
		
		if( i == ms_mapCodecs.end() )
		{
			return 0;
		}

		return i->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecManager::registerCodec( const StringA& _type, Codec* _codec )
	{
		TCodecMap::iterator it = ms_mapCodecs.find( _type );

		if(it != ms_mapCodecs.end())
		{
			return;
		}

		ms_mapCodecs.insert(std::make_pair(_type, _codec));
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecManager::unregisterCodec( const StringA& _type )
	{
		ms_mapCodecs.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecManager::initialize()
	{
		registerCodec( "png", new ImageCodecPNG() );
		registerCodec( "jpg", new ImageCodecJPEG() );
		registerCodec( "jpeg", new ImageCodecJPEG() );
		registerCodec( "jfif", new ImageCodecJPEG() );
		registerCodec( "mne", new ImageCodecMNE() );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecManager::cleanup()
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