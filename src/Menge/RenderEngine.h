#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Viewport.h"

#	include "Holder.h"

#	include "math/mat4.h"
#	include "math/vec4.h"

#	include <string>
#	include <vector>

namespace Menge
{
	class FileEngine;
	class Camera3D;

	class RenderEngine
	{
	public:
		RenderEngine( RenderSystemInterface * _interface );
		
	public:
		void render();

		void setRenderViewport( const Viewport & _viewport );
		const Viewport & getRenderViewport() const;

		void setRenderCamera( Camera3D * _camera );
		Camera3D * getRenderCamera();

		RenderImageInterface * loadImage( const TextureDesc & _desc );
		RenderImageInterface * loadImage( const std::string & _fileName, std::vector<char> & _buff, size_t _filter );

		void renderImage(
			const mt::mat3f & _transform, 
			const mt::vec2f & _offset,
			const mt::vec4f & _uv,
			const mt::vec2f & _size,
			unsigned int _color, 
			const RenderImageInterface* _image);

		void	releaseImage( RenderImageInterface* _rmi );

		void	setProjectionMatrix( const mt::mat4f& _projection );
		void	setViewMatrix( const mt::mat4f& _view );
		void	setWorldMatrix( const mt::mat4f& _world );

		void	beginLayer();
		void	endLayer();

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
		Viewport m_renderViewport;
		Camera3D * m_renderCamera;
	};
}