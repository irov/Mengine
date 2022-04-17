#include "DevToDebugScriptEmbedding.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonDocumentTraceback.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "DevToDebugTab.h"

#include "DevToDebugPropertyConstBoolean.h"
#include "DevToDebugPropertyGetterBoolean.h"
#include "DevToDebugPropertyConstString.h"
#include "DevToDebugPropertyGetterString.h"
#include "DevToDebugPropertyConstColor.h"
#include "DevToDebugPropertyGetterColor.h"

#include "DevToDebugWidgetText.h"
#include "DevToDebugWidgetButton.h"
#include "DevToDebugWidgetCheckbox.h"

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
        DevToDebugTabPtr s_getDevToDebugTab( const ConstString & _name )
        {
            const DevToDebugTabInterfacePtr & tab = DEVTODEBUG_SERVICE()
                ->getTab( _name );

            return DevToDebugTabPtr::dynamic_from( tab );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_hasDevToDebugTab( const ConstString & _name )
        {
            bool result = DEVTODEBUG_SERVICE()
                ->hasTab( _name );

            return result;
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
            DevToDebugWidgetInterfacePtr text = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ), MENGINE_DOCUMENT_PYBIND );

            text->setId( _id );

            return DevToDebugWidgetTextPtr::dynamic_from( text );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugWidgetButtonPtr s_createDevToDebugWidgetButton( const ConstString & _id )
        {
            DevToDebugWidgetInterfacePtr button = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_PYBIND );

            button->setId( _id );

            return DevToDebugWidgetButtonPtr::dynamic_from( button );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugPropertyInterfacePtr s_createDevToDebugPropertyBoolean( const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property;

            if( _cb.is_callable() == false )
            {
                DevToDebugPropertyConstBooleanPtr property_const = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstBoolean" ), MENGINE_DOCUMENT_PYBIND );

                bool value = _cb.extract();

                property_const->setValue( value );

                property = property_const;
            }
            else
            {
                DevToDebugPropertyGetterBooleanPtr property_getter = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyGetterBoolean" ), MENGINE_DOCUMENT_PYBIND );

                property_getter->setGetter( [_cb, _args]( bool * const _value )
                {
                    bool value = _cb.call_args( _args );

                    *_value = value;
                } );

                property = property_getter;
            }

            return property;
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugPropertyInterfacePtr s_createDevToDebugPropertyString( const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property;

            if( _cb.is_callable() == false )
            {
                DevToDebugPropertyConstStringPtr property_const = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstString" ), MENGINE_DOCUMENT_PYBIND );

                String value = _cb.extract();

                property_const->setValue( value );

                property = property_const;
            }
            else
            {
                DevToDebugPropertyGetterStringPtr property_getter = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyGetterString" ), MENGINE_DOCUMENT_PYBIND );

                property_getter->setGetter( [_cb, _args]( String * const _value )
                {
                    String value = _cb.call_args( _args );

                    *_value = value;
                } );

                property = property_getter;
            }

            return property;
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugPropertyInterfacePtr s_createDevToDebugPropertyColor( const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property;

            if( _cb.is_callable() == false )
            {
                DevToDebugPropertyConstColorPtr property_const = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstColor" ), MENGINE_DOCUMENT_PYBIND );

                Color value = _cb.extract();

                property_const->setValue( value );

                property = property_const;
            }
            else
            {
                DevToDebugPropertyGetterColorPtr property_getter = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyGetterColor" ), MENGINE_DOCUMENT_PYBIND );

                property_getter->setGetter( [_cb, _args]( Color * const _value )
                {
                    Color value = _cb.call_args( _args );

                    *_value = value;
                } );

                property = property_getter;
            }

            return property;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_DevToDebugWidget_setHide( DevToDebugWidget * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::s_createDevToDebugPropertyBoolean( _cb, _args );

            _widget->setBaseProperty( STRINGIZE_STRING_LOCAL( "hide" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_DevToDebugWidgetText_setColor( DevToDebugWidgetText * _text, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::s_createDevToDebugPropertyColor( _cb, _args );

            _text->setDataProperty( STRINGIZE_STRING_LOCAL( "color" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_DevToDebugWidgetText_setText( DevToDebugWidgetText * _text, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::s_createDevToDebugPropertyString( _cb, _args );

            _text->setDataProperty( STRINGIZE_STRING_LOCAL( "content" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_DevToDebugWidgetButton_setTitle( DevToDebugWidgetButton * _button, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::s_createDevToDebugPropertyString( _cb, _args );

            _button->setDataProperty( STRINGIZE_STRING_LOCAL( "title" ), property );
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
        static void s_DevToDebugWidgetCheckbox_setTitle( DevToDebugWidgetCheckbox * _checkbox, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::s_createDevToDebugPropertyString( _cb, _args );

            _checkbox->setDataProperty( STRINGIZE_STRING_LOCAL( "title" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_DevToDebugWidgetCheckbox_setValue( DevToDebugWidgetCheckbox * _checkbox, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::s_createDevToDebugPropertyBoolean( _cb, _args );

            _checkbox->setDataProperty( STRINGIZE_STRING_LOCAL( "value" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_DevToDebugWidgetCheckbox_setClickEvent( DevToDebugWidgetCheckbox * _checkbox, const pybind::object & _cb, const pybind::args & _args )
        {
            _checkbox->setClickEvent( [_cb, _args]( bool _value )
            {
                _cb.call_args( _value, _args );
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
        pybind::def_function( _kernel, "getDevToDebugTab", &Detail::s_getDevToDebugTab );
        pybind::def_function( _kernel, "hasDevToDebugTab", &Detail::s_hasDevToDebugTab );
        pybind::def_function( _kernel, "removeDevToDebugTab", &Detail::s_removeDevToDebugTab );

        pybind::def_function( _kernel, "createDevToDebugWidgetText", &Detail::s_createDevToDebugWidgetText );
        pybind::def_function( _kernel, "createDevToDebugWidgetButton", &Detail::s_createDevToDebugWidgetButton );

        pybind::interface_<DevToDebugTab, pybind::bases<Scriptable>>( _kernel, "DevToDebugTab" )
            .def_static( "addWidget", &Detail::s_DevToDebugTab_addWidget )
            .def_static( "findWidget", &Detail::s_DevToDebugTab_findWidget )
            ;

        pybind::interface_<DevToDebugWidget, pybind::bases<Scriptable>>( _kernel, "DevToDebugWidget" )
            .def( "getId", &DevToDebugWidget::getId )
            .def_static_args( "setHide", &Detail::s_DevToDebugWidget_setHide )
            ;

        pybind::interface_<DevToDebugWidgetText, pybind::bases<DevToDebugWidget>>( _kernel, "DevToDebugWidgetText" )
            .def_static_args( "setColor", &Detail::s_DevToDebugWidgetText_setColor )
            .def_static_args( "setText", &Detail::s_DevToDebugWidgetText_setText )
            ;

        pybind::interface_<DevToDebugWidgetButton, pybind::bases<DevToDebugWidget>>( _kernel, "DevToDebugWidgetButton" )
            .def_static_args( "setTitle", &Detail::s_DevToDebugWidgetButton_setTitle )
            .def_static_args( "setClickEvent", &Detail::s_DevToDebugWidgetButton_setClickEvent )
            ;

        pybind::interface_<DevToDebugWidgetCheckbox, pybind::bases<DevToDebugWidget>>( _kernel, "DevToDebugWidgetCheckbox" )
            .def_static_args( "setTitle", &Detail::s_DevToDebugWidgetCheckbox_setTitle )
            .def_static_args( "setValue", &Detail::s_DevToDebugWidgetCheckbox_setValue )
            .def_static_args( "setClickEvent", &Detail::s_DevToDebugWidgetCheckbox_setClickEvent )
            ;

        Helper::registerScriptWrapping<DevToDebugTab>( _kernel, STRINGIZE_STRING_LOCAL( "DevToDebugTab" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<DevToDebugWidgetText>( _kernel, STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<DevToDebugWidgetButton>( _kernel, STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<DevToDebugWidgetCheckbox>( _kernel, STRINGIZE_STRING_LOCAL( "DevToDebugWidgetCheckbox" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<DevToDebugTab>();
        _kernel->remove_scope<DevToDebugWidget>();
        _kernel->remove_scope<DevToDebugWidgetText>();
        _kernel->remove_scope<DevToDebugWidgetButton>();
        _kernel->remove_scope<DevToDebugWidgetCheckbox>();        

        _kernel->remove_from_module( "addDevToDebugTab", nullptr );
        _kernel->remove_from_module( "removeDevToDebugTab", nullptr );
        _kernel->remove_from_module( "createDevToDebugWidgetText", nullptr );
        _kernel->remove_from_module( "createDevToDebugWidgetButton", nullptr );        
        _kernel->remove_from_module( "createDevToDebugWidgetCheckbox", nullptr );

        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "DevToDebugTab" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "DevToDebugWidgetCheckbox" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}