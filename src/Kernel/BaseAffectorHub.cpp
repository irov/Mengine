#include "BaseAffectorHub.h"

#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseAffectorHub::BaseAffectorHub()
        : m_provider( nullptr )
        , m_angularSpeed( 0.f )
        , m_linearSpeed( 0.f, 0.f, 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseAffectorHub::~BaseAffectorHub()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAffectorHub::setAffectorHubProvider( const AffectorHubProviderInterface * _provider )
    {
        m_provider = _provider;
    }
    //////////////////////////////////////////////////////////////////////////
    const AffectorHubProviderInterface * BaseAffectorHub::getAffectorHubProvider() const
    {
        return m_provider;
    }
    //////////////////////////////////////////////////////////////////////////
    AFFECTOR_ID BaseAffectorHub::addAffector( const AffectorPtr & _affector )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _affector, INVALID_AFFECTOR_ID, "affector is nullptr" );

        AFFECTOR_ID id = GENERATE_UNIQUE_IDENTITY();

        _affector->setId( id );

        EUpdateMode updatableMode = m_provider->getAffectorableUpdatableMode();
        uint32_t updatableDeep = m_provider->getAffectorableUpdatableLeafDeep();

        if( _affector->prepare( updatableMode, updatableDeep ) == false )
        {
            return INVALID_AFFECTOR_ID;
        }

        m_affectors.push_back( _affector );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseAffectorHub::hasAffector( AFFECTOR_ID _id ) const
    {
        for( const AffectorPtr & affector : m_affectors )
        {
            AFFECTOR_ID id = affector->getId();

            if( id != _id )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseAffectorHub::stopAffector( AFFECTOR_ID _id )
    {
        for( IntrusiveSlugAffector it( m_affectors ); it.eof() == false; )
        {
            AffectorPtr affector( *it );

            it.next_shuffle();

            AFFECTOR_ID id = affector->getId();

            if( id != _id )
            {
                continue;
            }

            affector->stop();

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAffectorHub::stopAffectors( EAffectorType _type )
    {
        for( IntrusiveSlugAffector it( m_affectors ); it.eof() == false; )
        {
            AffectorPtr affector( *it );

            it.next_shuffle();

            if( affector->getAffectorType() == _type )
            {
                affector->stop();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAffectorHub::stopAllAffectors()
    {
        for( IntrusiveSlugAffector it( m_affectors ); it.eof() == false; )
        {
            AffectorPtr affector( *it );

            it.next_shuffle();

            affector->stop();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAffectorHub::setAngularSpeed( float _angular )
    {
        m_angularSpeed = _angular;
    }
    //////////////////////////////////////////////////////////////////////////
    float BaseAffectorHub::getAngularSpeed() const
    {
        return m_angularSpeed;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAffectorHub::setLinearSpeed( const mt::vec3f & _linearSpeed )
    {
        m_linearSpeed = _linearSpeed;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & BaseAffectorHub::getLinearSpeed() const
    {
        return m_linearSpeed;
    }
}
