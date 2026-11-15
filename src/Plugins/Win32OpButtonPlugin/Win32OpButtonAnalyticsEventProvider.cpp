#include "Win32OpButtonAnalyticsEventProvider.h"

#include "Interface/AnalyticsContextInterface.h"
#include "Interface/AnalyticsEventParameterInterface.h"

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonSdk.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32OpButtonAnalyticsEventProvider::Win32OpButtonAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32OpButtonAnalyticsEventProvider::~Win32OpButtonAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonAnalyticsEventProvider::onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event )
    {
        const ConstString & eventName = _event->getName();

        typedef OpButton::OpButtonMap<OpButton::OpButtonString, OpButton::OpButtonJson> OpButtonMetadata;
        OpButtonMetadata metadata;

        _event->foreachParameters( [&metadata]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
        {
            OpButton::OpButtonString key = _name.c_str();

            EAnalyticsEventParameterType type = _parameter->getType();

            switch( type )
            {
            case EAEPT_BOOLEAN:
                {
                    AnalyticsEventParameterBooleanInterface * paramBoolean = static_cast<AnalyticsEventParameterBooleanInterface *>(_parameter.get());
                    bool value = paramBoolean->resolveValue();
                    metadata[key] = OpButton::OpButtonJson( value );
                }break;
            case EAEPT_INTEGER:
                {
                    AnalyticsEventParameterIntegerInterface * paramInteger = static_cast<AnalyticsEventParameterIntegerInterface *>(_parameter.get());
                    int64_t value = paramInteger->resolveValue();
                    metadata[key] = OpButton::OpButtonJson( value );
                }break;
            case EAEPT_DOUBLE:
                {
                    AnalyticsEventParameterDoubleInterface * paramDouble = static_cast<AnalyticsEventParameterDoubleInterface *>(_parameter.get());
                    double value = paramDouble->resolveValue();
                    metadata[key] = OpButton::OpButtonJson( value );
                }break;
            case EAEPT_STRING:
                {
                    AnalyticsEventParameterStringInterface * paramString = static_cast<AnalyticsEventParameterStringInterface *>(_parameter.get());
                    const String & value = paramString->resolveValue();
                    const Char * valueData = value.c_str();
                    size_t valueSize = value.size();
                    OpButton::OpButtonString stringValue( valueData, valueSize );
                    metadata[key] = OpButton::OpButtonJson( stringValue );
                }break;
            case EAEPT_CONSTSTRING:
                {
                    AnalyticsEventParameterConstStringInterface * paramConstString = static_cast<AnalyticsEventParameterConstStringInterface *>(_parameter.get());
                    const ConstString & value = paramConstString->resolveValue();
                    const Char * valueData = value.c_str();
                    OpButton::OpButtonString stringValue( valueData );
                    metadata[key] = OpButton::OpButtonJson( stringValue );
                }break;
            default:
                break;
            }
        } );

        EAnalyticsEventCategory category = _event->getCategory();

        if( category == AEEC_SYSTEM )
        {
            return;
        }

        OpButton::OpButtonString eventNameStr = eventName.c_str();

        OpButton::OpButtonSdk::event( eventNameStr, metadata );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonAnalyticsEventProvider::onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName )
    {
        OpButton::OpButtonString screenType = _screenType.c_str();
        OpButton::OpButtonString screenName = _screenName.c_str();

        OpButton::OpButtonSdk::changeScreen( screenType, screenName );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonAnalyticsEventProvider::onAnalyticsFlush()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
