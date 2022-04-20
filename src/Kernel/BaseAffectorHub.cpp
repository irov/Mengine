#include "BaseAffectorHub.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/IntrusivePtrScope.h"
#include "Kernel/EnumeratorHelper.h"

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
    UniqueId BaseAffectorHub::addAffector( const AffectorPtr & _affector )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _affector, "affector is nullptr" );

        UniqueId id = Helper::generateUniqueIdentity();

        _affector->setId( id );

        EUpdateMode updatableMode = m_provider->getAffectorableUpdatableMode();
        uint32_t updatableDeep = m_provider->getAffectorableUpdatableLeafDeep();

        if( _affector->prepare( updatableMode, updatableDeep ) == false )
        {
            return INVALID_UNIQUE_ID;
        }

        m_affectors.push_back( _affector );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseAffectorHub::hasAffector( UniqueId _id ) const
    {
        for( const AffectorPtr & affector : m_affectors )
        {
            UniqueId id = affector->getId();

            if( id != _id )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseAffectorHub::stopAffector( UniqueId _id )
    {
        IntrusivePtrScope ankh( this );

        for( IntrusiveSlugAffector it( m_affectors ); it.eof() == false; )
        {
            AffectorPtr affector( *it );

            it.next_shuffle();

            UniqueId id = affector->getId();

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
        IntrusivePtrScope ankh( this );

        for( IntrusiveSlugAffector it( m_affectors ); it.eof() == false; )
        {
            AffectorPtr affector( *it );

            it.next_shuffle();

            if( affector->getAffectorType() == _type )
            {
                affector->stop();
            }
        }

        switch( _type )
        {
        case ETA_POSITION:
            {
                this->setLinearSpeed( mt::vec3f( 0.f, 0.f, 0.f ) );
            }break;
        case ETA_ANGLE:
            {
                this->setAngularSpeed( 0.f );
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAffectorHub::stopAllAffectors()
    {
        IntrusivePtrScope ankh( this );

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
    //////////////////////////////////////////////////////////////////////////
}
