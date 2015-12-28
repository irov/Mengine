#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/NotificationServiceInterface.h"

#	include "Kernel/Node.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class Camera2D
		: public Node
        , public RenderCameraInterface
	{
	public:
		Camera2D();
		
	protected:
		bool _activate() override;
		void _deactivate() override;

	public:
		void setCameraPosition( const mt::vec3f & _pos );
		void setCameraDirection( const mt::vec3f & _dir );
		void setCameraUp( const mt::vec3f & _up );
		void setCameraRightSign( float _rightSign );

	public:
		void setRenderport( const Viewport & _viewport );
		const Viewport & getRenderport() const;
		
		
	public:
		void setFixedRenderport( bool _value );
		bool getFixedRenderport() const;

	public:
		const Viewport & getCameraRenderport() const override;

	public:
		const mt::mat4f & getCameraViewMatrix() const override;
		const mt::mat4f & getCameraViewMatrixInv() const override;

		const mt::mat4f & getCameraProjectionMatrix() const override;
		const mt::mat4f & getCameraProjectionMatrixInv() const override;

	public:
		const mt::box2f & getCameraBBoxWM() const override;

	public:
		void setOrthogonalProjection( bool _orthogonalProjection );
		bool isOrthogonalProjection() const override;

	protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void invalidateViewMatrix_();
		void invalidateProjectionMatrix_();

	protected:
		void updateMatrix_() const;
		void updateProjectionMatrix_() const;
		void updateBBoxWM_() const;

	protected:
		void notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution );

	protected:
		mt::vec3f m_cameraPosition;
		mt::vec3f m_cameraDirection;
		mt::vec3f m_cameraUp;
		float m_cameraRightSign;

		float m_zNear;
		float m_zFar;

		Viewport m_renderport;

		ObserverInterface * m_observerChangeWindowResolution;

		mutable mt::mat4f m_viewMatrix;
		mutable mt::mat4f m_viewMatrixInv;

		mutable mt::mat4f m_projectionMatrix;
		mutable mt::mat4f m_projectionMatrixInv;

		mutable mt::box2f m_bboxWM;

		mutable bool m_fixedRenderport;
		mutable bool m_invalidateProjectionMatrix;
		mutable bool m_invalidateMatrix;
		mutable bool m_invalidateBB;

		bool m_orthogonalProjection;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const Viewport & Camera2D::getCameraRenderport() const
	{
		return m_renderport;
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
	inline const mt::mat4f & Camera2D::getCameraProjectionMatrixInv() const
	{
		if( m_invalidateProjectionMatrix == true )
		{
			this->updateProjectionMatrix_();
		}

		return m_projectionMatrixInv;
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
	inline const mt::mat4f & Camera2D::getCameraViewMatrixInv() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::box2f & Camera2D::getCameraBBoxWM() const
	{
		if( m_invalidateBB == true )
		{
			this->updateBBoxWM_();
		}

		return m_bboxWM;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void Camera2D::invalidateViewMatrix_()
	{
		m_invalidateMatrix = true;
		m_invalidateBB = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void Camera2D::invalidateProjectionMatrix_()
	{
		m_invalidateProjectionMatrix = true;
		m_invalidateBB = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Camera2D::isOrthogonalProjection() const
	{
		return m_orthogonalProjection;
	}
}
