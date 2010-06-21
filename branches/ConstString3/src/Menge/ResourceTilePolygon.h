#	pragma once

#	include "Config/Typedef.h"

#	include "ResourceReference.h"

#	include <vector>

namespace Menge
{
	class ResourceImage;

	struct TileDecl
	{
		float min_angle;
		float max_angle;
		ConstString image_resource;
		ConstString junc_image_resource;
		ConstString image_back_resource;
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
		ResourceTilePolygon();

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

		const ConstString& getResourceName() const;
		const ConstString& getJunkName() const;

	protected:
		bool _compile() override;
		void _release() override;

		TTileDecls m_tiles;

		ConstString m_resourcename;
		ConstString m_juncName;

		TResourceVec m_imageResources;

		ResourceImage* m_image;
		ResourceImage* m_imageJunc;
	};
}
