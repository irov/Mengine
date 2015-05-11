#	pragma once

#	include "Interface/ServiceInterface.h"

#   include "Config/String.h"

namespace Menge
{
	class PluginInterface
	{
	public:
		PluginInterface(){};
		virtual ~PluginInterface(){};

	public:
		virtual bool initialize( ServiceProviderInterface * _provider ) = 0;
		virtual void finalize() = 0;

	public:
		virtual void destroy() = 0;
	};

	typedef bool (*TPluginCreate)( PluginInterface ** _plugin );

    class PluginServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("PluginService")

    public:
        virtual PluginInterface * loadPlugin( const WString & _name ) = 0;
        virtual void unloadPlugin( const WString & _name ) = 0;
    };

#   define PLUGIN_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::PluginServiceInterface)


#	define PLUGIN_DECLARE_STATIC(Name, Type)\
	extern "C"\
	{\
		bool initPlugin##Name( Menge::PluginInterface ** _plugin )\
		{\
			*_plugin = new Type();\
			return true;\
		}\
	}

#	define PLUGIN_DECLARE_DYNAMIC(Name, Type)\
	extern "C"\
	{\
		__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )\
		{\
			return initPlugin##Name( _plugin );\
		}\
	}

#   ifdef MENGE_PLUGIN_DLL
#	define PLUGIN_DECLARE(Name, Type)\
	PLUGIN_DECLARE_STATIC(Name, Type)\
	PLUGIN_DECLARE_DYNAMIC( Name, Type )
#	else
#	define PLUGIN_DECLARE(Name, Type)\
	PLUGIN_DECLARE_STATIC(Name, Type)
#	endif


#	define PLUGIN_EXPORT(Name)\
	extern "C"\
	{\
		extern bool initPlugin##Name( Menge::PluginInterface ** _plugin );\
	}
}