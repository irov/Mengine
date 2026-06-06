#include "DirectionalLight3D.h"

#include "Interface/Lighting3DServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DirectionalLight3D::DirectionalLight3D()
        : m_direction( 0.f, -1.f, 0.f )
        , m_color( 1.f, 1.f, 1.f )
        , m_ambient( 0.f, 0.f, 0.f )
        , m_intensity( 1.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DirectionalLight3D::~DirectionalLight3D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DirectionalLight3D::setLightDirection( const mt::vec3f & _dir )
    {
        m_direction = _dir;

        if( this->isActivate() == true )
        {
            this->apply_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & DirectionalLight3D::getLightDirection() const
    {
        return m_direction;
    }
    //////////////////////////////////////////////////////////////////////////
    void DirectionalLight3D::setLightColor( const mt::vec3f & _color )
    {
        m_color = _color;

        if( this->isActivate() == true )
        {
            this->apply_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & DirectionalLight3D::getLightColor() const
    {
        return m_color;
    }
    //////////////////////////////////////////////////////////////////////////
    void DirectionalLight3D::setLightIntensity( float _intensity )
    {
        m_intensity = _intensity;

        if( this->isActivate() == true )
        {
            this->apply_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float DirectionalLight3D::getLightIntensity() const
    {
        return m_intensity;
    }
    //////////////////////////////////////////////////////////////////////////
    void DirectionalLight3D::setLightAmbient( const mt::vec3f & _ambient )
    {
        m_ambient = _ambient;

        if( this->isActivate() == true )
        {
            this->apply_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & DirectionalLight3D::getLightAmbient() const
    {
        return m_ambient;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DirectionalLight3D::_activate()
    {
        this->apply_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DirectionalLight3D::_deactivate()
    {
        LIGHTING3D_SERVICE()
            ->clearDirectionalLight();

        LIGHTING3D_SERVICE()
            ->setAmbient( mt::vec3f( 0.f, 0.f, 0.f ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DirectionalLight3D::apply_()
    {
        LIGHTING3D_SERVICE()
            ->setAmbient( m_ambient );

        LIGHTING3D_SERVICE()
            ->setDirectionalLight( m_direction, m_color, m_intensity );
    }
    //////////////////////////////////////////////////////////////////////////
}
