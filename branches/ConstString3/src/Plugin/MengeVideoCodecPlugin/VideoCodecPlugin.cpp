#	include "VideoCodecPlugin.h"
#	include "VideoCodecDecoderSystem.h"

#	include "Utils/Core/File.h"

#	include "Interface/LogSystemInterface.h"

extern "C" // only required if using g++
{
	//////////////////////////////////////////////////////////////////////////
	bool initPluginMengeVideoCodec( Menge::PluginInterface ** _plugin )
	{
		*_plugin = new Menge::VideoCodecPlugin();
		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
	{
		return initPluginMengeVideoCodec( _plugin );
	}
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoCodecPlugin::VideoCodecPlugin()
		: m_codecService(0)
		, m_stringize(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoCodecPlugin::initialize( ServiceProviderInterface * _provider )
	{
		m_codecService = _provider->getServiceT<CodecServiceInterface>( "CodecService" );
		m_stringize = _provider->getServiceT<StringizeServiceInterface>( "StringizeService" );

		if( m_codecService == 0 )
		{
			return false;
		}

		LogServiceInterface * logService = _provider->getServiceT<LogServiceInterface>( "LogService" );
		
		ConstString c_ffmpegVideo =  m_stringize->stringize("ffmpegVideo");

		VideoCodecDecoderSystem * ffmpegDecoder = new VideoCodecDecoderSystem(c_ffmpegVideo, logService);

		ffmpegDecoder->initialize();

		m_decoders.push_back( ffmpegDecoder );
		
		for( TVectorVideoDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{			
			const ConstString & name = (*it)->getName();

			m_codecService->registerDecoder( name, (*it) );
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoCodecPlugin::finalize()
	{
		for( TVectorVideoDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();
			m_codecService->unregisterDecoder( name );

			delete (*it);
		}

		delete this;
	}
}