#include "SteamScriptEmbedding.h"

#include "SteamServiceInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool s_steamSetAchievement( const ConstString & _name )
        {
            bool result = STEAM_SERVICE()
                ->setAchievement( _name );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_steamGetAchievement( pybind::kernel_interface * _kernel, const ConstString & _name )
        {
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
        static bool s_steamSetStateInteger( const ConstString & _name, int32_t _value )
        {
            bool result = STEAM_SERVICE()
                ->setStateInteger( _name, _value );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_steamSetStateFloat( const ConstString & _name, float _value )
        {
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
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "Steam", true );

        pybind::def_function( _kernel, "steamSetAchievement", &Detail::s_steamSetAchievement );
        pybind::def_function_kernel( _kernel, "steamGetAchievement", &Detail::s_steamGetAchievement );
        pybind::def_function( _kernel, "steamSetStateInteger", &Detail::s_steamSetStateInteger );
        pybind::def_function( _kernel, "steamSetStateFloat", &Detail::s_steamSetStateFloat );

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
