#	pragma once

#	include "Camera.h"

#	include "Node.h"
#	include "Core/Viewport.h"

//#	include "Math/mat4.h"

namespace Menge
{
	class Scene;
	class Viewport;

	class Camera2D
		: public Camera
		, public Node
	{
	public:
		Camera2D();
		~Camera2D();

	public:
		const mt::mat4f & getViewMatrix() override;
		const mt::mat4f & getProjectionMatrix() override;
		const Viewport & getRenderViewport() override;
		bool is3D() const override;

	public:
		inline const Viewport & getViewport();
		inline bool isInvalidateViewport() const;

	public:
		void setViewportSize( const mt::vec2f & _size );
		const mt::vec2f& getViewportSize() const;

		void setTarget( Node* _target );
		void enableTargetFollowing( bool _enable, float _force );
		void setBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower );
		

		void setParallax( const mt::vec2f& _parallax );
		const mt::vec2f& getParallax() const;
		void setOffset( const mt::vec2f& _offset );
		const mt::vec2f& getOffset() const;

		void setRenderViewport( const Viewport & _viewport );

	public:
		void loader( XmlElement * _xml ) override;

	public:
		inline std::size_t getCameraRevision() const;

	protected:
		bool _activate() override;
		void _update( float _timing ) override;
		void _invalidateWorldMatrix() override;

	protected:
		void updateViewport();
		void invalidateViewport();		

	private:
		inline void updateViewMatrix();
		inline void invalidateViewMatrix();
		inline bool isInvalidateViewMatrix() const;

	protected:
		Viewport m_viewport;
		bool m_invalidateViewport;

		mt::vec2f m_viewportSize;

		Node* m_target;
		bool m_targetFollowing;
		bool m_boundsEnabled;
		float m_followingForce;
		mt::vec2f m_boundLeftUpper;
		mt::vec2f m_boundRightLower;

		mt::mat4f m_viewMatrix;
		bool m_invalidateViewMatrix;

		mt::vec2f m_parallax;
		mt::vec2f m_offset;

		mt::mat4f m_projectionMatrix;
		Viewport m_renderViewport;

		std::size_t m_cameraRevision;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Camera2D::isInvalidateViewport() const
	{
		return m_invalidateViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void Camera2D::invalidateViewMatrix()
	{
		m_invalidateViewMatrix = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Camera2D::isInvalidateViewMatrix() const
	{
		return m_invalidateViewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline std::size_t Camera2D::getCameraRevision() const
	{
		return m_cameraRevision;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const Viewport & Camera2D::getViewport()
	{
		if( isInvalidateViewport() == true )
		{
			updateViewport();			
		}

		return m_viewport;
	}

}
