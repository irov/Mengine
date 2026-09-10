#include "AttributionScriptEmbedding.h"

#include "Interface/AttributionServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/UnicodeHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static ParamVariant pythonAttributionValue( const ParamVariant & _value )
        {
            String utf8;
            if( Helper::get( _value, &utf8 ) == false )
            {
                return _value;
            }

            WString unicode;
            if( Helper::utf8ToUnicode( utf8, &unicode ) == false )
            {
                return _value;
            }

            return unicode;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool registerAttribution( const ConstString & _name, EAttributionType _type )
        {
            bool successful = ATTRIBUTION_SERVICE()
                ->registerAttribution( _name, _type );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool setAttribution( const ConstString & _name, const ParamVariant & _value )
        {
            ParamVariant value = _value;

            WString unicode;
            if( Helper::get( _value, &unicode ) == true )
            {
                String utf8;
                if( Helper::unicodeToUtf8( unicode, &utf8 ) == false )
                {
                    return false;
                }

                value = utf8;
            }

            bool successful = ATTRIBUTION_SERVICE()
                ->setAttribution( _name, value );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static ParamVariant getAttribution( const ConstString & _name )
        {
            ParamVariant value = ATTRIBUTION_SERVICE()
                ->getAttribution( _name );

            ParamVariant pythonValue = Detail::pythonAttributionValue( value );

            return pythonValue;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool clearAttribution( const ConstString & _name )
        {
            bool successful = Detail::setAttribution( _name, nullptr );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static void clearAttributions()
        {
            ATTRIBUTION_SERVICE()
                ->clearAttributions();
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AttributionScriptEmbedding::AttributionScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AttributionScriptEmbedding::~AttributionScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AttributionScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::enum_<EAttributionType>( _kernel, "EAttributionType" )
            .def( "EAT_BOOLEAN", EAttributionType::EAT_BOOLEAN )
            .def( "EAT_INTEGER", EAttributionType::EAT_INTEGER )
            .def( "EAT_DOUBLE", EAttributionType::EAT_DOUBLE )
            .def( "EAT_STRING", EAttributionType::EAT_STRING )
            ;

        pybind::def_function( _kernel, "registerAttribution", &Detail::registerAttribution );
        pybind::def_function( _kernel, "setAttribution", &Detail::setAttribution );
        pybind::def_function( _kernel, "getAttribution", &Detail::getAttribution );
        pybind::def_function( _kernel, "clearAttribution", &Detail::clearAttribution );
        pybind::def_function( _kernel, "clearAttributions", &Detail::clearAttributions );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AttributionScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "registerAttribution", nullptr );
        _kernel->remove_from_module( "setAttribution", nullptr );
        _kernel->remove_from_module( "getAttribution", nullptr );
        _kernel->remove_from_module( "clearAttribution", nullptr );
        _kernel->remove_from_module( "clearAttributions", nullptr );

        _kernel->remove_from_module( "EAT_BOOLEAN", nullptr );
        _kernel->remove_from_module( "EAT_INTEGER", nullptr );
        _kernel->remove_from_module( "EAT_DOUBLE", nullptr );
        _kernel->remove_from_module( "EAT_STRING", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
