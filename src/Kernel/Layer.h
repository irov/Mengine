#	pragma once

#	include "Config/Typedef.h"

#	include "Kernel/Node.h"

#	include "Math/vec2.h"
#	include "Math/vec4.h"

namespace Menge
{
	class Scene;
	class Arrow;

	class Layer
		: public Node
	{
		DECLARE_VISITABLE( Node )

	public:
		Layer();
		~Layer();

	protected:
		bool _activate() override;
		
	protected:
		void _addChild( Node * _node ) override;
		void _changeParent( Node * _oldParent, Node * _newParent ) override;

    protected:
		virtual void _updateBoundingBox( mt::box2f& _boundingBox ) const override;

	public:
		virtual void calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node* _node ) const;
	};
}
