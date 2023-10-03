#include "AnalyticsFactory.h"

#include "AnalyticsEventBuilder.h"
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
        m_factoryAnalyticsEventBuilder = Helper::makeFactoryPool<AnalyticsEventBuilder, 16>( MENGINE_DOCUMENT_FACTORABLE );
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
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEventBuilder );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEvent );

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEventConstParameterBoolean );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEventConstParameterConstString );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEventConstParameterString );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEventConstParameterInteger );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEventConstParameterDouble );

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsContext );        

        m_factoryAnalyticsEventBuilder = nullptr;
        m_factoryAnalyticsEvent = nullptr;

        m_factoryAnalyticsEventConstParameterBoolean = nullptr;
        m_factoryAnalyticsEventConstParameterConstString = nullptr;
        m_factoryAnalyticsEventConstParameterString = nullptr;
        m_factoryAnalyticsEventConstParameterInteger = nullptr;
        m_factoryAnalyticsEventConstParameterDouble = nullptr;

        m_factoryAnalyticsContext = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterfacePtr AnalyticsFactory::makeEventBuilder( const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr eventBuilder = m_factoryAnalyticsEventBuilder->createObject( _doc );

        AnalyticsContextInterfacePtr context = this->makeContext( _doc );

        eventBuilder->setContext( context );

        return eventBuilder;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventInterfacePtr AnalyticsFactory::makeEvent( const ConstString & _name, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventPtr event = m_factoryAnalyticsEvent->createObject( _doc );

        event->setName( _name );

        return event;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterBooleanInterfacePtr AnalyticsFactory::makeEventConstParameterBoolean( bool _value, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventConstParameterBooleanPtr parameter = m_factoryAnalyticsEventConstParameterBoolean->createObject( _doc );

        parameter->setValue( _value );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterConstStringInterfacePtr AnalyticsFactory::makeEventConstParameterConstString( const ConstString & _value, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventConstParameterConstStringPtr parameter = m_factoryAnalyticsEventConstParameterConstString->createObject( _doc );

        parameter->setValue( _value );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterStringInterfacePtr AnalyticsFactory::makeEventConstParameterString( const String & _value, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventConstParameterStringPtr parameter = m_factoryAnalyticsEventConstParameterString->createObject( _doc );

        parameter->setValue( _value );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterIntegerInterfacePtr AnalyticsFactory::makeEventConstParameterInteger( int64_t _value, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventConstParameterIntegerPtr parameter = m_factoryAnalyticsEventConstParameterInteger->createObject( _doc );

        parameter->setValue( _value );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterDoubleInterfacePtr AnalyticsFactory::makeEventConstParameterDouble( double _value, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventConstParameterDoublePtr parameter = m_factoryAnalyticsEventConstParameterDouble->createObject( _doc );

        parameter->setValue( _value );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterBooleanInterfacePtr AnalyticsFactory::makeEventGetterParameterBoolean( const LambdaAnalyticsParameterGetterBoolean & _lambda, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventGetterParameterBooleanPtr parameter = m_factoryAnalyticsEventGetterParameterBoolean->createObject( _doc );

        parameter->setAnalyticsFactory( AnalyticsFactoryInterfacePtr::from( this ) );

        parameter->setGetter( _lambda );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterConstStringInterfacePtr AnalyticsFactory::makeEventGetterParameterConstString( const LambdaAnalyticsParameterGetterConstString & _lambda, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventGetterParameterConstStringPtr parameter = m_factoryAnalyticsEventGetterParameterConstString->createObject( _doc );

        parameter->setAnalyticsFactory( AnalyticsFactoryInterfacePtr::from( this ) );

        parameter->setGetter( _lambda );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterStringInterfacePtr AnalyticsFactory::makeEventGetterParameterString( const LambdaAnalyticsParameterGetterString & _lambda, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventGetterParameterStringPtr parameter = m_factoryAnalyticsEventGetterParameterString->createObject( _doc );

        parameter->setAnalyticsFactory( AnalyticsFactoryInterfacePtr::from( this ) );

        parameter->setGetter( _lambda );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterIntegerInterfacePtr AnalyticsFactory::makeEventGetterParameterInteger( const LambdaAnalyticsParameterGetterInteger & _lambda, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventGetterParameterIntegerPtr parameter = m_factoryAnalyticsEventGetterParameterInteger->createObject( _doc );

        parameter->setAnalyticsFactory( AnalyticsFactoryInterfacePtr::from( this ) );

        parameter->setGetter( _lambda );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterDoubleInterfacePtr AnalyticsFactory::makeEventGetterParameterDouble( const LambdaAnalyticsParameterGetterDouble & _lambda, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventGetterParameterDoublePtr parameter = m_factoryAnalyticsEventGetterParameterDouble->createObject( _doc );

        parameter->setAnalyticsFactory( AnalyticsFactoryInterfacePtr::from( this ) );

        parameter->setGetter( _lambda );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsContextInterfacePtr AnalyticsFactory::makeContext( const DocumentInterfacePtr & _doc )
    {
        AnalyticsContextPtr context = m_factoryAnalyticsContext->createObject( _doc );

        context->setAnalyticsFactory( AnalyticsFactoryInterfacePtr::from( this ) );

        return context;
    }
    //////////////////////////////////////////////////////////////////////////
}
