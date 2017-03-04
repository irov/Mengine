#	include "MovieSceneEffect.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MovieSceneEffect::MovieSceneEffect()
        : m_propagate(nullptr)
		, m_propagateEnable(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MovieSceneEffect::~MovieSceneEffect()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	void MovieSceneEffect::enablePropagate( bool _value )
	{
		m_propagateEnable = _value;

		if( m_propagateEnable == true )
		{
			m_propagate->getTransformation( m_position, m_origin, m_scale, m_skew, m_rotation );

			mt::vec3f position;
			mt::vec3f origin;

			mt::vec3f scale;
			mt::vec2f skew;
			mt::vec3f rotation;

			this->getTransformation( position, origin, scale, skew, rotation );

			m_propagate->setTransformation( position, origin, scale, skew, rotation );
		}
		else
		{
			m_propagate->setTransformation( m_position, m_origin, m_scale, m_skew, m_rotation );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void MovieSceneEffect::setPropagateNode( Node * _propagate )
    {
        m_propagate = _propagate;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSceneEffect::_invalidateColor()
    {
        if( m_propagate == nullptr )
		{
			return;
		}

		const ColourValue & personal_colour = this->getPersonalColor();

		m_propagate->setLocalColor( personal_colour );	
	}
    //////////////////////////////////////////////////////////////////////////
    void MovieSceneEffect::_invalidateWorldMatrix()
    {
		if( m_propagate == nullptr )
		{
			return;
		}

		if( m_propagateEnable == false )
		{
			return;
		}
		
        mt::vec3f position;
        mt::vec3f origin;

        mt::vec3f scale;
		mt::vec2f skew;
        mt::vec3f rotation;

		this->getTransformation( position, origin, scale, skew, rotation );

		m_propagate->setTransformation( position, origin, scale, skew, rotation );
    }
}