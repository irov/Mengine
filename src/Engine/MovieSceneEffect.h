#pragma once

#include "Kernel/Node.h"

namespace Mengine
{
	class MovieSceneEffect
		: public Node
	{
    public:
        MovieSceneEffect();
        ~MovieSceneEffect() override;

    public:
        void setPropagateNode( Node * _propagate );

	public:
		void enablePropagate( bool _value );

    protected:
        void _invalidateColor() override;

    protected:
        void _invalidateWorldMatrix() override;

    protected:
        Node * m_propagate;

		bool m_propagateEnable;

		mt::vec3f m_position;
		mt::vec3f m_origin;

		mt::vec3f m_scale;
		mt::vec2f m_skew;
		mt::vec3f m_rotation;
	};
}