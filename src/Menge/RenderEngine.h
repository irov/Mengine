#	pragma once

#	include <string>

#	include "math/mat4.h"
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

		void render();

		RenderImageInterface * loadImage( const TextureDesc & _desc );
		RenderImageInterface * loadImage( const std::string & _imageFile, int _filter );

		void renderImage(
			const mt::mat3f & _transform, 
			const mt::vec2f & _offset,
			const mt::vec4f & _uv,
			const mt::vec2f & _size,
			unsigned int _mixedColor, 
			RenderImageInterface* _rii);

		void	releaseImage( RenderImageInterface* _rmi );

		void	setProjectionMatrix(const mt::mat4f& _projection);
		void	setViewMatrix(const mt::mat4f& _view);
		void	setWorldMatrix(const mt::mat4f& _world);

		void	beginLayer();
		void	endLayer();

		RenderFontInterface* loadFont(const std::string &_font);

		void renderText(
			const mt::vec2f & _pos, 
			RenderFontInterface* _font, 
			const std::string & _text);

		void releaseRenderFont( RenderFontInterface* _fnt );
		
				//void	setMaterialColor(unsigned char _ambient[4], 
		//unsigned char _diffuse[4],
		//unsigned char _specular[4]);
		
		//bool beginScene(int _color);
		//bool endScene();

		//void drawPrimitive(PrimitiveData * _pd);

		//void drawLine3D(const mt::vec3f& p1, const mt::vec3f& p2, unsigned long color);
		//void drawLine2D(const mt::vec2f& p1, const mt::vec2f& p2, unsigned long color);
		//void drawBox( const mt::vec3f & MinEdge, const mt::vec3f & MaxEdge, unsigned long _color);


		//VertexData * createVertexData();
		//IndexData * createIndexData();
		//VertexDeclaration * createVertexDeclaration();

		//void	setTexture(RenderImageInterface * _tex);

		
		

	protected:
		RenderSystemInterface * m_interface;
	};
}