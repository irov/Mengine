#	pragma once

#	include "Core/ModuleBase.h"

#	include "s3eGoogleAdMob.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	enum MarmaladeGoogleAdMobState
	{
		ADMOB_PREPARE,		
		ADMOB_LOAD,
		ADMOB_SHOW,
		ADMOB_DISMISSED,
		ADMOB_CLOSE,
		ADMOB_DESTROY
	};
	//////////////////////////////////////////////////////////////////////////
	class MarmaladeGoogleAdMobModule
		: public ModuleBase
	{
	public:
		MarmaladeGoogleAdMobModule();
		~MarmaladeGoogleAdMobModule();

	protected:
		bool _avaliable() override;

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		void _update( bool _focus ) override;

	protected:
		void _message( const ConstString & _messageName, const TMapParams & _params ) override;
		void _messageAll( const ConstString & _messageName, const TMapParams & _params ) override;

	protected:
		void prepareAd_();
		void showAd_();
		void closeAd_();
		void destroyAd_();

	protected:
		void onAdLoad( s3eGoogleAdMobCallbackLoadedData * _data );
		void onAdAction( s3eGoogleAdMobCallbackActionData * _data );
		void onAdError( s3eGoogleAdMobCallbackErrorData * _data );

	protected:
		static int32 s_onAdLoad( void* systemData, void* userData );
		static int32 s_onAdAction( void* systemData, void* userData );
		static int32 s_onAdError( void* systemData, void* userData );


	protected:
		MarmaladeGoogleAdMobState m_state;

		uint64_t m_timeShow;

		s3eGoogleAdMobId m_Id;
	};
}