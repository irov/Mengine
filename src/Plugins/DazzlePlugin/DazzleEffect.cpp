#include "DazzleEffect.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DazzleEffect::DazzleEffect()
        : m_effect( nullptr )
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
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_release()
    {

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
    void DazzleEffect::_updateBoundingBox( mt::box2f * _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        MENGINE_UNUSED( _boundingBox );
        MENGINE_UNUSED( _boundingBoxCurrent );
    }
    //////////////////////////////////////////////////////////////////////////
}