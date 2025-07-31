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
    void BaseAffectorHub::addAffector( const AffectorPtr & _affector )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _affector, "affector is nullptr" );

        EUpdateMode updatableMode = m_provider->getAffectorableUpdatableMode();
        uint32_t updatableDeep = m_provider->getAffectorableUpdatableLeafDeep();

        _affector->prepare( updatableMode, updatableDeep );

        m_affectors.push_back( _affector );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAffectorHub::stopAffectors( EAffectorType _type )
    {
        IntrusivePtrScope ankh( this );

        for( IntrusiveSlugAffector it( m_affectors ); it.eof() == false; )
        {
            AffectorPtr affector( *it );

            it.next_shuffle();

            EAffectorType affectorType = affector->getAffectorType();

            if( affectorType != _type )
            {
                continue;
            }

            affector->stop();
        }

        switch( _type )
        {
        case EAFFECTORTYPE_POSITION:
            {
                this->setLinearSpeed( mt::vec3f( 0.f, 0.f, 0.f ) );
            }break;
        case EAFFECTORTYPE_ANGLE:
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
