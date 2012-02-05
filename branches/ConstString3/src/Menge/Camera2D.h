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

		void setRenderport( const Viewport & _renderport );
		inline const Viewport & getRenderport() const;

		void setParallax( const mt::vec2f& _parallax );
		const mt::vec2f& getParallax() const;

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
		void invalidateViewport();

	protected:
		void updateViewport_();

	protected:
		Node * m_target;
		Node * m_offsetProvider;

		Viewport m_viewport;
		bool m_invalidateViewport;

		Viewport m_renderport;

		mt::vec2f m_parallax;

		size_t m_cameraRevision;
	};
	//////////////////////////////////////////////////////////////////////////
	inline size_t Camera2D::getCameraRevision() const
	{
		return m_cameraRevision;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const Viewport & Camera2D::getViewport()
	{
		if( m_invalidateViewport == true )
		{
			updateViewport_();
		}

		return m_viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const Viewport & Camera2D::getRenderport() const
	{
		return m_renderport;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void Camera2D::invalidateViewport()
	{
		m_invalidateViewport = true;
	}

}
