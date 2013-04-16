#	pragma once

//#	include <xxbind/xxbind.hpp>

namespace Menge
{
    class ServiceProviderInterface;

	class ScriptWrapper
	{
	public:
		static void entityWrap( ServiceProviderInterface * _serviceProvider );
		static void helperWrap( ServiceProviderInterface * _serviceProvider );
		static void mathWrap( ServiceProviderInterface * _serviceProvider );
		static void nodeWrap( ServiceProviderInterface * _serviceProvider );
        static void constsWrap( ServiceProviderInterface * _serviceProvider );
        static void constsUnwrap( ServiceProviderInterface * _serviceProvider );
		static void soundWrap( ServiceProviderInterface * _serviceProvider );
	};
}
