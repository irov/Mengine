#pragma once

#include "Interface/RenderCameraInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
	class RenderCameraProxy
        : public Factorable
        , public RenderCameraInterface
	{
	public:
		RenderCameraProxy();
        ~RenderCameraProxy() override;
		
	public:
		void initialize( const mt::mat4f & _pm, const mt::mat4f & _vm );

	protected:
		const mt::mat4f & getCameraViewMatrix() const override;
		const mt::mat4f & getCameraViewMatrixInv() const override;
		const mt::mat4f & getCameraProjectionMatrix() const override;
		const mt::mat4f & getCameraProjectionMatrixInv() const override;

	protected:
		const mt::mat4f & getCameraViewProjectionMatrix() const override;
        const mt::mat4f & getCameraViewProjectionMatrixInv() const override;

	protected:		
		mt::mat4f m_viewMatrix;
		mt::mat4f m_viewMatrixInv;
		mt::mat4f m_projectionMatrix;
		mt::mat4f m_projectionMatrixInv;

		mt::mat4f m_viewProjectionMatrix;
        mt::mat4f m_viewProjectionMatrixInv;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCameraProxy::getCameraViewMatrixInv() const
	{
		return m_viewMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCameraProxy::getCameraProjectionMatrix() const
	{
		return m_projectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCameraProxy::getCameraProjectionMatrixInv() const
	{
		return m_projectionMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCameraProxy::getCameraViewMatrix() const
	{
		return m_viewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCameraProxy::getCameraViewProjectionMatrix() const
	{ 
		return m_viewProjectionMatrix;
	}
    //////////////////////////////////////////////////////////////////////////
    inline const mt::mat4f & RenderCameraProxy::getCameraViewProjectionMatrixInv() const
    {
        return m_viewProjectionMatrixInv;
    }
}
