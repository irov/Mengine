#include "Win32PilotAnalyticsEventProvider.h"

#include "Interface/AnalyticsEventParameterInterface.h"
#include "Interface/AnalyticsContextInterface.h"

#include "Pilot/PilotSdk.h"
#include "Pilot/PilotJson.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32PilotAnalyticsEventProvider::Win32PilotAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32PilotAnalyticsEventProvider::~Win32PilotAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PilotAnalyticsEventProvider::onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event )
    {
        const ConstString & eventName = _event->getName();

        Pilot::PilotMap<Pilot::PilotString, Pilot::PilotJson> metadata;

        _event->foreachParameters( [&metadata]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
        {
            Pilot::PilotString key = _name.c_str();

            EAnalyticsEventParameterType type = _parameter->getType();

            switch( type )
            {
            case EAEPT_BOOLEAN:
                {
                    AnalyticsEventParameterBooleanInterface * paramBoolean = static_cast<AnalyticsEventParameterBooleanInterface *>(_parameter.get());
                    bool value = paramBoolean->resolveValue();
                    metadata[key] = Pilot::PilotJson( value );
                }break;
            case EAEPT_INTEGER:
                {
                    AnalyticsEventParameterIntegerInterface * paramInteger = static_cast<AnalyticsEventParameterIntegerInterface *>(_parameter.get());
                    int64_t value = paramInteger->resolveValue();
                    metadata[key] = Pilot::PilotJson( value );
                }break;
            case EAEPT_DOUBLE:
                {
                    AnalyticsEventParameterDoubleInterface * paramDouble = static_cast<AnalyticsEventParameterDoubleInterface *>(_parameter.get());
                    double value = paramDouble->resolveValue();
                    metadata[key] = Pilot::PilotJson( value );
                }break;
            case EAEPT_STRING:
                {
                    AnalyticsEventParameterStringInterface * paramString = static_cast<AnalyticsEventParameterStringInterface *>(_parameter.get());
                    const String & value = paramString->resolveValue();
                    metadata[key] = Pilot::PilotJson( Pilot::PilotString( value.c_str(), value.size() ) );
                }break;
            case EAEPT_CONSTSTRING:
                {
                    AnalyticsEventParameterConstStringInterface * paramConstString = static_cast<AnalyticsEventParameterConstStringInterface *>(_parameter.get());
                    const ConstString & value = paramConstString->resolveValue();
                    metadata[key] = Pilot::PilotJson( Pilot::PilotString( value.c_str() ) );
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

        Pilot::PilotString eventNameStr = eventName.c_str();

        Pilot::PilotSdk::event( eventNameStr, metadata );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PilotAnalyticsEventProvider::onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName )
    {
        Pilot::PilotString screenType = _screenType.c_str();
        Pilot::PilotString screenName = _screenName.c_str();

        Pilot::PilotSdk::changeScreen( screenType, screenName );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PilotAnalyticsEventProvider::onAnalyticsFlush()
    {
        // Pilot SDK handles flushing internally
    }
    //////////////////////////////////////////////////////////////////////////
}
