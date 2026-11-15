#include "iOSOpButtonScriptEmbedding.h"

#import "iOSOpButtonPlugin.h"

#include "Environment/Python/PythonIncluder.h"

@import OpButtonSDK;

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool opButtonIsInitialized()
        {
            if( [OpButtonBridge isInitialized] == NO )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool opButtonAddButton( NSString * _tab, NSString * _identifier, NSString * _title, const pybind::object & _cb, const pybind::args & _args )
        {
            pybind::object callback = _cb;
            pybind::args args = _args;
            void (^handler)(void) = ^{
                callback.call_args( args );
            };

            if( [[iOSOpButtonPlugin sharedInstance] addCommandButton:_tab identifier:_identifier title:_title handler:handler] == NO )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void opButtonRemoveTab( NSString * _tab )
        {
            [[iOSOpButtonPlugin sharedInstance] removeCommandTab:_tab];
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSOpButtonScriptEmbedding::iOSOpButtonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSOpButtonScriptEmbedding::~iOSOpButtonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSOpButtonScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function( _kernel, "opButtonIsInitialized", &Detail::opButtonIsInitialized );
        pybind::def_function_args( _kernel, "opButtonAddButton", &Detail::opButtonAddButton );
        pybind::def_function( _kernel, "opButtonRemoveTab", &Detail::opButtonRemoveTab );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSOpButtonScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "opButtonIsInitialized", nullptr );
        _kernel->remove_from_module( "opButtonAddButton", nullptr );
        _kernel->remove_from_module( "opButtonRemoveTab", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
