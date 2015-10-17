#	pragma once

#	include "Math/vec2.h"
#	include "Math/vec3.h"

#	include <Windows.h>
#	include <d3d9.h>

#	include <string>

#	include <stdint.h>

namespace Menge
{
	struct Vertex
	{
		mt::vec3f pos;
		uint32_t color;
		mt::vec2f uv[8];
	};

	typedef uint32_t Indices;

	class Render
	{
	public:
		Render();
		virtual ~Render();

	public:
		virtual bool initialize();
		virtual void finalize();
		
	public:
		bool test();

	public:
		bool beginScene();
		void endScene();

	public:
		void setViewport( DWORD _vw, DWORD _vh );

	public:
		bool lockVertexBuffer( UINT _count, Vertex ** _vertices );
		void unlockVertexBuffer();

		bool lockIndicesBuffer( UINT _count, Indices ** _indices );
		void unlockIndicesBuffer();
		
		void drawIndexedPrimitive( INT _baseVertexIndex, UINT _numVertices, UINT _primCount );
		
	public:
		bool getRenderData( unsigned char * _buffer, size_t _pitch, DWORD _width, DWORD _height );

	public:
		bool getRegValue( const std::wstring & _path, const std::wstring & _key, std::wstring & _value ) const;

	protected:
		virtual bool initResource_();
		virtual void initStates_();
		virtual void releaseResource_();
		virtual bool resetDevice_();

	protected:
		HWND m_hwnd;

		HMODULE m_hd3d9;

		IDirect3D9 * m_pD3D;
		D3DPRESENT_PARAMETERS m_d3d9pp;
		LPDIRECT3DDEVICE9 m_device;
		
		LPDIRECT3DVERTEXBUFFER9 m_vertexBuffer;
		LPDIRECT3DINDEXBUFFER9 m_indexBuffer;

		LPDIRECT3DTEXTURE9 m_renderTexture;		
		LPDIRECT3DSURFACE9 m_surfacePlain;
		LPDIRECT3DSURFACE9 m_surface;
	};
}