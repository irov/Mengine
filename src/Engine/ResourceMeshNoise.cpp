//
//#	include "ResourceMeshNoise.h"
//#	include "ResourceImplement.h"
//#	include "XmlEngine.h"
//
//namespace Menge
//{
//	//////////////////////////////////////////////////////////////////////////
//	RESOURCE_IMPLEMENT( ResourceMeshNoise );
//	//////////////////////////////////////////////////////////////////////////
//	ResourceMeshNoise::ResourceMeshNoise()
//		: m_width( 40 )
//		, m_height( 30 )
//	{
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceMeshNoise::loader( XmlElement * _xml )
//	{
//		ResourceReference::loader( _xml );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool ResourceMeshNoise::_compile()
//	{
//		Vertex3D tv;
//		for( int j = 0; j < m_height; j++ )
//		{
//			for( int i = 0; i < m_width; i++ )
//			{
//				tv.pos[0] = i; tv.pos[1] = -j; tv.pos[2] = 0.0f;
//				tv.n[0] = 0.0f; tv.n[1] = 0.0f; tv.n[2] = 1.0f;
//				tv.uv[0] = 1.0f - static_cast<float>( i ) / ( m_width - 1 ); tv.uv[1] = static_cast<float>( j ) / ( m_height - 1 );
//				m_vertices.push_back( tv );
//			} 
//		}
//		for( int j = 0; j < m_height - 1; j++ )
//		{
//			for( int i = 0; i < m_width - 1; i++ )
//			{
//				uint16 index = (m_width * j)+i;
//				m_indices.push_back( index ); m_indices.push_back( index + 1 ); m_indices.push_back( index + m_width );
//				m_indices.push_back( index + 1 ); m_indices.push_back( index + m_width + 1 ); m_indices.push_back( index + m_width );
//			}
//		}
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceMeshNoise::_release()
//	{
//		m_vertices.clear();
//		m_indices.clear();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	const ResourceMesh::TVectorVertex3D & ResourceMeshNoise::getVertexData()
//	{
//		return m_vertices;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	const ResourceMesh::TVectorIndicies & ResourceMeshNoise::getIndexData()
//	{
//		return m_indices;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceMeshNoise::addTiming( float _timing )
//	{
//		static float timing = 0.0f;
//		timing += _timing;
//		for( int i = 0; i < m_width; i++ )
//		{
//			float kx = 1.0f - ::fabsf( static_cast<float>( i ) / (m_width-1) - 0.5f ) * 2.0f;
//			for( int j = 0; j < m_height; j++ )
//			{
//				//float noise = perlinNoise_( i * 0.5, j * 0.5, _timing ) * 20.0f;
//				//m_vertices[i*m_width + j].pos[2] = -noise;
//				float ky = 1.0f - ::fabsf( static_cast<float>( j ) / (m_height-1) - 0.5f ) * 2.0f;
//				float z = ::sinf( i + timing * 0.001f ) * kx + ::cosf( j + timing * 0.001f ) * ky;
//				m_vertices[i*m_width + j].pos[2] = z * 0.3f;
//			}
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float ResourceMeshNoise::noise_( int x, int y )
//	{
//		int n = x + y * 57;
//		n = ::powf( (n<<13), n );
//		return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);    	
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float ResourceMeshNoise::smoothedNoise_( int x, int y )
//	{
//		float corners = ( noise_(x-1, y-1)+noise_(x+1, y-1)+noise_(x-1, y+1)+noise_(x+1, y+1) ) / 16.0f;
//		float sides   = ( noise_(x-1, y)  +noise_(x+1, y)  +noise_(x, y-1)  +noise_(x, y+1) ) /  8.0f;
//		float center  =  noise_(x, y) / 4.0f;
//		return corners + sides + center;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float ResourceMeshNoise::interpolatedNoise_( float x, float y )
//	{
//		int integer_X    = int(x);
//		float fractional_X = x - integer_X;
//
//		int integer_Y    = int(y);
//		float fractional_Y = y - integer_Y;
//
//		float v1 = smoothedNoise_(integer_X,     integer_Y);
//		float v2 = smoothedNoise_(integer_X + 1, integer_Y);
//		float v3 = smoothedNoise_(integer_X,     integer_Y + 1);
//		float v4 = smoothedNoise_(integer_X + 1, integer_Y + 1);
//
//		float i1 = linearInterpolate(v1 , v2 , fractional_X);
//		float i2 = linearInterpolate(v3 , v4 , fractional_X);
//
//		return linearInterpolate(i1 , i2 , fractional_Y);
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float ResourceMeshNoise::perlinNoise_( int x, int y, float _timing )
//	{
//			float total = 0.0f;
//			float persistence = 1.0f;
//			int octavesNum = 1;
//
//			for( int i = 0; i < octavesNum; i++ )
//			{
//				float frequency = ::powf( 2, i );
//				float amplitude = ::powf( persistence, i );
//
//				total = total + interpolatedNoise_(x * frequency, y * frequency) * amplitude;
//			}
//
//			return total;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float ResourceMeshNoise::linearInterpolate( float a, float b, float x )
//	{
//		return a*(1-x) + b*x;
//	}
//	//////////////////////////////////////////////////////////////////////////
//}	// namespace Menge
