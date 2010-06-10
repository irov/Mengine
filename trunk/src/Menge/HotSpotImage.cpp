/*
 *	HotSpotImage.cpp
 *
 *	Created by _Berserk_ on 3.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "HotSpotImage.h"

#	include "ResourceManager.h"
#	include "ResourceHotspotImage.h"

#	include "Logger/Logger.h"

#	include "XmlEngine.h"
#	include "BinParser.h"

#	include "Utils/Core/String.h"
#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	HotSpotImage::HotSpotImage()
		: m_resourceHotspotImage( NULL )
		, m_frame( 0 )
		, m_alphaTest( 0.f )
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
			XML_CASE_ATTRIBUTE_NODE( "ImageIndex", "Value", m_frame );
			XML_CASE_ATTRIBUTE_NODE( "AlphaTest", "Value", m_alphaTest );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::parser( BinParser * _parser )
	{
		HotSpot::parser( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::ImageMap_Name, m_resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::ImageIndex_Value, m_frame );
			BIN_CASE_ATTRIBUTE( Protocol::AlphaTest_Value, m_alphaTest );
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
			ResourceFactoryParam param;
			param.category = "ResourceHotspotImage";
			param.name = hotspotResourceName;

			m_resourceHotspotImage = Holder<ResourceManager>::hostage()
				->createResourceWithParamT<ResourceHotspotImage>( "ResourceHotspotImage", param );

			m_resourceHotspotImage->setImageResource( m_resourceName, m_frame );

			Holder<ResourceManager>::hostage()
				->registerResource( m_resourceHotspotImage );

			m_resourceHotspotImage = Holder<ResourceManager>::hostage()
				->getResourceT<ResourceHotspotImage>( hotspotResourceName );

			if( m_resourceHotspotImage == NULL )
			{
				MENGE_LOG_ERROR( "Error: HotSpotImage can't get resource '%s'"
					, hotspotResourceName.c_str()
					);

				return false;
			}
		}

		const mt::vec2f& bSize = m_resourceHotspotImage->getSize();
		m_polygon.clear_points();
		m_polygon.add_point( mt::vec2f::zero_v2 );
		m_polygon.add_point( mt::vec2f( bSize.x, 0.0f ) );
		m_polygon.add_point( bSize );
		m_polygon.add_point( mt::vec2f( 0.0f, bSize.y ) );

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
		if( HotSpot::testPolygon( _transform, _screenPoly, _screenTransform ) == false )
		{
			return false;
		}

		mt::vec2f point;
		mt::mul_v2_m3( point, _screenPoly[0], _screenTransform );
		mt::mat3f invWM;
		mt::inv_m3( invWM, _transform );
		mt::vec2f pointIn;
		mt::mul_v2_m3( pointIn, point, invWM );
	
		return m_resourceHotspotImage->testPoint( pointIn, m_alphaTest );
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceHotspotImage * HotSpotImage::getResourseHotspotImage()
	{
		return m_resourceHotspotImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::setResourceName( const String& _resourceName )
	{
		if( m_resourceName == _resourceName )
		{
			return;
		}
		
		m_resourceName = _resourceName;

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::setFrame( std::size_t _frame )
	{
		if( m_frame == _frame )
		{
			return;
		}
		
		m_frame = _frame;

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::setAlphaTest( float _value )
	{
		m_alphaTest = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	float HotSpotImage::getAlphaTest() const
	{
		return m_alphaTest;
	}

	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
