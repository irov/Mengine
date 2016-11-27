#	include "MarmaladeGoogleAdMobModule.h"

#	include "Interface/ConfigInterface.h"
#	include "Interface/GameInterface.h"
#	include "Interface/ApplicationInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/TimerInterface.h"

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
		: m_state(ADMOB_PREPARE)
		, m_Id(0)
		, m_timeShow(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeGoogleAdMobModule::~MarmaladeGoogleAdMobModule()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeGoogleAdMobModule::_initialize()
	{
		if( s3eGoogleAdMobAvailable() == S3E_FALSE )
		{
			return false;
		}

		s3eGoogleAdMobRegister( S3E_GOOGLEADMOB_CALLBACK_AD_LOADED, s_onAdLoad, this );
		s3eGoogleAdMobRegister( S3E_GOOGLEADMOB_CALLBACK_AD_ACTION, s_onAdAction, this );
		s3eGoogleAdMobRegister( S3E_GOOGLEADMOB_CALLBACK_AD_ERROR, s_onAdError, this );

		m_timeShow = TIMER_SERVICE( m_serviceProvider )
			->getMilliseconds();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::_update( bool _focus )
	{
		(void)_focus;

		switch( m_state )
		{
		case ADMOB_PREPARE:
			{
				this->prepareAd_();
			}break;
		case ADMOB_CLOSE:
			{
				this->closeAd_();
			}break;
		case ADMOB_DESTROY:
			{
				this->destroyAd_();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::_message( const ConstString & _messageName, const TMapParams & _params )
	{
		if( _messageName == "Show" )
		{
			this->showAd_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::_messageAll( const ConstString & _messageName, const TMapParams & _params )
	{
		if( _messageName == "onSceneChange" )
		{
			if( m_state != ADMOB_SHOW )
			{
				return;
			}

			uint64_t AutoShowTime = CONFIG_VALUE( m_serviceProvider, "GoogleAdMob", "AutoShowTime", 0U );

			if( AutoShowTime == 0 )
			{
				return;
			}
			
			uint64_t time = TIMER_SERVICE( m_serviceProvider )
				->getMilliseconds();

			if( time - m_timeShow < AutoShowTime )
			{
				return;
			}

			m_timeShow = time;

			this->showAd_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::prepareAd_()
	{
		String adUnitId = CONFIG_VALUE( m_serviceProvider, "GoogleAdMob", "adUnitId", "" );

		LOGGER_WARNING( m_serviceProvider )("MarmaladeGoogleAdMobModule set adUnitId '%s'"
			, adUnitId.c_str()
			);

		if( s3eGoogleAdMobSetAdUnitId( adUnitId.c_str(), S3E_FALSE ) == S3E_RESULT_ERROR )
		{
			const char * adMobErrorMsg = s3eGoogleAdMobGetErrorString();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeGoogleAdMobModule::prepareAd_ invalid set adUnitId error '%s'"
				, adMobErrorMsg
				);

			return;
		}

		if( s3eGoogleAdMobPrepareAd( &m_Id ) == S3E_RESULT_ERROR )
		{
			const char * adMobErrorMsg = s3eGoogleAdMobGetErrorString();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeGoogleAdMobModule::prepareAd_ invalid prepare Ad error '%s'"
				, adMobErrorMsg
				);

			return;
		}

		LOGGER_WARNING( m_serviceProvider )("MarmaladeGoogleAdMobModule prepare Ad '%d'"
			, m_Id
			);

		if( s3eGoogleAdMobLoadInterstitialAd( m_Id ) == S3E_RESULT_ERROR )
		{
			const char * adMobErrorMsg = s3eGoogleAdMobGetErrorString();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeGoogleAdMobModule::prepareAd_ invalid load interstitial Ad error '%s'"
				, adMobErrorMsg
				);

			return;
		}

		m_state = ADMOB_LOAD;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::showAd_()
	{
		if( m_state != ADMOB_SHOW )
		{
			return;
		}

		s3eGoogleAdMobAdInfo info;

		if( s3eGoogleAdMobInspectAd( m_Id, &info ) == S3E_RESULT_ERROR )
		{
			const char * adMobErrorMsg = s3eGoogleAdMobGetErrorString();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeGoogleAdMobModule::showAd_ invalid inspect Ad error '%s'"
				, adMobErrorMsg
				);

			return;
		}

		if( info.m_IsLoading == S3E_TRUE )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeGoogleAdMobModule::showAd_ invalid Ad '%d' is loading..."
				, m_Id
				);

			return;
		}

		if( s3eGoogleAdMobShowAd( m_Id ) == S3E_RESULT_ERROR )
		{
			const char * adMobErrorMsg = s3eGoogleAdMobGetErrorString();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeGoogleAdMobModule::showAd_ invalid show Ad '%d' error '%s'"
				, m_Id
				, adMobErrorMsg
				);

			return;
		}		

		APPLICATION_SERVICE( m_serviceProvider )
			->setFreeze( true );

		m_state = ADMOB_DISMISSED;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::closeAd_()
	{
		APPLICATION_SERVICE( m_serviceProvider )
			->setFreeze( false );

		m_state = ADMOB_DESTROY;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::destroyAd_()
	{
		if( s3eGoogleAdMobDestroyAd( m_Id ) == S3E_RESULT_ERROR )
		{
			const char * adMobErrorMsg = s3eGoogleAdMobGetErrorString();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeGoogleAdMobModule::showAd_ invalid show Ad error '%s'"
				, adMobErrorMsg
				);

			return;
		}

		m_state = ADMOB_PREPARE;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::onAdLoad( s3eGoogleAdMobCallbackLoadedData * _data )
	{
		if( m_Id != _data->m_Id )
		{
			return;
		}

		LOGGER_WARNING( m_serviceProvider )("MarmaladeGoogleAdMobModule::onAdLoad Ad '%d'"
			, m_Id
			);

		TMapParams params;
		GAME_SERVICE( m_serviceProvider )
			->userEvent( STRINGIZE_STRING_LOCAL( m_serviceProvider, "GoogleAdMobLoad" ), params );

		m_state = ADMOB_SHOW;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobModule::onAdAction( s3eGoogleAdMobCallbackActionData * _data )
	{
		//if( m_Id != _data->m_Id )
		//{
		//	return;
		//}

		s3eGoogleAdMobActionType actionType = _data->m_Type;

		LOGGER_WARNING( m_serviceProvider )("MarmaladeGoogleAdMobModule::onAdAction Ad '%d' action '%d'"
			, m_Id
			, actionType
			);

		switch( actionType )
		{
		case S3E_GOOGLEADMOB_ACTION_EXPANDED:
			{				
			} break;
		case S3E_GOOGLEADMOB_ACTION_DISMISSED:
			{
				m_state = ADMOB_CLOSE;
			} break;
		case S3E_GOOGLEADMOB_ACTION_LEFTAP:
			{
				//m_state = ADMOB_CLOSE;
			}break;
		}

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

		LOGGER_ERROR( m_serviceProvider )("MarmaladeGoogleAdMobModule::onAdError Ad '%d' error '%d'"
			, m_Id
			, loadError
			);

		TMapParams params;

		WString wstr_loadError;
		Utils::unsignedToWString( loadError, wstr_loadError );

		params[STRINGIZE_STRING_LOCAL( m_serviceProvider, "error" )] = wstr_loadError;

		GAME_SERVICE( m_serviceProvider )
			->userEvent( STRINGIZE_STRING_LOCAL( m_serviceProvider, "GoogleAdMobError" ), params );
	}
}
