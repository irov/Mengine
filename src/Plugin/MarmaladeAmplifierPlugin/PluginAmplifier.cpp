#	include "PluginAmplifier.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/StringizeInterface.h"

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool initPluginMengeAmplifier( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::PluginAmplifier();

        return true;
    }
#   ifdef MENGE_PLUGIN_DLL
    ////////////////////////////////////////////////////////////////////////////
    __declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
        return initPluginMengeAmplifier( _plugin );
    }
#   endif
}
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN(Amplifier);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PluginAmplifier::PluginAmplifier()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginAmplifier::_initialize()
	{		
		SERVICE_CREATE( m_serviceProvider, Amplifier );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginAmplifier::_finalize()
	{
		SERVICE_FINALIZE( m_serviceProvider, Menge::AmplifierInterface );
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginAmplifier::_destroy()
	{
		SERVICE_DESTROY( m_serviceProvider, Menge::AmplifierInterface );
	}
}