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

namespace Menge
{
	class Camera
	{
	public:
		virtual const mt::mat4f& getViewMatrix() = 0;
		virtual const mt::mat4f& getProjectionMatrix() = 0;
		virtual const mt::vec4f& getRenderArea() = 0;
		virtual bool is3D() const = 0;

	public:
		void setRenderTarget( const String& _renderTarget )
		{
			m_renderTarget = _renderTarget;
		}
		const String& getRenderTarget() const
		{
			return m_renderTarget;
		}

	private:
		String m_renderTarget;
	};
}	// namespace Menge
