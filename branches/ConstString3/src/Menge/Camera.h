/*
 *	Camera.h
 *
 *	Created by _Berserk_ on 24.2.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Config/Typedef.h"
#	include "Math/mat4.h"
#	include "Core/Viewport.h"

namespace Menge
{
	class Camera
	{
	public:
		Camera();

	public:
		virtual const mt::mat4f& getViewMatrix() = 0;
		virtual const mt::mat4f& getProjectionMatrix() = 0;
		virtual const Viewport & getRenderViewport() = 0;
		virtual bool is3D() const = 0;

	public:
		void setRenderTarget( const ConstString& _renderTarget );
		inline const ConstString& getRenderTarget() const;

	private:
		ConstString m_renderTarget;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString& Camera::getRenderTarget() const
	{
		return m_renderTarget;
	}
}	// namespace Menge
