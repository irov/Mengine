#	pragma once

#	include "Kernel/ResourceImage.h"

#	include "Kernel/Node.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class Node;
	class RenderViewportInterface;
	class RenderCameraInterface;

	class ResourceImageRender
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageRender )

	public:
		ResourceImageRender();

	public:
		void setWidth( uint32_t _width );
		uint32_t getWidth() const;

		void setHeight( uint32_t _height );
		uint32_t getHeight() const;

		void setChannels( uint32_t _channels );
		uint32_t getChannels() const;

		void setDepth( uint32_t _depth );
		uint32_t getDepth() const;

		void setPixelFormat( PixelFormat _format );
		PixelFormat getPixelFormat() const;

	public:
		bool bake( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane, Node * _node, bool _clear );

	protected:
		bool _compile() override;

	protected:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_channels;
		uint32_t m_depth;

		PixelFormat m_format;
	};
}