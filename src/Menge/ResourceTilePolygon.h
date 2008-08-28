#	pragma once

#	include "ResourceReference.h"

#	include "Config/Typedef.h"

#	include <vector>

#	include "math\vec2.h"
#	include "math\polygon.h"

namespace Menge
{
	class ResourceTilePolygon
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceTilePolygon )

	public:
		ResourceTilePolygon( const ResourceFactoryParam & _params );

	public:
		void loader( XmlElement * _xml ) override;

	public:
		const mt::TVectorPoints& getTriangles() const;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		void addVertex(const mt::vec2f & _vertex);

		mt::TVectorPoints m_poly;
		mt::TVectorPoints m_triangles;
		typedef std::vector<mt::polygon> TVectorConcavePolygon;
		TVectorConcavePolygon m_concaves;
	};
}