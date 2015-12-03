#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/NotificationServiceInterface.h"

#	include "Kernel/Node.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class CameraIsometric
		: public Node
        , public RenderCameraInterface
	{
	public:
		CameraIsometric();
		
	protected:
		bool _activate() override;
		void _deactivate() override;

	public:
		void setRenderport( const Viewport & _viewport );

	public:
		const Viewport & getCameraRenderport() const override;

	public:
		const mt::mat4f & getCameraWorldMatrix() const override;
		const mt::mat4f & getCameraWorldMatrixInv() const override;

		const mt::mat4f & getCameraProjectionMatrix() const override;
		const mt::mat4f & getCameraProjectionMatrixInv() const override;

		const mt::mat4f & getCameraViewMatrix() const override;
		const mt::mat4f & getCameraViewMatrixInv() const override;

	public:
		const mt::box2f & getCameraBBoxWM() const override;

	public:
		bool isOrthogonalProjection() const override;

	public:
		inline uint32_t getCameraRevision() const;

	protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void invalidateMatrix_();
		void invalidateViewport_();

	protected:
		void updateMatrix_() const;
		void updateProjectionMatrix_() const;
		void updateBBoxWM_() const;

	protected:
		uint32_t m_cameraRevision;

		Viewport m_renderport;
		float m_zNear;
		float m_zFar;

		mutable mt::mat4f m_worldMatrix;
		mutable mt::mat4f m_worldMatrixInv;
			
		mutable mt::mat4f m_viewMatrix;
		mutable mt::mat4f m_viewMatrixInv;

		mutable mt::mat4f m_projectionMatrix;
		mutable mt::mat4f m_projectionMatrixInv;

		mutable mt::box2f m_bboxWM;

		mutable bool m_fixedRenderport;
		mutable bool m_invalidateProjectionMatrix;
		mutable bool m_invalidateMatrix;
		mutable bool m_invalidateBB;
	};
	//////////////////////////////////////////////////////////////////////////
	inline uint32_t CameraIsometric::getCameraRevision() const
	{
		return m_cameraRevision;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const Viewport & CameraIsometric::getCameraRenderport() const
	{
		if( m_invalidateProjectionMatrix == true )
		{
			this->updateProjectionMatrix_();
		}

		return m_renderport;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & CameraIsometric::getCameraWorldMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & CameraIsometric::getCameraWorldMatrixInv() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_worldMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & CameraIsometric::getCameraProjectionMatrix() const
	{
		if( m_invalidateProjectionMatrix == true )
		{
			this->updateProjectionMatrix_();
		}

		return m_projectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & CameraIsometric::getCameraProjectionMatrixInv() const
	{
		if( m_invalidateProjectionMatrix == true )
		{
			this->updateProjectionMatrix_();
		}

		return m_projectionMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & CameraIsometric::getCameraViewMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & CameraIsometric::getCameraViewMatrixInv() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::box2f & CameraIsometric::getCameraBBoxWM() const
	{
		if( m_invalidateBB == true )
		{
			this->updateBBoxWM_();
		}

		return m_bboxWM;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void CameraIsometric::invalidateMatrix_()
	{
		m_invalidateMatrix = true;
		m_invalidateBB = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void CameraIsometric::invalidateViewport_()
	{
		m_invalidateProjectionMatrix = true;
		m_invalidateBB = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool CameraIsometric::isOrthogonalProjection() const
	{
		return false;
	}
}
