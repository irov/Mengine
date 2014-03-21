#	pragma once

#	include "Camera.h"

#	include "Interface/NotificationServiceInterace.h"

#	include "Kernel/Node.h"

#	include "Core/Viewport.h"

namespace Menge
{
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
		void setWidescreenSupport( bool _value );
		bool getWidescreenSupport() const;

	public:
		const Viewport & getCameraRenderport() const override;

	public:
		const mt::mat4f & getCameraWorldMatrix() const override;
		const mt::mat4f & getCameraProjectionMatrix() const override;
		const mt::mat4f & getCameraViewMatrix() const override;

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

		mt::mat4f m_worldMatrix;

		bool m_widescreenSupport;
		
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
	inline const Viewport & Camera2D::getCameraRenderport() const
	{
		if( m_invalidateProjectionMatrix == true )
		{
			this->updateProjectionMatrix_();
		}

		return m_renderport;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera2D::getCameraWorldMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera2D::getCameraProjectionMatrix() const
	{
		if( m_invalidateProjectionMatrix == true )
		{
			this->updateProjectionMatrix_();
		}

		return m_projectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera2D::getCameraViewMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrix;
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
