//#	pragma once
//
//#	include "ResourceMesh.h"
//#	include "Vertex.h"
//
//namespace Menge
//{
//	class ResourceMeshNoise
//		: public ResourceMesh
//	{
//		RESOURCE_DECLARE( ResourceMeshNoise )
//	public:
//		ResourceMeshNoise();
//	public:
//		void loader( XmlElement * _xml ) override;
//
//	protected:
//		bool _compile() override;
//		void _release() override;
//		void addTiming( float _timing );
//
//		const TVectorVertex3D & getVertexData() override;
//		const TVectorIndicies & getIndexData() override;
//
//	protected:
//		TVectorVertex3D m_vertices;
//		TVectorIndicies m_indices;
//
//		int m_width;
//		int m_height;
//
//	protected:
//		float linearInterpolate( float a, float b, float x );
//		float noise_( int x, int y );
//		float smoothedNoise_( int x, int y );
//		float interpolatedNoise_( float x, float y );
//		float perlinNoise_( int x, int y, float _timing );
//	};
//}	// namespace Menge
