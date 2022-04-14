#include "DevToDebugScriptEmbedding.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonDocumentTraceback.h"

#include "DevToDebugTab.h"
#include "DevToDebugWidgetText.h"
#include "DevToDebugWidgetButton.h"

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
        static void s_DevToDebugTab_addWidget( DevToDebugTab * _tab, const DevToDebugWidgetPtr & _widget )
        {
            _tab->addWidget( _widget );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugWidgetPtr s_DevToDebugTab_findWidget( DevToDebugTab * _tab, const ConstString & _id )
        {
            const DevToDebugWidgetInterfacePtr & widget = _tab->findWidget( _id );

            return DevToDebugWidgetPtr::dynamic_from( widget );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugWidgetTextPtr s_createDevToDebugWidgetText( const ConstString & _id )
        {
            DevToDebugWidgetTextInterfacePtr text = PROTOTYPE_SERVICE()
                ->generateVirtualInheritancePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ), MENGINE_DOCUMENT_PYBIND );

            text->setId( _id );

            return DevToDebugWidgetTextPtr::dynamic_from( text );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugWidgetButtonPtr s_createDevToDebugWidgetButton( const ConstString & _id )
        {
            DevToDebugWidgetButtonInterfacePtr button = PROTOTYPE_SERVICE()
                ->generateVirtualInheritancePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_PYBIND );

            button->setId( _id );

            return DevToDebugWidgetButtonPtr::dynamic_from( button );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_DevToDebugWidgetText_setGetterTitle( DevToDebugWidgetText * _text, const pybind::object & _cb, const pybind::args & _args )
        {
            _text->setGetterTitle( [_cb, _args]( String * const _value )
            {
                String value = _cb.call_args( _args );

                *_value = value;
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_DevToDebugWidgetButton_setClickEvent( DevToDebugWidgetButton * _button, const pybind::object & _cb, const pybind::args & _args )
        {
            _button->setClickEvent( [_cb, _args]()
            {
                _cb.call_args( _args );
            } );
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

        pybind::def_function( _kernel, "createDevToDebugWidgetText", &Detail::s_createDevToDebugWidgetText );
        pybind::def_function( _kernel, "createDevToDebugWidgetButton", &Detail::s_createDevToDebugWidgetButton );

        pybind::interface_<DevToDebugTab, pybind::bases<Scriptable>>( _kernel, "DevToDebugTab" )
            .def_static( "addWidget", &Detail::s_DevToDebugTab_addWidget )
            .def_static( "findWidget", &Detail::s_DevToDebugTab_findWidget )
            ;

        pybind::interface_<DevToDebugWidget, pybind::bases<Scriptable>>( _kernel, "DevToDebugWidget" )
            .def( "getId", &DevToDebugWidget::getId )
            .def( "setHide", &DevToDebugWidget::setHide )
            .def( "getHide", &DevToDebugWidget::getHide )
            ;

        pybind::interface_<DevToDebugWidgetText, pybind::bases<DevToDebugWidget>>( _kernel, "DevToDebugWidgetText" )
            .def( "setConstText", &DevToDebugWidgetText::setConstText )
            .def_static_args( "setGetterTitle", &Detail::s_DevToDebugWidgetText_setGetterTitle )
            ;

        pybind::interface_<DevToDebugWidgetButton, pybind::bases<DevToDebugWidget>>( _kernel, "DevToDebugWidgetButton" )
            .def( "setTitle", &DevToDebugWidgetButton::setTitle )
            .def_static_args( "setClickEvent", &Detail::s_DevToDebugWidgetButton_setClickEvent )
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

