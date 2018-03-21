#pragma once

#include "DX9RenderTarget.h"

#include "Core/ServantBase.h"

#include <d3d9.h>

namespace Mengine
{
	class DX9RenderTargetOffscreen
		: public DX9RenderTarget
	{
	public:
		DX9RenderTargetOffscreen();
		~DX9RenderTargetOffscreen() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool getData( unsigned char * _buffer, size_t _pitch ) override;

	protected:
		LPDIRECT3DSURFACE9 m_surfacePlain;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<DX9RenderTargetOffscreen> DX9RenderTargetOffscreenPtr;
}