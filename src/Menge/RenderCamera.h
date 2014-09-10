#	pragma once

#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	class Viewport;
	
	class RenderCamera
        : public RenderCameraInterface
	{
	public:
		RenderCamera();
		
	public:
		void initialize( const mt::mat4f & _wm, const mt::mat4f & _pm, const mt::mat4f & _vm, const Viewport & _renderport, const ConstString & _target, bool _isOrthogonalProjection );

	protected:
		const mt::mat4f & getCameraWorldMatrix() const override;
		const mt::mat4f & getCameraWorldMatrixInv() const override;
		const mt::mat4f & getCameraProjectionMatrix() const override;
		const mt::mat4f & getCameraProjectionMatrixInv() const override;
		const mt::mat4f & getCameraViewMatrix() const override;
		const mt::mat4f & getCameraViewMatrixInv() const override;

	public:
		const mt::box2f & getCameraBBoxWM() const override; 

	protected:
		const Viewport & getCameraRenderport() const override;

	protected:
		const ConstString & getRenderTarget() const override;

	protected:
		bool isOrthogonalProjection() const override;

	protected:
		mt::mat4f m_worldMatrix;
		mt::mat4f m_worldMatrixInv;
		mt::mat4f m_projectionMatrix;
		mt::mat4f m_projectionMatrixInv;
		mt::mat4f m_viewMatrix;
		mt::mat4f m_viewMatrixInv;

		mt::box2f m_bboxWM;

		Viewport m_renderport;

		ConstString m_renderTarget;
		bool m_isOrthonalProjection;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCamera::getCameraWorldMatrix() const
	{
		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCamera::getCameraWorldMatrixInv() const
	{
		return m_worldMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCamera::getCameraProjectionMatrix() const
	{
		return m_projectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCamera::getCameraProjectionMatrixInv() const
	{
		return m_projectionMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCamera::getCameraViewMatrix() const
	{
		return m_viewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCamera::getCameraViewMatrixInv() const
	{
		return m_viewMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::box2f & RenderCamera::getCameraBBoxWM() const
	{
		return m_bboxWM;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const Viewport & RenderCamera::getCameraRenderport() const
	{
		return m_renderport;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & RenderCamera::getRenderTarget() const
	{
		return m_renderTarget;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool RenderCamera::isOrthogonalProjection() const
	{
		return m_isOrthonalProjection;
	}
}
