#include "PointLight3D.h"

#include "Interface/Lighting3DServiceInterface.h"
#include "Interface/TransformationInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PointLight3D::PointLight3D()
        : m_radius( 1.f )
        , m_color( 1.f, 1.f, 1.f )
        , m_intensity( 1.f )
        , m_slot( UINT32_MAX )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PointLight3D::~PointLight3D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PointLight3D::setLightRadius( float _radius )
    {
        m_radius = _radius;

        if( this->isActivate() == true )
        {
            this->apply_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float PointLight3D::getLightRadius() const
    {
        return m_radius;
    }
    //////////////////////////////////////////////////////////////////////////
    void PointLight3D::setLightColor( const mt::vec3f & _color )
    {
        m_color = _color;

        if( this->isActivate() == true )
        {
            this->apply_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & PointLight3D::getLightColor() const
    {
        return m_color;
    }
    //////////////////////////////////////////////////////////////////////////
    void PointLight3D::setLightIntensity( float _intensity )
    {
        m_intensity = _intensity;

        if( this->isActivate() == true )
        {
            this->apply_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float PointLight3D::getLightIntensity() const
    {
        return m_intensity;
    }
    //////////////////////////////////////////////////////////////////////////
    void PointLight3D::updateLightFromTransform()
    {
        if( this->isActivate() == false )
        {
            return;
        }

        this->apply_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PointLight3D::_activate()
    {
        m_slot = LIGHTING3D_SERVICE()
            ->acquirePointLight();

        if( m_slot == UINT32_MAX )
        {
            return true;
        }

        this->apply_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PointLight3D::_deactivate()
    {
        if( m_slot == UINT32_MAX )
        {
            return;
        }

        LIGHTING3D_SERVICE()
            ->releasePointLight( m_slot );

        m_slot = UINT32_MAX;
    }
    //////////////////////////////////////////////////////////////////////////
    void PointLight3D::apply_()
    {
        if( m_slot == UINT32_MAX )
        {
            return;
        }

        const TransformationInterface * transformation = this->getTransformation();
        const mt::mat4f & wm = transformation->getWorldMatrix();
        mt::vec3f worldPos( wm.v3.x, wm.v3.y, wm.v3.z );

        LIGHTING3D_SERVICE()
            ->setPointLight( m_slot, worldPos, m_radius, m_color, m_intensity );
    }
    //////////////////////////////////////////////////////////////////////////
}
