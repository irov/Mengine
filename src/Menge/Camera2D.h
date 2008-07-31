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
		const Viewport & getViewport() const;

	public:
		void setViewportSize( const mt::vec2f & _size );

		void setTarget( Node* _target );
		void enableTargetFollowing( bool _enable, float _force );
		void setBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower );
		
		const mt::mat4f& getViewMatrix();
		void setViewOrigin( const mt::vec2f& _origin );
	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _activate() override;
		void _update( float _timing ) override;
		void _changePivot() override;

	protected:
		virtual void updateViewport();

	protected:
		Viewport m_viewport;

		mt::vec2f m_viewportSize;

		Node* m_target;
		bool m_targetFollowing;
		float m_followingForce;
		mt::vec2f m_boundLeftUpper;
		mt::vec2f m_boundRightLower;

		bool m_viewMatrixUpdated;
		mt::mat4f m_viewMatrix;
		void updateViewMatrix_();
		mt::vec2f m_viewOrigin;
	};
}