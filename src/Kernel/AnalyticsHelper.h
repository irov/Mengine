#pragma once

#include "Interface/AnalyticsEventParameterInterface.h"

#include "Config/Variant.h"

namespace Mengine
{
    namespace Helper
    {
        template<class ... Fn>
        void visitAnalyticsParameter( const AnalyticsEventParameterInterfacePtr & _parameter, Fn && ... fn )
        {
            auto dispatcher = Mengine::Helper::Overloaded{std::forward<Fn>( fn ) ...};

            EAnalyticsEventParameterType type = _parameter->getType();

            switch( type )
            {
            case EAEPT_BOOLEAN:
                {
                    AnalyticsEventParameterBooleanInterfacePtr parameter_boolean = AnalyticsEventParameterBooleanInterfacePtr::from( _parameter );

                    bool value = parameter_boolean->resolveValue();

                    dispatcher( value );
                }break;
            case EAEPT_INTEGER:
                {
                    AnalyticsEventParameterIntegerInterfacePtr parameter_integer = AnalyticsEventParameterIntegerInterfacePtr::from( _parameter );

                    int64_t value = parameter_integer->resolveValue();

                    dispatcher( value );
                }break;
            case EAEPT_DOUBLE:
                {
                    AnalyticsEventParameterDoubleInterfacePtr parameter_double = AnalyticsEventParameterDoubleInterfacePtr::from( _parameter );

                    double value = parameter_double->resolveValue();

                    dispatcher( value );
                }break;
            case EAEPT_STRING:
                {
                    AnalyticsEventParameterStringInterfacePtr parameter_string = AnalyticsEventParameterStringInterfacePtr::from( _parameter );

                    const String & value = parameter_string->resolveValue();

                    dispatcher( value );
                }break;
            case EAEPT_CONSTSTRING:
                {
                    AnalyticsEventParameterConstStringInterfacePtr parameter_conststring = AnalyticsEventParameterConstStringInterfacePtr::from( _parameter );

                    const ConstString & value = parameter_conststring->resolveValue();

                    dispatcher( value );
                }break;
            }
        }
    }
}