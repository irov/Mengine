#include "AnalyticsFactory.h"

#include "AnalyticsEvent.h"

#include "AnalyticsEventConstParameterBoolean.h"
#include "AnalyticsEventConstParameterConstString.h"
#include "AnalyticsEventConstParameterString.h"
#include "AnalyticsEventConstParameterInteger.h"
#include "AnalyticsEventConstParameterDouble.h"

#include "AnalyticsEventGetterParameterBoolean.h"
#include "AnalyticsEventGetterParameterConstString.h"
#include "AnalyticsEventGetterParameterString.h"
#include "AnalyticsEventGetterParameterInteger.h"
#include "AnalyticsEventGetterParameterDouble.h"

#include "AnalyticsContext.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionFactory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsFactory::AnalyticsFactory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsFactory::~AnalyticsFactory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AnalyticsFactory::initialize()
    {
        m_factoryAnalyticsEvent = Helper::makeFactoryPool<AnalyticsEvent, 256>( MENGINE_DOCUMENT_FACTORABLE );

        m_factoryAnalyticsEventConstParameterBoolean = Helper::makeFactoryPool<AnalyticsEventConstParameterBoolean, 1024>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAnalyticsEventConstParameterConstString = Helper::makeFactoryPool<AnalyticsEventConstParameterConstString, 1024>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAnalyticsEventConstParameterString = Helper::makeFactoryPool<AnalyticsEventConstParameterString, 1024>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAnalyticsEventConstParameterInteger = Helper::makeFactoryPool<AnalyticsEventConstParameterInteger, 1024>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAnalyticsEventConstParameterDouble = Helper::makeFactoryPool<AnalyticsEventConstParameterDouble, 1024>( MENGINE_DOCUMENT_FACTORABLE );

        m_factoryAnalyticsEventGetterParameterBoolean = Helper::makeFactoryPool<AnalyticsEventGetterParameterBoolean, 1024>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAnalyticsEventGetterParameterConstString = Helper::makeFactoryPool<AnalyticsEventGetterParameterConstString, 1024>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAnalyticsEventGetterParameterString = Helper::makeFactoryPool<AnalyticsEventGetterParameterString, 1024>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAnalyticsEventGetterParameterInteger = Helper::makeFactoryPool<AnalyticsEventGetterParameterInteger, 1024>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAnalyticsEventGetterParameterDouble = Helper::makeFactoryPool<AnalyticsEventGetterParameterDouble, 1024>( MENGINE_DOCUMENT_FACTORABLE );

        m_factoryAnalyticsContext = Helper::makeFactoryPool<AnalyticsContext, 256>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsFactory::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEvent );

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEventConstParameterBoolean );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEventConstParameterConstString );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEventConstParameterString );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEventConstParameterInteger );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEventConstParameterDouble );

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsContext );

        m_factoryAnalyticsEvent = nullptr;

        m_factoryAnalyticsEventConstParameterBoolean = nullptr;
        m_factoryAnalyticsEventConstParameterConstString = nullptr;
        m_factoryAnalyticsEventConstParameterString = nullptr;
        m_factoryAnalyticsEventConstParameterInteger = nullptr;
        m_factoryAnalyticsEventConstParameterDouble = nullptr;

        m_factoryAnalyticsContext = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventInterfacePtr AnalyticsFactory::makeAnalyticsEvent( EAnalyticsEventType _type, const ConstString & _name )
    {
        AnalyticsEventPtr event = m_factoryAnalyticsEvent->createObject( MENGINE_DOCUMENT_FACTORABLE );

        event->setType( _type );
        event->setName( _name );

        return event;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterBooleanInterfacePtr AnalyticsFactory::makeAnalyticsEventConstParameterBoolean( bool _value )
    {
        AnalyticsEventConstParameterBooleanPtr parameter = m_factoryAnalyticsEventConstParameterBoolean->createObject( MENGINE_DOCUMENT_FACTORABLE );

        parameter->setValue( _value );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterConstStringInterfacePtr AnalyticsFactory::makeAnalyticsEventConstParameterConstString( const ConstString & _value )
    {
        AnalyticsEventConstParameterConstStringPtr parameter = m_factoryAnalyticsEventConstParameterConstString->createObject( MENGINE_DOCUMENT_FACTORABLE );

        parameter->setValue( _value );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterStringInterfacePtr AnalyticsFactory::makeAnalyticsEventConstParameterString( const String & _value )
    {
        AnalyticsEventConstParameterStringPtr parameter = m_factoryAnalyticsEventConstParameterString->createObject( MENGINE_DOCUMENT_FACTORABLE );

        parameter->setValue( _value );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterIntegerInterfacePtr AnalyticsFactory::makeAnalyticsEventConstParameterInteger( int64_t _value )
    {
        AnalyticsEventConstParameterIntegerPtr parameter = m_factoryAnalyticsEventConstParameterInteger->createObject( MENGINE_DOCUMENT_FACTORABLE );

        parameter->setValue( _value );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterDoubleInterfacePtr AnalyticsFactory::makeAnalyticsEventConstParameterDouble( double _value )
    {
        AnalyticsEventConstParameterDoublePtr parameter = m_factoryAnalyticsEventConstParameterDouble->createObject( MENGINE_DOCUMENT_FACTORABLE );

        parameter->setValue( _value );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterBooleanInterfacePtr AnalyticsFactory::makeAnalyticsEventGetterParameterBoolean( const LambdaAnalyticsParameterGetterBoolean & _lambda )
    {
        AnalyticsEventGetterParameterBooleanPtr parameter = m_factoryAnalyticsEventGetterParameterBoolean->createObject( MENGINE_DOCUMENT_FACTORABLE );

        parameter->setAnalyticsFactory( AnalyticsFactoryInterfacePtr::from( this ) );

        parameter->setGetter( _lambda );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterConstStringInterfacePtr AnalyticsFactory::makeAnalyticsEventGetterParameterConstString( const LambdaAnalyticsParameterGetterConstString & _lambda )
    {
        AnalyticsEventGetterParameterConstStringPtr parameter = m_factoryAnalyticsEventGetterParameterConstString->createObject( MENGINE_DOCUMENT_FACTORABLE );

        parameter->setAnalyticsFactory( AnalyticsFactoryInterfacePtr::from( this ) );

        parameter->setGetter( _lambda );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterStringInterfacePtr AnalyticsFactory::makeAnalyticsEventGetterParameterString( const LambdaAnalyticsParameterGetterString & _lambda )
    {
        AnalyticsEventGetterParameterStringPtr parameter = m_factoryAnalyticsEventGetterParameterString->createObject( MENGINE_DOCUMENT_FACTORABLE );

        parameter->setAnalyticsFactory( AnalyticsFactoryInterfacePtr::from( this ) );

        parameter->setGetter( _lambda );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterIntegerInterfacePtr AnalyticsFactory::makeAnalyticsEventGetterParameterInteger( const LambdaAnalyticsParameterGetterInteger & _lambda )
    {
        AnalyticsEventGetterParameterIntegerPtr parameter = m_factoryAnalyticsEventGetterParameterInteger->createObject( MENGINE_DOCUMENT_FACTORABLE );

        parameter->setAnalyticsFactory( AnalyticsFactoryInterfacePtr::from( this ) );

        parameter->setGetter( _lambda );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterDoubleInterfacePtr AnalyticsFactory::makeAnalyticsEventGetterParameterDouble( const LambdaAnalyticsParameterGetterDouble & _lambda )
    {
        AnalyticsEventGetterParameterDoublePtr parameter = m_factoryAnalyticsEventGetterParameterDouble->createObject( MENGINE_DOCUMENT_FACTORABLE );

        parameter->setAnalyticsFactory( AnalyticsFactoryInterfacePtr::from( this ) );

        parameter->setGetter( _lambda );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsContextInterfacePtr AnalyticsFactory::makeAnalyticsContext()
    {
        AnalyticsContextPtr context = m_factoryAnalyticsContext->createObject( MENGINE_DOCUMENT_FACTORABLE );

        context->setAnalyticsFactory( AnalyticsFactoryInterfacePtr::from( this ) );

        return context;
    }
    //////////////////////////////////////////////////////////////////////////
}
