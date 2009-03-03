/*
 *	HotSpotImage.cpp
 *
 *	Created by _Berserk_ on 3.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "HotSpotImage.h"

#	include "ObjectImplement.h"

#	include "ResourceManager.h"
#	include "ResourceHotspotImage.h"

#	include "LogEngine.h"
#	include "XmlEngine.h"

#	include "Utils.h"
#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( HotSpotImage );
	//////////////////////////////////////////////////////////////////////////
	HotSpotImage::HotSpotImage()
		: m_resourceHotspotImage( NULL )
		, m_frame( 0 )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	HotSpotImage::~HotSpotImage()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::loader( XmlElement *_xml )
	{
		HotSpot::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ImageMap", "Name", m_resourceName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::_compile()
	{
		if( HotSpot::_compile() == false )
		{
			return false;
		}

		if( m_resourceName.empty() == true )
		{
			return false;
		}

		String hotspotResourceName = m_resourceName + "_ResourceHotspotImage" + Utils::toString( m_frame );
		m_resourceHotspotImage = Holder<ResourceManager>::hostage()
									->getResourceT<ResourceHotspotImage>( hotspotResourceName );

		if( m_resourceHotspotImage == NULL )	// if there is no such resource, create it
		{
			ResourceFactoryParam hParam;
			hParam.category = "ResourceHotspotImage";
			hParam.name = hotspotResourceName;
			m_resourceHotspotImage = new ResourceHotspotImage( hParam );
			m_resourceHotspotImage->setImageResource( m_resourceName, m_frame );
			Holder<ResourceManager>::hostage()
				->registerResource( m_resourceHotspotImage );
			m_resourceHotspotImage = Holder<ResourceManager>::hostage()
				->getResourceT<ResourceHotspotImage>( hotspotResourceName );

			if( m_resourceHotspotImage == NULL )
			{
				MENGE_LOG_ERROR( "Error: HotSpotImage can't get resource" );
				return false;
			}
		}

		const mt::vec2f& bSize = m_resourceHotspotImage->getSize();
		m_polygon.clear_points();
		m_polygon.add_point( mt::vec2f::zero_v2 );
		m_polygon.add_point( mt::vec2f( bSize.x, 0.0f ) );
		m_polygon.add_point( bSize );
		m_polygon.add_point( mt::vec2f( 0.0f, bSize.y ) );

		m_renderObjectHotspot->vertices.resize( 4 );

		uint16 indx[] = { 0, 1, 2, 3, 0 };
		m_renderObjectHotspot->passes[0].indicies.assign( indx, indx + 5 );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::_release()
	{
		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceHotspotImage );

		HotSpot::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testPolygon( const mt::mat3f& _transform, const mt::polygon& _screenPoly, const mt::mat3f& _screenTransform )
	{
		if( HotSpot::testPolygon( _transform, _screenPoly, _screenTransform ) == true )
		{
			mt::vec2f point;
			mt::mul_v2_m3( point, _screenPoly[0], _screenTransform );
			mt::mat3f invWM;
			mt::inv_m3( invWM, _transform );
			mt::vec2f pointIn;
			mt::mul_v2_m3( pointIn, point, invWM );
			return m_resourceHotspotImage->testPoint( pointIn );
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::setResourceName( const String& _resourceName )
	{
		if( m_resourceName != _resourceName )
		{
			m_resourceName = _resourceName;
			if( isCompile() == true )
			{
				recompile();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::setFrame( std::size_t _frame )
	{
		if( m_frame != _frame )
		{
			m_frame = _frame;
			if( isCompile() == true )
			{
				recompile();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
