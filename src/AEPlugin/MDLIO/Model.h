#	pragma once

#	include "Render.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"

#	include <d3d9.h>

#	include <stdint.h>

namespace Menge
{
	struct Model3DFrame
	{
		mt::vec3f cameraRight;
		mt::vec3f cameraUp;
		mt::vec3f cameraDir;
		mt::vec3f cameraPos;

		mt::vec3f * pos;
		mt::vec2f * uv;
		uint16_t * indecies;
	};

	class Model
	{
	public:
		Model( LPDIRECT3DDEVICE9 _pDevice );
		~Model();

	public:
		bool load( unsigned char * _buffer, size_t _size );
		
	public:
		void update( double _timing );

	public:
		void beginRender();

	public:
		double getTime() const;
		double getDuration() const;

	public:
		const Model3DFrame & getCurrentFrame() const;

		uint32_t getVertexCount() const;
		uint32_t getIndicesCount() const;
		float getCameraFOV() const;
		float getCameraWidth() const;
		float getCameraHeight() const;
		
	protected:
		LPDIRECT3DDEVICE9 m_pDevice;

		LPDIRECT3DTEXTURE9 m_pTexture;

		double m_timing;

		uint32_t m_frameCount;
		uint32_t m_vertexCount;
		uint32_t m_indicesCount;

		float m_frameDelay;

		float m_cameraFOV;
		float m_cameraWidth;
		float m_cameraHeight;

		Model3DFrame * m_model3DFrames;
	};
}