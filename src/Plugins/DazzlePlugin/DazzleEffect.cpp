#include "DazzleEffect.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DazzleEffect::DazzleEffect()
        : m_instance( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DazzleEffect::~DazzleEffect()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_play( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_restart( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_pause( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_resume( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_stop( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_end( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_interrupt( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float DazzleEffect::_getDuration() const
    {
        return 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_setLoop( bool _value )
    {
        MENGINE_UNUSED( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_activate()
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_deactivate()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceDazzleEffect, "name '%s' can't setup resource"
            , this->getName().c_str()
        );

        if( m_resourceDazzleEffect->compile() == false )
        {
            LOGGER_ERROR( "name '%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceDazzleEffect->getName().c_str()
            );

            return false;
        }

        const DazzleDataInterfacePtr & data = m_resourceDazzleEffect->getData();

        if( data->acquire() == false )
        {
            LOGGER_ERROR( "name '%s' resource '%s' not acquire composition"
                , this->getName().c_str()
                , m_resourceDazzleEffect->getName().c_str()
            );

            return false;
        }

        const dz_service_t * service = data->getDazzleService();
        const dz_effect_t * effect = data->getDazzleEffect();

        dz_instance_t * instance;
        if( dz_instance_create( service, &instance, effect, 0, DZ_NULLPTR ) == false )
        {
            return false;
        }
        
        m_instance = instance;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_release()
    {
        const DazzleDataInterfacePtr & data = m_resourceDazzleEffect->getData();

        const dz_service_t * service = data->getDazzleService();

        dz_instance_destroy( service, m_instance );
        m_instance = nullptr;

        data->release();

        m_resourceDazzleEffect->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::update( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        MENGINE_UNUSED( _boundingBox );
        MENGINE_UNUSED( _boundingBoxCurrent );
    }
    //////////////////////////////////////////////////////////////////////////
}