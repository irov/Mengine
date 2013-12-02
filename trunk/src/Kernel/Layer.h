#	pragma once

#	include "Config/Typedef.h"

#	include "Kernel/Node.h"

#	include "Math/vec2.h"
#	include "Math/vec4.h"

namespace Menge
{
	class Scene;
	class HotSpot;
	class Arrow;
	class Camera;

	class Layer
		: public Node
	{
	public:
		Layer();
		~Layer();

	public:
		void setMain( bool value );
		bool isMain() const;

	public:
		void setScene( Scene * _scene );
		Scene * getScene() override;
	
	public:
		void setSize( const mt::vec2f & _size );
		const mt::vec2f & getSize() const;

	public:
		void setLayer( Layer * _layer ) override;

    protected:
		virtual void _updateBoundingBox( mt::box2f& _boundingBox ) override;

	public:
		virtual void calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node* _node ) const;

	protected:
		bool m_main;
		mt::vec2f m_size;

		Scene * m_scene;
	};
}
