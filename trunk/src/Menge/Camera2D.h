#	pragma once

#	include "Camera.h"

#	include "Kernel/Node.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class Scene;
	class Viewport;

	class Observer;

	class Camera2D
		: public Node
        , public Camera
	{
	public:
		Camera2D();
		
	protected:
		bool _activate() override;
		void _deactivate() override;

	public:
		void setRenderport( const Viewport & _viewport );

	public:
		const Viewport & getRenderport() const override;

	public:
		const mt::mat4f & getProjectionMatrix() const override;
		const mt::mat4f & getViewMatrix() const override;

	public:
		bool isOrthogonalProjection() const override;

	public:
		inline size_t getCameraRevision() const;

	protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void invalidateMatrix_();
		void invalidateViewport_();

	protected:
		void updateMatrix_() const;
		void updateProjectionMatrix_() const;

	protected:
		void notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution );

	protected:
		size_t m_cameraRevision;

		Viewport m_renderport;

		Observer * m_observerChangeWindowResolution;

		mutable mt::mat4f m_viewMatrix;
		mutable mt::mat4f m_projectionMatrix;

		mutable bool m_invalidateProjectionMatrix;
		mutable bool m_invalidateMatrix;
	};
	//////////////////////////////////////////////////////////////////////////
	inline size_t Camera2D::getCameraRevision() const
	{
		return m_cameraRevision;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void Camera2D::invalidateMatrix_()
	{
		m_invalidateMatrix = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void Camera2D::invalidateViewport_()
	{
		m_invalidateProjectionMatrix = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Camera2D::isOrthogonalProjection() const
	{
		return true;
	}
}
