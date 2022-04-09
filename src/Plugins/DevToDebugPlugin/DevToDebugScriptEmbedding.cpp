#include "DevToDebugScriptEmbedding.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonDocumentTraceback.h"

#include "DevToDebugTab.h"

#include "Kernel/Scriptable.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        DevToDebugTabPtr s_addDevToDebugTab( const ConstString & _name )
        {
            DevToDebugTabInterfacePtr tab = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugTab" ), MENGINE_DOCUMENT_PYBIND );

            DEVTODEBUG_SERVICE()
                ->addTab( _name, tab );

            return DevToDebugTabPtr::dynamic_from( tab );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_removeDevToDebugTab( const ConstString & _name )
        {
            DEVTODEBUG_SERVICE()
                ->removeTab( _name );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_DevToDebugTab_addText( DevToDebugTab * _tab, const ConstString & _id, bool _hide, const String & _text )
        {
            DevToDebugWidgetTextInterfacePtr text = PROTOTYPE_SERVICE()
                ->generateVirtualInheritancePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ), MENGINE_DOCUMENT_PYBIND );

            text->setId( _id );
            text->setHide( _hide );

            text->setConstText( _text );

            _tab->addWidget( text );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_DevToDebugTab_addButton( DevToDebugTab * _tab, const ConstString & _id, bool _hide, const String & _title, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugWidgetButtonInterfacePtr button = PROTOTYPE_SERVICE()
                ->generateVirtualInheritancePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_PYBIND );

            button->setId( _id );
            button->setHide( _hide );

            button->setTitle( _title );

            button->setClickEvent( [_cb, _args]()
            {
                _cb.call_args( _args );
            } );

            _tab->addWidget( button );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugScriptEmbedding::DevToDebugScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugScriptEmbedding::~DevToDebugScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "DevToDebug", true );

        pybind::def_function( _kernel, "addDevToDebugTab", &Detail::s_addDevToDebugTab );
        pybind::def_function( _kernel, "removeDevToDebugTab", &Detail::s_removeDevToDebugTab );

        pybind::interface_<DevToDebugTab, pybind::bases<Scriptable>>( _kernel, "DevToDebugTab" )
            .def_static( "addText", &Detail::s_DevToDebugTab_addText )
            .def_static_args( "addButton", &Detail::s_DevToDebugTab_addButton )
            ;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<DevToDebugTab>();

        _kernel->remove_from_module( "addDevToDebugTab", nullptr );
        _kernel->remove_from_module( "removeDevToDebugTab", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

