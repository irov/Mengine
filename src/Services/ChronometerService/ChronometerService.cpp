#include "ChronometerService.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactoryPool.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ChronometerService, Mengine::ChronometerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ChronometerService::ChronometerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ChronometerService::~ChronometerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ChronometerService::_initializeService()
    {
        m_factoryChronometer = Helper::makeFactoryPool<Chronometer, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ChronometerService::_finalizeService()
    {
#if defined(MENGINE_DOCUMENT_ENABLE)
        for( const ChronometerPtr & chronometer : m_chronometers )
        {
            LOGGER_ASSERTION( "Not remove chronometer '%s'"
                , MENGINE_DOCUMENT_STR( chronometer->getDocument() )
            );
        }
#endif

        m_chronometers.clear();
        m_chronometersProcess.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryChronometer );
        m_factoryChronometer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ChronometerInterfacePtr ChronometerService::addChronometer( const LambdaChronometer & _lambda, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( _lambda );

        ChronometerPtr chronometer = m_factoryChronometer->createObject( _doc );

        chronometer->initialize( _lambda );

        m_chronometers.emplace_back( chronometer );

        return chronometer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ChronometerService::removeChronometer( const ChronometerInterfacePtr & _chronometer )
    {
        VectorChronometers::iterator it_found = Algorithm::find( m_chronometers.begin(), m_chronometers.end(), _chronometer );

        if( it_found == m_chronometers.end() )
        {
            LOGGER_ERROR( "not found remove chronometer" );

            return false;
        }

        *it_found = m_chronometers.back();
        m_chronometers.pop_back();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ChronometerService::update()
    {
        if( m_chronometers.empty() == true )
        {
            return;
        }

        Timestamp currentTime = Helper::getSystemTimestamp();

        m_chronometersProcess = m_chronometers;

        for( const ChronometerPtr & chronometer : m_chronometersProcess )
        {
            chronometer->update( currentTime );
        }

        m_chronometersProcess.clear();
    }
    //////////////////////////////////////////////////////////////////////////
}
