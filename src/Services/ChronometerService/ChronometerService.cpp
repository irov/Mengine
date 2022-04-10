#include "ChronometerService.h"

#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/TimeHelper.h"
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
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ChronometerService::_finalizeService()
    {
#if MENGINE_DOCUMENT_ENABLE
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

        UniqueId new_id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

        ChronometerDesc desc;
        desc.id = new_id;
        desc.lambda = _lambda;

#if MENGINE_DOCUMENT_ENABLE
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
        TimeMilliseconds currentTime = Helper::getTimeMilliseconds();

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
    //////////////////////////////////////////////////////////////////////////
}
