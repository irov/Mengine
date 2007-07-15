#	pragma once

#	include <string>

#	include "math/mat3.h"
#	include "math/vec4.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Holder.h"

namespace Menge
{
	class FileEngine;

	class RenderEngine
	{
	public:
		RenderEngine( RenderSystemInterface * _interface );
		
	public:

		void	setMaterialColor(unsigned char _ambient[4], 
		unsigned char _diffuse[4],
		unsigned char _specular[4]);

		bool beginScene(bool backBuffer, bool zBuffer, int color);
		bool endScene();

		void drawPrimitive(PrimitiveData * _pd);

		void drawLine(const mt::vec2f& p1, const mt::vec2f& p2, float width, unsigned long color);

		RenderImageInterface* loadImage(const std::string &	_imageFile, int _filter);

		void renderImage(
			const mt::mat3f& _transform, 
			const mt::vec2f& _offset,
			const mt::vec4f& _uv,
			const mt::vec2f& _size,
			unsigned int _mixedColor, 
			RenderImageInterface* _rmi);

		void releaseRenderImage( RenderImageInterface* _rmi );

		VertexData * createVertexData();
		IndexData * createIndexData();
		VertexDeclaration * createVertexDeclaration();

		Texture * createTextureInMemory(const TextureDesc& _desc);
		void	releaseTexture(Texture * _tex);

		void	setTexture(Texture * _tex);
		void	setProjectionMatrix(const mt::mat4f& _projection);
		void	setViewMatrix(const mt::mat4f& _view);
		void	setWorldMatrix(const mt::mat4f& _world);

		//////////////////////////////////////////////////////////////////////////
		RenderFontInterface* loadFont(const std::string &_fontXml);

		void renderText(
			const mt::vec2f & _pos, 
			RenderFontInterface* _font, 
			const std::string & _text);

		void releaseRenderFont( RenderFontInterface* _fnt );

		RenderImageInterface* loadImage( const TextureDesc& _desc );

	protected:
		RenderSystemInterface * m_interface;
	};
}