#	include "MovieSceneEffect.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MovieSceneEffect::MovieSceneEffect()
        : m_propagate(NULL)
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
        const ColourValue & local_colour = this->getLocalColor();

        m_propagate->setLocalColor( local_colour );

        const ColourValue & personal_colour = this->getPersonalColor();

        m_propagate->setPersonalColor( personal_colour );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSceneEffect::_invalidateWorldMatrix()
    {
        mt::vec3f position;
        mt::vec3f origin;
        mt::vec3f coordinate;

        mt::vec3f scale;
        mt::vec3f rotation;

        this->getTransformation( position, origin, coordinate, scale, rotation );

        m_propagate->setTransformation( position, origin, coordinate, scale, rotation );
    }
}