#include "Win32OpButtonScriptEmbedding.h"

#include "Win32OpButtonInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/ConstString.h"

#include "OpButton/OpButtonSdk.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool opButtonIsInitialized()
        {
            if( OpButton::OpButtonSdk::isInitialized() == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool opButtonAddButton( const ConstString & _tab, const ConstString & _identifier, const ConstString & _title, const pybind::object & _cb )
        {
            pybind::object callback = _cb;
            Lambda<void()> handler = [callback]() {
                callback.call();
            };

            if( WIN32_OPBUTTON_SERVICE()
                ->addCommandButton( _tab, _identifier, _title, handler ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void opButtonRemoveTab( const ConstString & _tab )
        {
            WIN32_OPBUTTON_SERVICE()
                ->removeCommandTab( _tab );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Win32OpButtonScriptEmbedding::Win32OpButtonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32OpButtonScriptEmbedding::~Win32OpButtonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32OpButtonScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function( _kernel, "opButtonIsInitialized", &Detail::opButtonIsInitialized );
        pybind::def_function( _kernel, "opButtonAddButton", &Detail::opButtonAddButton );
        pybind::def_function( _kernel, "opButtonRemoveTab", &Detail::opButtonRemoveTab );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "opButtonIsInitialized", nullptr );
        _kernel->remove_from_module( "opButtonAddButton", nullptr );
        _kernel->remove_from_module( "opButtonRemoveTab", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
