/*
 *	Layer2DTexture.h
 *
 *	Created by _Berserk_ on 25.2.2010
 *	Copyright 2010 Menge. All rights reserved.
 *
 */

#pragma  once

#	include "Layer2D.h"

namespace Menge
{
	class Texture;

	class Layer2DTexture
		: public Layer2D
	{
	public:
		Layer2DTexture();
		~Layer2DTexture();

	public:
		void loader( BinParser * _parser ) override;
		void render( Camera2D * _camera ) override;

	public:
		void setRenderTargetName( const ConstString& _name );
		const ConstString& getRenderTargetName() const;

		void setViewportSize( const mt::vec2f& _size );
		const mt::vec2f& getViewportSize() const;

		void setCameraOffset( const mt::vec2f _offset );
		const mt::vec2f& getCameraOffset() const;

	protected:
		bool _activate() override;

	private:
		ConstString m_renderTargetName;
		mt::vec2f m_viewportSize;
		mt::vec2f m_cameraOffset;
	};
}	// namespace
