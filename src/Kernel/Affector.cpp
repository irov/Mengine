#include "Kernel/Affector.h"

#include "Interface/UpdationInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"

#include "Kernel/IntrusivePtrScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Affector::Affector()
        : m_type( EAFFECTORTYPE_NONE )
        , m_status( EAFFECTORSTATUS_IDLE )
        , m_speedFactor( 1.f )
        , m_freeze( false )
        , m_affecting( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Affector::~Affector()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setAffectorType( EAffectorType _type )
    {
        m_type = _type;
    }
    //////////////////////////////////////////////////////////////////////////
    EAffectorType Affector::getAffectorType() const
    {
        return m_type;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setSpeedFactor( float _speedAffector )
    {
        m_speedFactor = _speedAffector;
    }
    //////////////////////////////////////////////////////////////////////////
    float Affector::getSpeedFactor() const
    {
        return m_speedFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setFreeze( bool _value )
    {
        m_freeze = _value;

        this->_setFreeze( m_freeze );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Affector::getFreeze() const
    {
        return m_freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::_setFreeze( bool _value )
    {
        MENGINE_UNUSED( _value );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setEasing( const EasingInterfacePtr & _easing )
    {
        m_easing = _easing;
    }
    //////////////////////////////////////////////////////////////////////////
    const EasingInterfacePtr & Affector::getEasing() const
    {
        return m_easing;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::prepare( EUpdateMode _updatableMode, uint32_t _updatableLeaf )
    {
        m_status = EAFFECTORSTATUS_PREPARE;

        this->_prepare();

        UpdationInterface * updation = this->getUpdation();

        updation->activate( _updatableMode, _updatableLeaf );
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::_prepare()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::update( const UpdateContext * _context )
    {
        float used = 0.f;

        m_affecting = true;
        bool finish = this->_affect( _context, &used );
        m_affecting = false;

        if( finish == true || m_status == EAFFECTORSTATUS_STOP )
        {
            TIMELINE_SERVICE()
                ->beginOffset( used, MENGINE_DOCUMENT_FACTORABLE );

            this->complete( finish );

            TIMELINE_SERVICE()
                ->endOffset();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::stop()
    {
        m_status = EAFFECTORSTATUS_STOP;

        this->_stop();

        this->complete( false );
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::_stop()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::complete( bool _isEnd )
    {
        MENGINE_UNUSED( _isEnd );

        if( m_affecting == true )
        {
            return;
        }

        m_status = EAFFECTORSTATUS_COMPLETE;

        IntrusivePtrScope ankh( this );

        UpdationInterface * updation = this->getUpdation();

        updation->deactivate();

        this->IntrusiveSlugLinkedPtr<Affector, void, IntrusivePtr, IntrusivePtrBase>::unlink();

        this->_complete( _isEnd );
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::_complete( bool _isEnd )
    {
        MENGINE_UNUSED( _isEnd );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
