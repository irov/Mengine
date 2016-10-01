#	pragma once

namespace Menge
{
    class ServiceProviderInterface;

	class PythonScriptWrapper
	{
	public:
		static void entityWrap( ServiceProviderInterface * _serviceProvider );
		static void helperWrap( ServiceProviderInterface * _serviceProvider );
		static void mathWrap( ServiceProviderInterface * _serviceProvider );
		static void nodeWrap( ServiceProviderInterface * _serviceProvider );
        static void constsWrap( ServiceProviderInterface * _serviceProvider );
		static void soundWrap( ServiceProviderInterface * _serviceProvider );
	};
}
