#include "DevToDebugScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/PluginServiceInterface.h"

#include "Environment/Python/PythonDocument.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "DevToDebugTab.h"

#include "DevToDebugPropertyConstBoolean.h"
#include "DevToDebugPropertyGetterBoolean.h"
#include "DevToDebugPropertyConstString.h"
#include "DevToDebugPropertyGetterString.h"
#include "DevToDebugPropertyConstColor.h"
#include "DevToDebugPropertyGetterColor.h"
#include "DevToDebugPropertyContent.h"


#include "DevToDebugWidgetText.h"
#include "DevToDebugWidgetButton.h"
#include "DevToDebugWidgetCheckbox.h"
#include "DevToDebugWidgetCommandLine.h"
#include "DevToDebugWidgetRadioButton.h"
#include "DevToDebugWidgetSelector.h"

#include "Kernel/Scriptable.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugTabPtr addDevToDebugTab( const ConstString & _name )
        {
            DevToDebugTabInterfacePtr tab = Helper::generatePrototype<DevToDebugTab>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

            if( tab->initialize() == false )
            {
                return nullptr;
            }

            DEVTODEBUG_SERVICE()
                ->addTab( _name, tab );

            return DevToDebugTabPtr::dynamic_from( tab );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugTabPtr getDevToDebugTab( const ConstString & _name )
        {
            const DevToDebugTabInterfacePtr & tab = DEVTODEBUG_SERVICE()
                ->getTab( _name );

            return DevToDebugTabPtr::dynamic_from( tab );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool hasDevToDebugTab( const ConstString & _name )
        {
            bool result = DEVTODEBUG_SERVICE()
                ->hasTab( _name );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static void removeDevToDebugTab( const ConstString & _name )
        {
            DEVTODEBUG_SERVICE()
                ->removeTab( _name );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugTab_addWidget( DevToDebugTab * _tab, const DevToDebugWidgetPtr & _widget )
        {
            if( _widget == nullptr )
            {
                LOGGER_ERROR( "DevToDebugTab invalid add widget [nullptr]" );

                return;
            }

            _tab->addWidget( _widget );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugWidgetPtr DevToDebugTab_findWidget( DevToDebugTab * _tab, const ConstString & _id )
        {
            const DevToDebugWidgetInterfacePtr & widget = _tab->findWidget( _id );

            return DevToDebugWidgetPtr::dynamic_from( widget );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugWidgetTextPtr createDevToDebugWidgetText( const ConstString & _id )
        {
            DevToDebugWidgetInterfacePtr widget = Helper::generatePrototype<DevToDebugWidgetText>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

            widget->setWidgetId( _id );

            return DevToDebugWidgetTextPtr::dynamic_from( widget );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugWidgetButtonPtr createDevToDebugWidgetButton( const ConstString & _id )
        {
            DevToDebugWidgetInterfacePtr widget = Helper::generatePrototype<DevToDebugWidgetButton>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

            widget->setWidgetId( _id );

            return DevToDebugWidgetButtonPtr::dynamic_from( widget );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugWidgetCheckboxPtr createDevToDebugWidgetCheckbox( const ConstString & _id )
        {
            DevToDebugWidgetInterfacePtr widget = Helper::generatePrototype<DevToDebugWidgetCheckbox>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

            widget->setWidgetId( _id );

            return DevToDebugWidgetCheckboxPtr::dynamic_from( widget );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugWidgetCommandLinePtr createDevToDebugWidgetCommandLine( const ConstString & _id )
        {
            DevToDebugWidgetInterfacePtr widget = Helper::generatePrototype<DevToDebugWidgetCommandLine>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

            widget->setWidgetId( _id );

            return DevToDebugWidgetCommandLinePtr::dynamic_from( widget );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugWidgetRadioButtonPtr createDevToDebugWidgetRadioButton( const ConstString & _id )
        {
            DevToDebugWidgetInterfacePtr widget = Helper::generatePrototype<DevToDebugWidgetRadioButton>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

            widget->setWidgetId( _id );

            DevToDebugPropertyContentPtr property_content = Helper::generatePrototype<DevToDebugPropertyContent>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

            widget->setDataProperty( STRINGIZE_STRING_LOCAL( "content" ), property_content );

            return DevToDebugWidgetRadioButtonPtr::dynamic_from( widget );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugWidgetSelectorPtr createDevToDebugWidgetSelector( const ConstString & _id )
        {
            DevToDebugWidgetInterfacePtr widget = Helper::generatePrototype<DevToDebugWidgetSelector>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

            widget->setWidgetId( _id );

            DevToDebugPropertyContentPtr property_content = Helper::generatePrototype<DevToDebugPropertyContent>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

            widget->setDataProperty( STRINGIZE_STRING_LOCAL( "content" ), property_content );

            return DevToDebugWidgetSelectorPtr::dynamic_from( widget );
        }
        //////////////////////////////////////////////////////////////////////////
        static DevToDebugPropertyInterfacePtr createDevToDebugPropertyBoolean( const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property;

            if( _cb.is_callable() == false )
            {
                DevToDebugPropertyConstBooleanPtr property_const = Helper::generatePrototype<DevToDebugPropertyConstBoolean>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

                bool value = _cb.extract();

                property_const->setValue( value );

                property = property_const;
            }
            else
            {
                DevToDebugPropertyGetterBooleanPtr property_getter = Helper::generatePrototype<DevToDebugPropertyGetterBoolean>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

                if( property_getter->initialize() == false )
                {
                    return nullptr;
                }

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
        static DevToDebugPropertyInterfacePtr createDevToDebugPropertyString( const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property;

            if( _cb.is_callable() == false )
            {
                DevToDebugPropertyConstStringPtr property_const = Helper::generatePrototype<DevToDebugPropertyConstString>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

                String value = _cb.extract();

                property_const->setValue( value );

                property = property_const;
            }
            else
            {
                DevToDebugPropertyGetterStringPtr property_getter = Helper::generatePrototype<DevToDebugPropertyGetterString>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

                if( property_getter->initialize() == false )
                {
                    return nullptr;
                }

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
        static DevToDebugPropertyInterfacePtr createDevToDebugPropertyColor( const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property;

            if( _cb.is_callable() == false )
            {
                DevToDebugPropertyConstColorPtr property_const = Helper::generatePrototype<DevToDebugPropertyConstColor>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

                Color value = _cb.extract();

                property_const->setValue( value );

                property = property_const;
            }
            else
            {
                DevToDebugPropertyGetterColorPtr property_getter = Helper::generatePrototype<DevToDebugPropertyGetterColor>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_PYTHON );

                if( property_getter->initialize() == false )
                {
                    return nullptr;
                }

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
        static void DevToDebugWidget_setHide( DevToDebugWidget * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::createDevToDebugPropertyBoolean( _cb, _args );

            _widget->setBaseProperty( STRINGIZE_STRING_LOCAL( "hide" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetText_setColor( DevToDebugWidgetText * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::createDevToDebugPropertyColor( _cb, _args );

            _widget->setDataProperty( STRINGIZE_STRING_LOCAL( "color" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetText_setText( DevToDebugWidgetText * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::createDevToDebugPropertyString( _cb, _args );

            _widget->setDataProperty( STRINGIZE_STRING_LOCAL( "content" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetButton_setTitle( DevToDebugWidgetButton * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::createDevToDebugPropertyString( _cb, _args );

            _widget->setDataProperty( STRINGIZE_STRING_LOCAL( "title" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetButton_setClickEvent( DevToDebugWidgetButton * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            _widget->setClickEvent( [_cb, _args]()
            {
                _cb.call_args( _args );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetCheckbox_setTitle( DevToDebugWidgetCheckbox * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::createDevToDebugPropertyString( _cb, _args );

            _widget->setDataProperty( STRINGIZE_STRING_LOCAL( "text" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetCheckbox_setValue( DevToDebugWidgetCheckbox * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::createDevToDebugPropertyBoolean( _cb, _args );

            _widget->setDataProperty( STRINGIZE_STRING_LOCAL( "value" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetCheckbox_setChangeEvent( DevToDebugWidgetCheckbox * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            _widget->setChangeEvent( [_cb, _args]( bool _value )
            {
                _cb.call_args( _value, _args );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetCommandLine_setTitle( DevToDebugWidgetCommandLine * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::createDevToDebugPropertyString( _cb, _args );

            _widget->setDataProperty( STRINGIZE_STRING_LOCAL( "title" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetCommandLine_setPlaceholder( DevToDebugWidgetCommandLine * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::createDevToDebugPropertyString( _cb, _args );

            _widget->setDataProperty( STRINGIZE_STRING_LOCAL( "placeholder" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetCommandLine_setCommandEvent( DevToDebugWidgetCommandLine * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            _widget->setCommandEvent( [_cb, _args]( const String & _value )
            {
                _cb.call_args( _value, _args );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetRadioButton_setTitle( DevToDebugWidgetRadioButton * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::createDevToDebugPropertyString( _cb, _args );

            _widget->setDataProperty( STRINGIZE_STRING_LOCAL( "title" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetRadioButton_addState( DevToDebugWidgetRadioButton * _widget, const ConstString & _id, const String & _value )
        {
            const DevToDebugPropertyInterfacePtr & property = _widget->getDataProperty( STRINGIZE_STRING_LOCAL( "content" ) );

            UnknownDevToDebugPropertyContentInterface * content = property->getUnknown();

            content->addState( _id, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetRadioButton_setChangeEvent( DevToDebugWidgetRadioButton * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            _widget->setChangeEvent( [_cb, _args]( const ConstString & _value )
            {
                _cb.call_args( _value, _args );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetSelector_setTitle( DevToDebugWidgetSelector * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            DevToDebugPropertyInterfacePtr property = Detail::createDevToDebugPropertyString( _cb, _args );

            _widget->setDataProperty( STRINGIZE_STRING_LOCAL( "title" ), property );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetSelector_addState( DevToDebugWidgetSelector * _widget, const ConstString & _id, const String & _value )
        {
            const DevToDebugPropertyInterfacePtr & property = _widget->getDataProperty( STRINGIZE_STRING_LOCAL( "content" ) );

            UnknownDevToDebugPropertyContentInterface * content = property->getUnknown();

            content->addState( _id, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        static void DevToDebugWidgetSelector_setChangeEvent( DevToDebugWidgetSelector * _widget, const pybind::object & _cb, const pybind::args & _args )
        {
            _widget->setChangeEvent( [_cb, _args]( const ConstString & _value )
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
        pybind::def_function( _kernel, "addDevToDebugTab", &Detail::addDevToDebugTab );
        pybind::def_function( _kernel, "getDevToDebugTab", &Detail::getDevToDebugTab );
        pybind::def_function( _kernel, "hasDevToDebugTab", &Detail::hasDevToDebugTab );
        pybind::def_function( _kernel, "removeDevToDebugTab", &Detail::removeDevToDebugTab );

        pybind::def_function( _kernel, "createDevToDebugWidgetText", &Detail::createDevToDebugWidgetText );
        pybind::def_function( _kernel, "createDevToDebugWidgetButton", &Detail::createDevToDebugWidgetButton );
        pybind::def_function( _kernel, "createDevToDebugWidgetCheckbox", &Detail::createDevToDebugWidgetCheckbox );
        pybind::def_function( _kernel, "createDevToDebugWidgetCommandLine", &Detail::createDevToDebugWidgetCommandLine );
        pybind::def_function( _kernel, "createDevToDebugWidgetRadioButton", &Detail::createDevToDebugWidgetRadioButton );
        pybind::def_function( _kernel, "createDevToDebugWidgetSelector", &Detail::createDevToDebugWidgetSelector );

        pybind::interface_<DevToDebugTab, pybind::bases<Scriptable>>( _kernel, "DevToDebugTab" )
            .def_static( "addWidget", &Detail::DevToDebugTab_addWidget )
            .def_static( "findWidget", &Detail::DevToDebugTab_findWidget )
            ;

        pybind::interface_<DevToDebugWidget, pybind::bases<Scriptable>>( _kernel, "DevToDebugWidget" )
            .def( "getWidgetId", &DevToDebugWidget::getWidgetId )
            .def_static_args( "setHide", &Detail::DevToDebugWidget_setHide )
            ;

        pybind::interface_<DevToDebugWidgetText, pybind::bases<DevToDebugWidget>>( _kernel, "DevToDebugWidgetText" )
            .def_static_args( "setColor", &Detail::DevToDebugWidgetText_setColor )
            .def_static_args( "setText", &Detail::DevToDebugWidgetText_setText )
            ;

        pybind::interface_<DevToDebugWidgetButton, pybind::bases<DevToDebugWidget>>( _kernel, "DevToDebugWidgetButton" )
            .def_static_args( "setTitle", &Detail::DevToDebugWidgetButton_setTitle )
            .def_static_args( "setClickEvent", &Detail::DevToDebugWidgetButton_setClickEvent )
            ;

        pybind::interface_<DevToDebugWidgetCheckbox, pybind::bases<DevToDebugWidget>>( _kernel, "DevToDebugWidgetCheckbox" )
            .def_static_args( "setTitle", &Detail::DevToDebugWidgetCheckbox_setTitle )
            .def_static_args( "setValue", &Detail::DevToDebugWidgetCheckbox_setValue )
            .def_static_args( "setChangeEvent", &Detail::DevToDebugWidgetCheckbox_setChangeEvent )
            ;

        pybind::interface_<DevToDebugWidgetCommandLine, pybind::bases<DevToDebugWidget>>( _kernel, "DevToDebugWidgetCommandLine" )
            .def_static_args( "setTitle", &Detail::DevToDebugWidgetCommandLine_setTitle )
            .def_static_args( "setPlaceholder", &Detail::DevToDebugWidgetCommandLine_setPlaceholder )
            .def_static_args( "setCommandEvent", &Detail::DevToDebugWidgetCommandLine_setCommandEvent )
            ;

        pybind::interface_<DevToDebugWidgetRadioButton, pybind::bases<DevToDebugWidget>>( _kernel, "DevToDebugWidgetRadioButton" )
            .def_static_args( "setTitle", &Detail::DevToDebugWidgetRadioButton_setTitle )
            .def_static( "addState", &Detail::DevToDebugWidgetRadioButton_addState )
            .def_static_args( "setChangeEvent", &Detail::DevToDebugWidgetRadioButton_setChangeEvent )
            ;

        pybind::interface_<DevToDebugWidgetSelector, pybind::bases<DevToDebugWidget>>( _kernel, "DevToDebugWidgetSelector" )
            .def_static_args( "setTitle", &Detail::DevToDebugWidgetSelector_setTitle )
            .def_static( "addState", &Detail::DevToDebugWidgetSelector_addState )
            .def_static_args( "setChangeEvent", &Detail::DevToDebugWidgetSelector_setChangeEvent )
            ;

        Helper::registerScriptWrapping<DevToDebugTab>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<DevToDebugWidgetText>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<DevToDebugWidgetButton>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<DevToDebugWidgetCheckbox>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<DevToDebugWidgetCommandLine>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<DevToDebugWidgetRadioButton>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<DevToDebugWidgetSelector>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

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
        _kernel->remove_scope<DevToDebugWidgetCommandLine>();
        _kernel->remove_scope<DevToDebugWidgetRadioButton>();
        _kernel->remove_scope<DevToDebugWidgetSelector>();

        _kernel->remove_from_module( "addDevToDebugTab", nullptr );
        _kernel->remove_from_module( "getDevToDebugTab", nullptr );
        _kernel->remove_from_module( "hasDevToDebugTab", nullptr );
        _kernel->remove_from_module( "removeDevToDebugTab", nullptr );
        _kernel->remove_from_module( "createDevToDebugWidgetText", nullptr );
        _kernel->remove_from_module( "createDevToDebugWidgetButton", nullptr );
        _kernel->remove_from_module( "createDevToDebugWidgetCheckbox", nullptr );
        _kernel->remove_from_module( "createDevToDebugWidgetCommandLine", nullptr );
        _kernel->remove_from_module( "createDevToDebugWidgetRadioButton", nullptr );
        _kernel->remove_from_module( "createDevToDebugWidgetSelector", nullptr );

        Helper::unregisterScriptWrapping<DevToDebugTab>();
        Helper::unregisterScriptWrapping<DevToDebugWidgetText>();
        Helper::unregisterScriptWrapping<DevToDebugWidgetButton>();
        Helper::unregisterScriptWrapping<DevToDebugWidgetCheckbox>();
        Helper::unregisterScriptWrapping<DevToDebugWidgetCommandLine>();
        Helper::unregisterScriptWrapping<DevToDebugWidgetRadioButton>();
        Helper::unregisterScriptWrapping<DevToDebugWidgetSelector>();
    }
    //////////////////////////////////////////////////////////////////////////
}
