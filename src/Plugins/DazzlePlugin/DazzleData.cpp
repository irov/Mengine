#include "DazzleData.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/MaterialEnumInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DazzleData::DazzleData()
        : m_service( nullptr )
        , m_effect( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DazzleData::~DazzleData()
    {
        if( m_effect != nullptr )
        {
            dz_effect_destroy( m_service, m_effect );
            m_effect = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleData::acquire()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleData::release()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleData::setDazzleService( const dz_service_t * _service )
    {
        m_service = _service;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleData::setDazzleEffect( dz_effect_t * _effect )
    {
        m_effect = _effect;
    }
    //////////////////////////////////////////////////////////////////////////
    const dz_effect_t * DazzleData::getDazzleEffect() const
    {
        return m_effect;
    }
    //////////////////////////////////////////////////////////////////////////
    dz_result_t DazzleData::setDazzleEmitterTexture( const dz_shape_mask_source_t * _source )
    {
        dz_result_t result = dz_effect_set_emitter_texture( m_service, m_effect, _source );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
}
