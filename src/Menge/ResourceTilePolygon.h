#	pragma once

#	include "ResourceReference.h"

#	include "Config/Typedef.h"

#	include <vector>
#	include <list>

#	include "math\vec2.h"
#	include "math\polygon.h"

namespace Menge
{
	class ResourceImage;

	class RenderImageInterface;

	struct Quad //fix
	{
		mt::TVectorPoints v; //FIXME: !!
		float angle;
		float s;
		float t;
	};

	typedef std::vector<mt::polygon> TListConcavePolygon;
	typedef std::vector<Quad> TListQuad;

	class ResourceTilePolygon
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceTilePolygon )

	public:
		ResourceTilePolygon( const ResourceFactoryParam & _params );

	public:
		void loader( XmlElement * _xml ) override;

	public:
		const mt::TVectorPoints * getTriangles() const;
		const mt::TVectorPoints * getTextureCoords() const;
		const TListConcavePolygon * getShapeList() const;
		const RenderImageInterface * getImage() const;

		size_t getTileCount() const;

		const RenderImageInterface * getPlugImage() const;
		const RenderImageInterface * getTileImage(unsigned int _tile) const;

		float getMinAngle(unsigned int _tile) const;
		float getMaxAngle(unsigned int _tile) const;
		const TListQuad * getTileGeometry(unsigned int _tile) const;
		const TListQuad * getTileJunks(unsigned int _tile) const;

	protected:
		bool _compile() override;
		void _release() override;

	private:

		struct Tile
		{
			Tile(float _minAngle, float _maxAngle, const String & _name)
			: m_minAngle( _minAngle )
			, m_maxAngle( _maxAngle )
			, m_imagename( _name )
			, m_image(0)
			, m_imageResource(0)
			{}

			float m_minAngle;
			float m_maxAngle;
			String m_imagename;
			const RenderImageInterface * m_image;
			ResourceImage * m_imageResource;

			TListQuad m_geometry;
			TListQuad m_junks;
		};

		void create(Tile & _tile, const mt::TVectorPoints & _poly, const mt::vec2f & _juncSize);

		std::vector<Tile> m_tiles;

		void addVertex(const mt::vec2f & _vertex);

		mt::TVectorPoints m_poly;
		mt::TVectorPoints m_triangles;
		mt::TVectorPoints m_uvs;

		String m_resourcename;
		String m_juncName;

		const RenderImageInterface * m_image;
		const RenderImageInterface * m_imageJunc;

		ResourceImage * m_resource;
		ResourceImage * m_resourceJunc;

		std::list<ResourceImage*> m_imageResources;
		TListConcavePolygon m_concaves;
	};
}