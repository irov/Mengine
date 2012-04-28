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
		void setViewport( const Viewport & _viewport );
		inline const Viewport & getViewport();

		void setParallax( const mt::vec2f& _parallax );
		const mt::vec2f& getParallax() const;

		const mt::mat4f & Camera2D::getProjectionMatrix();
		const mt::mat4f & Camera2D::getViewMatrix();

	public:
		void setTargetNode( Node * _target );
		void setTargetOffset( Node * _offsetProvider );

	public:
		inline size_t getCameraRevision() const;

	protected:
		void _invalidateWorldMatrix() override;

	protected:
		void _update( float _timing ) override; 

	protected:
		void invalidateMatrix_();

	protected:
		void updateMatrix_();

	protected:
		Node * m_target;
		Node * m_offsetProvider;

		Viewport m_viewport;		

		mt::mat4f m_viewMatrix;
		mt::mat4f m_projectionMatrix;

		size_t m_cameraRevision;

		bool m_invalidateMatrix;
	};
	//////////////////////////////////////////////////////////////////////////
	inline size_t Camera2D::getCameraRevision() const
	{
		return m_cameraRevision;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const Viewport & Camera2D::getViewport()
	{
		return m_viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void Camera2D::invalidateMatrix_()
	{
		m_invalidateMatrix = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera2D::getProjectionMatrix()
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_projectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera2D::getViewMatrix()
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrix;
	}

}
