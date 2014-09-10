#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class Camera
		: public RenderCameraInterface
	{
	public:
		Camera();

	public:
		void setRenderTarget( const ConstString& _renderTarget );

	public:
		const ConstString & getRenderTarget() const override;

	private:
		ConstString m_renderTarget;
	};
}	// namespace Menge
