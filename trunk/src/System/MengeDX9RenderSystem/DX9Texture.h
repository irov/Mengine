/*
 *	DX9Texture.h
 *
 *	Created by _Berserk_ on 23.05.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/RenderSystemInterface.h"

struct IDirect3DTexture9;

namespace Menge
{
	class DX9Texture
		: public RenderImageInterface
	{
	public:
		DX9Texture( IDirect3DTexture9* _d3dInterface );
		virtual ~DX9Texture();

		IDirect3DTexture9* getInterface();

		size_t getWidth() const;
		size_t getHeight() const;

		virtual void release();
	public:
	public:
		unsigned char * lock( int* _pitch, const Rect& _rect, bool _readOnly ) override;
		void unlock() override;

	public:
		void destroy() override;

	protected:
		IDirect3DTexture9* m_d3dInterface;
		size_t m_hwWidth;
		size_t m_hwHeight;
	};
}	// namespace Menge
