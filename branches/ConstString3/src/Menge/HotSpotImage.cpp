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

#	include "RenderEngine.h"
#	include "XmlEngine.h"
#	include "BinParser.h"

#	include "Core/String.h"
#	include "Consts.h"


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
	void HotSpotImage::_loaded()
	{
		m_resourceHotspotImageName = m_resourceName.str() + "_ResourceHotspotImage" + Utils::toString( m_frame );
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

		m_resourceHotspotImage = ResourceManager::get()
			->getResourceT<ResourceHotspotImage>( m_resourceHotspotImageName );

		// if there is no such resource, create it
		if( m_resourceHotspotImage == NULL )
		{
			ResourceFactoryParam param;

			param.category = Consts::get()->c_ResourceHotspotImage;
			param.name = m_resourceHotspotImageName;

			m_resourceHotspotImage = ResourceManager::get()
				->createResourceWithParamT<ResourceHotspotImage>( param.category, param );

			m_resourceHotspotImage->setImageResource( m_resourceName, m_frame );

			ResourceManager::get()
				->registerResource( m_resourceHotspotImage );

			m_resourceHotspotImage = ResourceManager::get()
				->getResourceT<ResourceHotspotImage>( m_resourceHotspotImageName );

			if( m_resourceHotspotImage == NULL )
			{
				MENGE_LOG_ERROR( "Error: HotSpotImage can't get resource '%s'"
					, m_resourceHotspotImageName.c_str()
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
		ResourceManager::get()
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
	void HotSpotImage::setResourceName( const ConstString& _resourceName )
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
