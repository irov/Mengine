#	include "MovieSceneEffect.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MovieSceneEffect::MovieSceneEffect()
        : m_propagate(nullptr)
    {
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
		
        mt::vec3f position;
        mt::vec3f origin;

        mt::vec3f scale;
        mt::vec3f rotation;

        this->getTransformation( position, origin, scale, rotation );

        m_propagate->setTransformation( position, origin, scale, rotation );
    }
}