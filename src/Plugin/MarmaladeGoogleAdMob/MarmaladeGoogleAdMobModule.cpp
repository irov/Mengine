#	include "MarmaladeGoogleAdMobModule.h"

#	include "Interface/ConfigInterface.h"
#	include "Interface/GameInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "s3eDebug.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeGoogleAdMobModule::s_onAdLoad( void* systemData, void* userData )
	{
		if( systemData == nullptr || userData == nullptr )
		{
			return 1;
		}

		s3eGoogleAdMobCallbackLoadedData* data = static_cast<s3eGoogleAdMobCallbackLoadedData*>(systemData);

		MarmaladeGoogleAdMobModule * module = static_cast<MarmaladeGoogleAdMobModule*>(userData);

		module->onAdLoad( data );

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeGoogleAdMobModule::s_onAdAction( void* systemData, void* userData )
	{
		if( systemData == nullptr || userData == nullptr )
		{
			return 1;
		}

		s3eGoogleAdMobCallbackActionData * data = static_cast<s3eGoogleAdMobCallbackActionData*>(systemData);

		MarmaladeGoogleAdMobModule * module = static_cast<MarmaladeGoogleAdMobModule*>(userData);

		module->onAdAction( data );

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeGoogleAdMobModule::s_onAdError( void* systemData, void* userData )
	{
		if( systemData == nullptr || userData == nullptr )
		{
			return 1;
		}

		s3eGoogleAdMobCallbackErrorData * data = static_cast<s3eGoogleAdMobCallbackErrorData*>(systemData);

		MarmaladeGoogleAdMobModule * module = static_cast<MarmaladeGoogleAdMobModule*>(userData);

		module->onAdError( data );

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeGoogleAdMobModule::MarmaladeGoogleAdMobModule()
		: m_Id(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeGoogleAdMobModule::~MarmaladeGoogleAdMobModule()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeGoogleAdMobModule::_initialize()
	{
		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, "MarmaladeGoogleAdMobModule::_initialize" );

		if( s3eGoogleAdMobAvailable() == S3E_FALSE )
		{
			return false;
		}

		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, "1" );

		s3eGoogleAdMobRegister( S3E_GOOGLEADMOB_CALLBACK_AD_LOADED, s_onAdLoad, this );
		s3eGoogleAdMobRegister( S3E_GOOGLEADMOB_CALLBACK_AD_ACTION, s_onAdAction, this );
		s3eGoogleAdMobRegister( S3E_GOOGLEADMOB_CALLBACK_AD_ERROR, s_onAdError, this );

		String adUnitId = CONFIG_VALUE( m_serviceProvider, "GoogleAdMob", "adUnitId", "" );

		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, "2" );

		if( s3eGoogleAdMobSetAdUnitId( adUnitId.c_str(), S3E_TRUE ) == S3E_RESULT_SUCCESS )
		{
			return false;
		}

		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, "3" );
		
		if( s3eGoogleAdMobPrepareAd( &m_Id ) == S3E_RESULT_SUCCESS )
		{
			return false;
		}

		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, "4" );

		if( s3eGoogleAdMobLoadAd( m_Id, S3E_FALSE ) == S3E_RESULT_SUCCESS )
		{
			return false;
		}

		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, "5" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::_update( float _time, float _timing )
	{
		(void)_time;
		(void)_timing;

		//bool status = m_userStats->RequestCurrentStats();
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::_message( const ConstString & _messageName, const TMapParams & _params )
	{
		if( _messageName == "Show" )
		{
			s3eGoogleAdMobShowAd( m_Id );
			s3eGoogleAdMobDestroyAd( m_Id );

			s3eGoogleAdMobPrepareAd( &m_Id );
			s3eGoogleAdMobLoadAd( m_Id, S3E_FALSE );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::onAdLoad( s3eGoogleAdMobCallbackLoadedData * _data )
	{
		if( m_Id != _data->m_Id )
		{
			return;
		}

		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, "6" );

		TMapParams params;
		GAME_SERVICE( m_serviceProvider )
			->userEvent( STRINGIZE_STRING_LOCAL( m_serviceProvider, "GoogleAdMobLoad" ), params );

		s3eGoogleAdMobShowAd( m_Id );

		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, "7" );

		s3eGoogleAdMobDestroyAd( m_Id );

		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, "8" );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::onAdAction( s3eGoogleAdMobCallbackActionData * _data )
	{
		if( m_Id != _data->m_Id )
		{
			return;
		}

		s3eGoogleAdMobActionType actionType = _data->m_Type;

		TMapParams params;

		WString wstr_actionType;
		Utils::unsignedToWString( actionType, wstr_actionType );

		params[STRINGIZE_STRING_LOCAL( m_serviceProvider, "type" )] = wstr_actionType;

		GAME_SERVICE( m_serviceProvider )
			->userEvent( STRINGIZE_STRING_LOCAL( m_serviceProvider, "GoogleAdMobAction" ), params );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::onAdError( s3eGoogleAdMobCallbackErrorData * _data )
	{
		if( m_Id != _data->m_Id )
		{
			return;
		}

		s3eGoogleAdMobLoadError loadError = _data->m_Error;

		TMapParams params;

		WString wstr_loadError;
		Utils::unsignedToWString( loadError, wstr_loadError );

		params[STRINGIZE_STRING_LOCAL( m_serviceProvider, "error" )] = wstr_loadError;

		GAME_SERVICE( m_serviceProvider )
			->userEvent( STRINGIZE_STRING_LOCAL( m_serviceProvider, "GoogleAdMobError" ), params );
	}
}
