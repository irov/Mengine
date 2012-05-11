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
		size_t m_cameraRevision;

		Viewport m_viewport;	

		mutable Viewport m_viewportWM;
		mutable mt::mat4f m_viewMatrix;
		mutable mt::mat4f m_projectionMatrix;

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
