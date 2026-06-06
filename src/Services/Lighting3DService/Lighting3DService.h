#pragma once

#include "Interface/Lighting3DServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Lighting3DService
        : public ServiceBase<Lighting3DServiceInterface>
    {
        DECLARE_FACTORABLE( Lighting3DService );
        DECLARE_LIFECYCLEABLE();

    public:
        Lighting3DService();
        ~Lighting3DService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setAmbient( const mt::vec3f & _color ) override;
        const mt::vec3f & getAmbient() const override;

    public:
        void setDirectionalLight( const mt::vec3f & _dir, const mt::vec3f & _color, float _intensity ) override;
        void clearDirectionalLight() override;
        bool hasDirectionalLight() const override;

    public:
        uint32_t acquirePointLight() override;
        void releasePointLight( uint32_t _slot ) override;

        void setPointLight( uint32_t _slot, const mt::vec3f & _pos, float _radius, const mt::vec3f & _color, float _intensity ) override;

    public:
        void packUploadBlock( float * _dst ) const override;

    protected:
        struct PointLightSlot
        {
            mt::vec3f pos;
            float invRadius;
            mt::vec3f color;
            float intensity;
            bool acquired;
        };

    protected:
        mt::vec3f m_ambient;

        bool m_hasDirectional;
        mt::vec3f m_directionalDir;
        mt::vec3f m_directionalColor;
        float m_directionalIntensity;

        PointLightSlot m_pointLights[MENGINE_LIGHTING3D_POINT_LIGHT_MAX];
    };
    //////////////////////////////////////////////////////////////////////////
}
