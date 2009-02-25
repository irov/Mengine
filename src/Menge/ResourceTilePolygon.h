#	pragma once

#	include "Config/Typedef.h"

#	include "ResourceReference.h"

#	include <vector>

//#	include "math/vec2.h"
//#	include "math/polygon.h"

namespace Menge
{
	class ResourceImage;

	class RenderImageInterface;

	struct TileDecl
	{
		float min_angle;
		float max_angle;
		String image_resource;
		String junc_image_resource;
		String image_back_resource;
		ResourceImage* image;
		ResourceImage* junc_image;
		ResourceImage* image_back;
	};

	typedef std::vector<TileDecl> TTileDecls;

	typedef std::vector<ResourceImage*> TResourceVec;
	

	class ResourceTilePolygon
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceTilePolygon )

	public:
		ResourceTilePolygon( const ResourceFactoryParam & _params );

	public:
		void loader( XmlElement * _xml ) override;

	public:
		ResourceImage* getImage();
		ResourceImage* getPlugImage();

		/*struct TileDecl
		{
			float min_angle;
			float max_angle;
			String image_resource;
			String junc_image_resource;
			String image_back_resource;
			ResourceImage* image;
			ResourceImage* junc_image;
			ResourceImage* image_back;
		};*/

		//typedef std::vector<TileDecl> TTileDecls;

		const TTileDecls& getTileDecls() const;

		const String& getResourceName() const;
		const String& getJunkName() const;

	protected:
		bool _compile() override;
		void _release() override;

		TTileDecls m_tiles;

		String m_resourcename;
		String m_juncName;

		TResourceVec m_imageResources;

		ResourceImage* m_image;
		ResourceImage* m_imageJunc;
	};
}
