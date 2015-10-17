#	include "Model.h"

#	include "stdex/memory_reader.h"

#	include "Math/mat4.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
#	define DXCALL
	//////////////////////////////////////////////////////////////////////////
	static bool isTexturePOW2( uint32_t n )
	{
		return !(n & (n - 1));
	}
	//////////////////////////////////////////////////////////////////////////
	static uint32_t getTexturePOW2( uint32_t n )
	{
		--n;
		n |= n >> 16;
		n |= n >> 8;
		n |= n >> 4;
		n |= n >> 2;
		n |= n >> 1;
		++n;

		return n;
	}
	//////////////////////////////////////////////////////////////////////////
	Model::Model( LPDIRECT3DDEVICE9 _pDevice )
		: m_pDevice( _pDevice )
		, m_pTexture( NULL )
		, m_timing( 0.f )
		, m_frameCount( 0 )
		, m_vertexCount( 0 )
		, m_indicesCount( 0 )
		, m_frameDelay( 0.f )
		, m_cameraFOV( 0.f )
		, m_cameraWidth( 0.f )
		, m_cameraHeight( 0.f )
		, m_model3DFrames( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Model::~Model()
	{
		for( uint32_t i = 0; i != m_frameCount; ++i )
		{
			Model3DFrame & frame = m_model3DFrames[i];

			delete [] frame.pos;
			delete [] frame.uv;
			delete [] frame.indecies;
		}

		delete[] m_model3DFrames;

		if( m_pTexture != NULL )
		{
			m_pTexture->Release();
			m_pTexture = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Model::load( unsigned char * _buffer, size_t _size )
	{
		size_t read = 0;
		stdex::memory_reader mr( _buffer, _size, read );

		mr << m_frameCount;
		mr << m_vertexCount;
		mr << m_indicesCount;

		mr << m_frameDelay;

		mr << m_cameraFOV;
		mr << m_cameraWidth;
		mr << m_cameraHeight;

		m_model3DFrames = new Model3DFrame[ m_frameCount ];

		for( uint32_t i = 0; i != m_frameCount; ++i )
		{
			Model3DFrame & frame = m_model3DFrames[ i ];
						
			mr << frame.cameraRight;
			mr << frame.cameraUp;
			mr << frame.cameraDir;
			mr << frame.cameraPos;

			frame.pos = new mt::vec3f[m_vertexCount];
			frame.uv = new mt::vec2f[m_vertexCount];
			frame.indecies = new uint16_t[m_indicesCount];

			mr.readPODs( frame.pos, m_vertexCount );
			mr.readPODs( frame.uv, m_vertexCount );
			mr.readPODs( frame.indecies, m_indicesCount );
		}

		uint16_t width;
		uint16_t height;
		mr << width;
		mr << height;

		if( width != 0 && height != 0 )
		{
			if( isTexturePOW2( width ) == false || isTexturePOW2( height ) == false )
			{
				//_logger.message( "Model::load invalid texture POW2 %d:%d please set texture %d:%d"
				//	, width
				//	, height
				//	, getTexturePOW2( width )
				//	, getTexturePOW2( height )
				//	);

				return false;
			}

			LPDIRECT3DTEXTURE9 texture = NULL;
			if( m_pDevice->CreateTexture( width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture, NULL ) != D3D_OK )
			{
				//_logger.message( "Model::load invalid create texture %d:%d"
				//	, width
				//	, height
				//	);

				return false;
			}

			D3DLOCKED_RECT lockedRect;
			DXCALL texture->LockRect( 0, &lockedRect, NULL, 0 );

			if( lockedRect.pBits == NULL || lockedRect.Pitch == 0 )
			{
				//_logger.message( "Model::load invalid lock texture %d:%d"
				//	, width
				//	, height
				//	);

				return false;
			}

			for( uint16_t i = 0; i != height; ++i )
			{
				unsigned char * bits = static_cast<unsigned char *>(lockedRect.pBits) + lockedRect.Pitch * i;

				mr.readPODs( bits, width * 4 );
			}

			DXCALL texture->UnlockRect( 0 );

			m_pTexture = texture;
		}
		else
		{
			m_pTexture = NULL;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model::update( double _timing )
	{
		m_timing = _timing;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model::beginRender()
	{
		mt::mat4f projection;
		mt::make_projection_fov_m4( projection, m_cameraFOV, m_cameraWidth / m_cameraHeight, 1.f, 1000.f );

		DXCALL m_pDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX *)projection.buff() );

		const Model3DFrame & frame = this->getCurrentFrame();

		mt::mat4f lookAt;

		mt::vec3f cameraPos( frame.cameraPos.x, frame.cameraPos.y, frame.cameraPos.z );
		mt::vec3f cameraDir( -frame.cameraDir.x, -frame.cameraDir.z, -frame.cameraDir.y );
		mt::vec3f cameraUp( -frame.cameraRight.x, -frame.cameraRight.z, -frame.cameraRight.y );

		mt::make_lookat_m4( lookAt, cameraPos, cameraDir, cameraUp, -1.f );

		DXCALL m_pDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX *)lookAt.buff() );

		DXCALL m_pDevice->SetTexture( 0, m_pTexture );

		DXCALL m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	}
	//////////////////////////////////////////////////////////////////////////
	double Model::getTime() const
	{
		return m_timing;
	}
	//////////////////////////////////////////////////////////////////////////
	double Model::getDuration() const
	{
		return m_frameCount * m_frameDelay;
	}
	//////////////////////////////////////////////////////////////////////////
	const Model3DFrame & Model::getCurrentFrame() const
	{
		uint32_t frameIndex = ( uint32_t )(m_timing / m_frameDelay + 0.5);

		return m_model3DFrames[frameIndex];
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Model::getVertexCount() const
	{
		return m_vertexCount;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Model::getIndicesCount() const
	{
		return m_indicesCount;
	}
	//////////////////////////////////////////////////////////////////////////
	float Model::getCameraFOV() const
	{
		return m_cameraFOV;
	}
	//////////////////////////////////////////////////////////////////////////
	float Model::getCameraWidth() const
	{
		return m_cameraWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	float Model::getCameraHeight() const
	{
		return m_cameraHeight;
	}
}