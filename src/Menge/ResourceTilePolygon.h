#	pragma once

#	include "Config/Typedef.h"

#	include "ResourceReference.h"

#	include <vector>

#	include "math/vec2.h"
#	include "math/polygon.h"

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
		const RenderImageInterface * getImage() const;
		const RenderImageInterface * getPlugImage() const;

		struct TileDecl
		{
			float min_angle;
			float max_angle;
			String image_resource;
			String junc_image_resource;
			String image_back_resource;
			const RenderImageInterface* image;
			const RenderImageInterface* junc_image;
			const RenderImageInterface* image_back;
		};

		typedef std::vector<TileDecl> TTileDecls;
		

		const TTileDecls& getTileDecls() const;

	protected:
		bool _compile() override;
		void _release() override;

		std::vector<TileDecl> m_tiles;

		String m_resourcename;
		String m_juncName;

		std::vector<ResourceImage*> m_imageResources;
		const RenderImageInterface* m_image;
		const RenderImageInterface* m_imageJunc;
	};
}