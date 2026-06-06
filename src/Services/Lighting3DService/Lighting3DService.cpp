#include "Lighting3DService.h"

#include "Kernel/Logger.h"

#include <math.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Lighting3DService, Mengine::Lighting3DService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Lighting3DService::Lighting3DService()
        : m_ambient( 0.f, 0.f, 0.f )
        , m_hasDirectional( false )
        , m_directionalDir( 0.f, -1.f, 0.f )
        , m_directionalColor( 1.f, 1.f, 1.f )
        , m_directionalIntensity( 0.f )
    {
        for( PointLightSlot & slot : m_pointLights )
        {
            slot.pos = mt::vec3f( 0.f, 0.f, 0.f );
            slot.invRadius = 0.f;
            slot.color = mt::vec3f( 0.f, 0.f, 0.f );
            slot.intensity = 0.f;
            slot.acquired = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Lighting3DService::~Lighting3DService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Lighting3DService::_initializeService()
    {

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Lighting3DService::_finalizeService()
    {
        m_hasDirectional = false;
        m_directionalIntensity = 0.f;

        for( PointLightSlot & slot : m_pointLights )
        {
            slot.acquired = false;
            slot.invRadius = 0.f;
            slot.intensity = 0.f;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Lighting3DService::setAmbient( const mt::vec3f & _color )
    {
        m_ambient = _color;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & Lighting3DService::getAmbient() const
    {
        return m_ambient;
    }
    //////////////////////////////////////////////////////////////////////////
    void Lighting3DService::setDirectionalLight( const mt::vec3f & _dir, const mt::vec3f & _color, float _intensity )
    {
        float lenSq = _dir.x * _dir.x + _dir.y * _dir.y + _dir.z * _dir.z;

        if( lenSq > 1e-8f )
        {
            float inv = 1.f / sqrtf( lenSq );
            m_directionalDir = mt::vec3f( _dir.x * inv, _dir.y * inv, _dir.z * inv );
        }
        else
        {
            m_directionalDir = mt::vec3f( 0.f, -1.f, 0.f );
        }

        m_directionalColor = _color;
        m_directionalIntensity = _intensity;
        m_hasDirectional = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Lighting3DService::clearDirectionalLight()
    {
        m_hasDirectional = false;
        m_directionalIntensity = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Lighting3DService::hasDirectionalLight() const
    {
        return m_hasDirectional;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Lighting3DService::acquirePointLight()
    {
        for( uint32_t i = 0; i != MENGINE_LIGHTING3D_POINT_LIGHT_MAX; ++i )
        {
            if( m_pointLights[i].acquired == false )
            {
                m_pointLights[i].acquired = true;
                m_pointLights[i].invRadius = 0.f;
                m_pointLights[i].intensity = 0.f;
                return i;
            }
        }

        LOGGER_ERROR( "lighting3d: point light pool exhausted (max=%u)"
            , (uint32_t)MENGINE_LIGHTING3D_POINT_LIGHT_MAX
        );

        return UINT32_MAX;
    }
    //////////////////////////////////////////////////////////////////////////
    void Lighting3DService::releasePointLight( uint32_t _slot )
    {
        if( _slot >= MENGINE_LIGHTING3D_POINT_LIGHT_MAX )
        {
            return;
        }

        m_pointLights[_slot].acquired = false;
        m_pointLights[_slot].invRadius = 0.f;
        m_pointLights[_slot].intensity = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void Lighting3DService::setPointLight( uint32_t _slot, const mt::vec3f & _pos, float _radius, const mt::vec3f & _color, float _intensity )
    {
        if( _slot >= MENGINE_LIGHTING3D_POINT_LIGHT_MAX )
        {
            return;
        }

        PointLightSlot & p = m_pointLights[_slot];
        p.pos = _pos;
        p.invRadius = (_radius > 1e-4f) ? (1.f / _radius) : 0.f;
        p.color = _color;
        p.intensity = _intensity;
    }
    //////////////////////////////////////////////////////////////////////////
    void Lighting3DService::packUploadBlock( float * _dst ) const
    {
        _dst[0] = m_ambient.x;
        _dst[1] = m_ambient.y;
        _dst[2] = m_ambient.z;
        _dst[3] = 0.f;

        _dst[4] = m_directionalDir.x;
        _dst[5] = m_directionalDir.y;
        _dst[6] = m_directionalDir.z;
        _dst[7] = m_hasDirectional ? 1.f : 0.f;

        _dst[8] = m_directionalColor.x;
        _dst[9] = m_directionalColor.y;
        _dst[10] = m_directionalColor.z;
        _dst[11] = m_hasDirectional ? m_directionalIntensity : 0.f;

        float * pos = _dst + 12;
        float * col = _dst + 12 + MENGINE_LIGHTING3D_POINT_LIGHT_MAX * 4;

        for( uint32_t i = 0; i != MENGINE_LIGHTING3D_POINT_LIGHT_MAX; ++i )
        {
            const PointLightSlot & p = m_pointLights[i];

            pos[i * 4 + 0] = p.pos.x;
            pos[i * 4 + 1] = p.pos.y;
            pos[i * 4 + 2] = p.pos.z;
            pos[i * 4 + 3] = (p.acquired == true) ? p.invRadius : 0.f;

            col[i * 4 + 0] = p.color.x;
            col[i * 4 + 1] = p.color.y;
            col[i * 4 + 2] = p.color.z;
            col[i * 4 + 3] = (p.acquired == true) ? p.intensity : 0.f;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
