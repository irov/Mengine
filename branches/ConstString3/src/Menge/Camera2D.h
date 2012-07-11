#	pragma once

#	include "Camera.h"

#	include "Node.h"

#	include "Core/Viewport.h"

//#	include "Math/mat4.h"

namespace Menge
{
	class Scene;
	class Viewport;

	class Observer;

	class Camera2D
		: public Camera
		, public Node
	{
	public:
		Camera2D();
		
	protected:
		bool _activate() override;
		void _deactivate() override;

	public:
		void setViewport( const Viewport & _viewport );

	public:
		const Viewport & getViewport() const override;
		const mt::mat4f & getProjectionMatrix() const override;
		const mt::mat4f & getViewMatrix() const override;

	public:
		inline size_t getCameraRevision() const;

	protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void invalidateMatrix_();

	protected:
		void updateMatrix_() const;

	protected:
		void notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution );

	protected:
		size_t m_cameraRevision;

		Viewport m_viewport;

		Observer * m_notifyChangeWindowResolution;

		mutable Viewport m_viewportWM;
		mutable mt::mat4f m_viewMatrixWM;
		mutable mt::mat4f m_projectionMatrixWM;

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
}
