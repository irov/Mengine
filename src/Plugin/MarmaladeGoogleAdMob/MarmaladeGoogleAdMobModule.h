#	pragma once

#	include "Core/ModuleBase.h"

#	include "s3eGoogleAdMob.h"

namespace Menge
{
	class MarmaladeGoogleAdMobModule
		: public ModuleBase
	{
	public:
		MarmaladeGoogleAdMobModule();
		~MarmaladeGoogleAdMobModule();

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		void _update( float _time, float _timing ) override;

	protected:
		void _message( const ConstString & _messageName, const TMapParams & _params ) override;

	protected:
		void onAdLoad( s3eGoogleAdMobCallbackLoadedData * _data );
		void onAdAction( s3eGoogleAdMobCallbackActionData * _data );
		void onAdError( s3eGoogleAdMobCallbackErrorData * _data );

	protected:
		static int32 s_onAdLoad( void* systemData, void* userData );
		static int32 s_onAdAction( void* systemData, void* userData );
		static int32 s_onAdError( void* systemData, void* userData );


	protected:
		s3eGoogleAdMobId m_Id;
	};
}