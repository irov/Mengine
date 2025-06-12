#include "SteamScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/PluginServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "SteamServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool steamSetAchievement( const ConstString & _name )
        {
            if( SERVICE_IS_INITIALIZE( SteamServiceInterface ) == false )
            {
                return false;
            }

            bool result = STEAM_SERVICE()
                ->setAchievement( _name );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool steamGetAchievement( pybind::kernel_interface * _kernel, const ConstString & _name )
        {
            if( SERVICE_IS_INITIALIZE( SteamServiceInterface ) == false )
            {
                return false;
            }

            bool achieved;
            bool result = STEAM_SERVICE()
                ->getAchievement( _name, &achieved );

            if( result == false )
            {
                return _kernel->ret_none();
            }

            return _kernel->ret_bool( achieved );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool steamSetStateInteger( const ConstString & _name, int32_t _value )
        {
            if( SERVICE_IS_INITIALIZE( SteamServiceInterface ) == false )
            {
                return false;
            }

            bool result = STEAM_SERVICE()
                ->setStateInteger( _name, _value );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool steamSetStateFloat( const ConstString & _name, float _value )
        {
            if( SERVICE_IS_INITIALIZE( SteamServiceInterface ) == false )
            {
                return false;
            }

            bool result = STEAM_SERVICE()
                ->setStateFloat( _name, _value );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SteamScriptEmbedding::SteamScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SteamScriptEmbedding::~SteamScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SteamScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function( _kernel, "steamSetAchievement", &Detail::steamSetAchievement );
        pybind::def_function_kernel( _kernel, "steamGetAchievement", &Detail::steamGetAchievement );
        pybind::def_function( _kernel, "steamSetStateInteger", &Detail::steamSetStateInteger );
        pybind::def_function( _kernel, "steamSetStateFloat", &Detail::steamSetStateFloat );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SteamScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "steamSetAchievement", nullptr );
        _kernel->remove_from_module( "steamGetAchievement", nullptr );
        _kernel->remove_from_module( "steamSetStateInteger", nullptr );
        _kernel->remove_from_module( "steamSetStateFloat", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
