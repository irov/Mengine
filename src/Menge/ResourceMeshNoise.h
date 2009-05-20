#	pragma once

#	include "ResourceMesh.h"
#	include "Vertex.h"

namespace Menge
{
	class ResourceMeshNoise
		: public ResourceMesh
	{
		RESOURCE_DECLARE( ResourceMeshNoise )
	public:
		ResourceMeshNoise( const ResourceFactoryParam & _params );
	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;
		void addTiming( float _timing );

		const std::vector<Vertex3D>& getVertexData() override;
		const std::vector<uint16>& getIndexData() override;

	protected:
		std::vector<Vertex3D> m_vertices;
		std::vector<uint16> m_indices;
		int m_width;
		int m_height;
		float linearInterpolate( float a, float b, float x );
		float noise_( int x, int y );
		float smoothedNoise_( int x, int y );
		float interpolatedNoise_( float x, float y );
		float perlinNoise_( int x, int y, float _timing );
	};
}	// namespace Menge
