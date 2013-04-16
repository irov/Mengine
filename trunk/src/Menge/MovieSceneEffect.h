#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class MovieSceneEffect
		: public Node
	{
    public:
        MovieSceneEffect();

    public:
        void setPropagateNode( Node * _propagate );

    protected:
        void _invalidateColor() override;

    protected:
        void _invalidateWorldMatrix() override;

    protected:
        Node * m_propagate;
	};
}