#	include "CodecManager.h"

#	include "ImageCodecPNG.h"
#	include "ImageCodecJPEG.h"
#	include "ImageCodecMNE.h"
#	include <algorithm>
#	include <cctype>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CodecManager::CodecManager() 
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CodecManager::~CodecManager() 
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecManager::initialize()
	{
		registerCodec(new ImageCodecPNG("png"));
		registerCodec(new ImageCodecJPEG("jpg"));
		registerCodec(new ImageCodecJPEG("jpeg"));
		registerCodec(new ImageCodecJPEG("jfif"));
		registerCodec(new ImageCodecMNE("mne"));
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
	Codec * CodecManager::getCodec( const String& _extension )
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
	void CodecManager::registerCodec( Codec * _codec )
	{
		const String & codecType = _codec->getType();

		TCodecMap::iterator it = ms_mapCodecs.find(codecType);

		if(it != ms_mapCodecs.end())
		{
			return;
		}

		ms_mapCodecs.insert(std::make_pair(codecType, _codec));
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecManager::unregisterCodec( Codec * _codec )
	{
		const String & codecType = _codec->getType();
		ms_mapCodecs.erase(codecType);
	}
	//////////////////////////////////////////////////////////////////////////
}