#	pragma once

#	include "Camera.h"

#	include "Kernel/Node.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class Observer;

	class Camera3D
        : public Node
		, public Camera		
	{
	public:
		Camera3D();

	protected:
		bool _activate() override;
		void _deactivate() override;

	public:
		void setViewport( const Viewport & _viewport );

	public:
		void setCameraInterest( const mt::vec3f & _pos );
		void setCameraFOV( float _fov );
		void setCameraAspect( float _aspect );

	public:
		const Viewport & getViewport() const override;
		const mt::mat4f & getProjectionMatrix() const override;
		const mt::mat4f & getViewMatrix() const override;

	protected:
		void _invalidateWorldMatrix() override;

	protected:
		void invalidateMatrix_();

	protected:
		void updateMatrix_() const;

	protected:
		void notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution );
		
	protected:
		Viewport m_viewport;

		mt::vec3f m_cameraInterest;
		float m_cameraFOV;
		float m_cameraAspect;

		Observer * m_notifyChangeWindowResolution;

		mutable Viewport m_viewportWM;
		mutable mt::mat4f m_viewMatrixWM;
		mutable mt::mat4f m_projectionMatrixWM;
		
		mutable bool m_invalidateMatrix;
	};
	//////////////////////////////////////////////////////////////////////////
	inline void Camera3D::invalidateMatrix_()
	{
		m_invalidateMatrix = true;
	}
}
