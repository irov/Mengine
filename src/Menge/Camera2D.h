#	pragma once

#	include "Node.h"
#	include "Viewport.h"

#	include "Math/mat4.h"

namespace Menge
{
	class Scene;
	class Viewport;

	class Camera2D
		: public Node
	{
		OBJECT_DECLARE( Camera2D )
	public:
		Camera2D();
		~Camera2D();

	public:
		const Viewport & getViewport();

	public:
		void setViewportSize( const mt::vec2f & _size );

		void setTarget( Node* _target );
		void enableTargetFollowing( bool _enable, float _force );
		void setBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower );
		
		const mt::mat4f& getViewMatrix();

		void setParallax( const mt::vec2f& _parallax );
		const mt::vec2f& getParallax() const;

	public:
		void loader( XmlElement * _xml ) override;

		void applyView();

	protected:
		bool _activate() override;
		void _update( float _timing ) override;
		void _invalidateWorldMatrix() override;

	protected:
		void updateViewport();
		void invalidateViewport();
		bool isInvalidateViewport() const;

	private:
		void updateViewMatrix();
		void invalidateViewMatrix();
		bool isInvalidateViewMatrix() const;

	protected:
		Viewport m_viewport;
		bool m_invalidateViewport;

		mt::vec2f m_viewportSize;

		Node* m_target;
		bool m_targetFollowing;
		float m_followingForce;
		mt::vec2f m_boundLeftUpper;
		mt::vec2f m_boundRightLower;

		mt::mat4f m_viewMatrix;
		bool m_invalidateViewMatrix;

		mt::vec2f m_parallax;
	};
}