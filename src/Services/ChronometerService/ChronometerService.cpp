#include "ChronometerService.h"

#include "Interface/TimeSystemInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ChronometerService, Mengine::ChronometerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ChronometerService::ChronometerService()
        : m_oldTime( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ChronometerService::~ChronometerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ChronometerService::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ChronometerService::_finalizeService()
    {
#ifdef MENGINE_DEBUG
        for( const ChronometerDesc & desc : m_chronometers )
        {
            LOGGER_ERROR( "Not remove chronometer '%s'"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_chronometers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ChronometerService::addChronometer( const LambdaChronometer & _lambda, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        uint32_t new_id = GENERATE_UNIQUE_IDENTITY();

        ChronometerDesc desc;
        desc.id = new_id;
        desc.lambda = _lambda;

#ifdef MENGINE_DEBUG
        desc.doc = _doc;
#endif

        m_chronometers.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ChronometerService::removeChronometer( uint32_t _id )
    {
        for( ChronometerDesc & desc : m_chronometers )
        {
            if( desc.id != _id )
            {
                continue;
            }

            desc = m_chronometers.back();
            m_chronometers.pop_back();

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ChronometerService::update()
    {
        uint64_t currentTime = GET_TIME_MILLISECONDS();

        if( m_oldTime == 0 || currentTime - m_oldTime >= 1000 )
        {
            m_chronometersProcess = m_chronometers;

            for( const ChronometerDesc & desc : m_chronometersProcess )
            {
                desc.lambda( desc.id, currentTime );
            }

            m_chronometersProcess.clear();

            m_oldTime = currentTime - currentTime % 1000;
        }
    }
}
