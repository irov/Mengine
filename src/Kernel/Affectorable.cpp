#include "Kernel/Affectorable.h"

#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Affectorable::Affectorable()
        : m_angularSpeed( 0.f )
        , m_linearSpeed( 0.f, 0.f, 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Affectorable::~Affectorable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AFFECTOR_ID Affectorable::addAffector( const AffectorPtr & _affector )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _affector, INVALID_AFFECTOR_ID, "affector is nullptr" );

        AFFECTOR_ID id = GENERATE_UNIQUE_IDENTITY();

        _affector->setId( id );

        EUpdateMode updatableMode = this->getAffectorableUpdatableMode();
        uint32_t updatableDeep = this->getAffectorableUpdatableLeafDeep();

        if( _affector->prepare( updatableMode, updatableDeep ) == false )
        {
            return INVALID_AFFECTOR_ID;
        }

        m_affectors.push_back( _affector );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Affectorable::hasAffector( AFFECTOR_ID _id ) const
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
    bool Affectorable::stopAffector( uint32_t _id )
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
    void Affectorable::stopAffectors( EAffectorType _type )
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
    void Affectorable::stopAllAffectors()
    {
        for( IntrusiveSlugAffector it( m_affectors ); it.eof() == false; )
        {
            AffectorPtr affector( *it );

            it.next_shuffle();

            affector->stop();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Affectorable::setAngularSpeed( float _angular )
    {
        m_angularSpeed = _angular;
    }
    //////////////////////////////////////////////////////////////////////////
    float Affectorable::getAngularSpeed() const
    {
        return m_angularSpeed;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affectorable::setLinearSpeed( const mt::vec3f & _linearSpeed )
    {
        m_linearSpeed = _linearSpeed;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & Affectorable::getLinearSpeed() const
    {
        return m_linearSpeed;
    }
}
