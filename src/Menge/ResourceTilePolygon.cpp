#	include "ResourceTilePolygon.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

#	include "math/triangulation.h"

#	include "ResourceImage.h"

#	include "Interface/RenderSystemInterface.h"

#	include "ResourceManager.h"

#	include "math/angle.h"
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT(ResourceTilePolygon);
	//////////////////////////////////////////////////////////////////////////
	ResourceTilePolygon::ResourceTilePolygon(const ResourceFactoryParam & _params)
		: ResourceReference(_params)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::loader(XmlElement * _xml)
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Tile" )
			{
				//float minAngle;
				//float maxAngle;
				//String imageName;
				TileDecl tileDecl;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "MinAngle", tileDecl.min_angle );
					XML_CASE_ATTRIBUTE( "MaxAngle", tileDecl.max_angle );
					XML_CASE_ATTRIBUTE( "Image", tileDecl.image_resource );
					XML_CASE_ATTRIBUTE( "ImageBack", tileDecl.image_back_resource );
					XML_CASE_ATTRIBUTE( "JuncImage", tileDecl.junc_image_resource );
				}

				m_tiles.push_back( tileDecl );
			}

			XML_CASE_NODE( "TileJunc" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Image", m_juncName );
				}
			}

			XML_CASE_ATTRIBUTE_NODE( "ImageMap", "Name", m_resourcename );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceTilePolygon::getImage() const
	{
		return m_image;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceTilePolygon::getPlugImage() const
	{
		return m_imageJunc;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::_release()
	{
		for(std::vector<ResourceImage*>::iterator it = m_imageResources.begin(); it != m_imageResources.end(); it++)
		{
			Holder<ResourceManager>::hostage()->releaseResource(*it);
		}

		m_imageResources.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceTilePolygon::_compile()
	{
		ResourceManager* resourceManager = Holder<ResourceManager>::hostage();

		ResourceImage* resourceImage = 
			resourceManager->getResourceT<ResourceImage>( m_resourcename );

		if( resourceImage == 0 )
		{
			MENGE_LOG_ERROR( "Image resource not getting \"%s\""
				, m_resourcename.c_str() );
			return false;
		}

		m_image = resourceImage->getImage( 0 );
		m_imageResources.push_back( resourceImage );

		resourceImage = 
			resourceManager->getResourceT<ResourceImage>( m_juncName );

		if( resourceImage == 0 )
		{
			MENGE_LOG_ERROR( "Image resource not getting \"%s\""
				, m_juncName.c_str() );
			return false;
		}

		m_imageJunc = resourceImage->getImage( 0 );
		m_imageResources.push_back( resourceImage );

		for( TTileDecls::size_type i = 0; i < m_tiles.size(); i++ )
		{
			if( m_tiles[i].image_resource.empty() == false )
			{
				resourceImage = resourceManager->getResourceT<ResourceImage>( m_tiles[i].image_resource );
				if( resourceImage != NULL )
				{
					m_tiles[i].image = resourceImage->getImage( 0 );
					m_imageResources.push_back( resourceImage );
				}
			}
			if( m_tiles[i].junc_image_resource.empty() == false )
			{
				resourceImage = resourceManager->getResourceT<ResourceImage>( m_tiles[i].junc_image_resource );
				if( resourceImage != NULL )
				{
					m_tiles[i].junc_image = resourceImage->getImage( 0 );
					m_imageResources.push_back( resourceImage );
				}
			}
			if( m_tiles[i].image_back_resource.empty() == false )
			{
				resourceImage = resourceManager->getResourceT<ResourceImage>( m_tiles[i].image_back_resource );
				if( resourceImage != NULL )
				{
					m_tiles[i].image_back = resourceImage->getImage( 0 );
					m_imageResources.push_back( resourceImage );
				}
			}
			else
			{
				m_tiles[i].image_back = NULL;
			}
		}
		/*mt::vec2f juncSize = mt::vec2f(m_imageJunc->getWidth(),m_imageJunc->getHeight());

		// FIXXXXXXX
		for(std::vector<Tile>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++)
		{
			create(*it,m_poly,juncSize);
		}*/

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	/*void ResourceTilePolygon::create(Tile & _tile, const mt::TVectorPoints & _poly, const mt::vec2f & _juncSize)
	{
		ResourceImage * imageResource =
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( _tile.m_imagename );

		m_imageResources.push_back(imageResource);

		 _tile.m_image = imageResource->getImage(0);

		float _width = _tile.m_image->getWidth();
		float _height = _tile.m_image->getHeight();

		mt::vec2f edge;
		mt::vec2f penumbra;

		bool found_junction = false;

		mt::vec2f track_edge;

		mt::TVectorPoints::size_type pointsNum = _poly.size();

		Quad quad;
		quad.v.resize(4);

		int track_point = 0;

		_tile.m_geometry.clear();
		_tile.m_junks.clear();

		for(mt::TVectorPoints::size_type i = 0; i < pointsNum; i++)
		{
			int next_i = (i + 1) % pointsNum;

			edge = _poly[next_i] - _poly[i];

			float angle = mt::signed_angle(edge) * mt::m_rad2deg;

			if(mt::angle_in_interval_deg(angle,_tile.m_minAngle,_tile.m_maxAngle))
			{
				track_point = next_i;
				track_edge = mt::perp(edge);

				penumbra = track_edge / edge.length() * _height * 0.5f;

				quad.v[0] = _poly[i] - penumbra;
				quad.v[1] = _poly[next_i] - penumbra;
				quad.v[2] = _poly[next_i] + penumbra;
				quad.v[3] = _poly[i] + penumbra;
				quad.angle = angle;

				quad.s = edge.length() / _width;
				quad.t = mt::length_v2_v2(quad.v[1], quad.v[2]) / _height;

				_tile.m_geometry.push_back(quad);

				found_junction = true;

				if(i < pointsNum-1)
					continue;
			}

			if(found_junction)
			{
				mt::vec2f normal = (track_edge + mt::perp(edge));
				normal = normal / normal.length();

				penumbra = normal * _juncSize.y * 0.5f;

				mt::vec2f lpoint = _poly[track_point] - mt::perp(normal) * _juncSize.x * 0.5f;
				mt::vec2f rpoint = _poly[track_point] + mt::perp(normal) * _juncSize.x * 0.5f;

				quad.v[0] = lpoint - penumbra;
				quad.v[1] = rpoint - penumbra;
				quad.v[2] = rpoint + penumbra;
				quad.v[3] = lpoint + penumbra;

				quad.s = mt::length_v2_v2(quad.v[0], quad.v[1]) / _juncSize.x;
				quad.t = mt::length_v2_v2(quad.v[1], quad.v[2]) / _juncSize.y;


				_tile.m_junks.push_back(quad);

				found_junction = false;
			}
		}
	}*/
	//////////////////////////////////////////////////////////////////////////
	const ResourceTilePolygon::TTileDecls& ResourceTilePolygon::getTileDecls() const
	{
		return m_tiles;
	}
	//////////////////////////////////////////////////////////////////////////
}