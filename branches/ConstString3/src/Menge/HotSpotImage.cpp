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

#	include "LogEngine.h"

#	include "RenderEngine.h"

#	include "Core/String.h"
#	include "Consts.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	HotSpotImage::HotSpotImage()
		: m_resourceHotspotImage( NULL )
		, m_alphaTest( 0.f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpotImage::~HotSpotImage()
	{
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

		ConstString resourceHotspotImageName = ConstString( Helper::to_str(m_resourceName) + "_ResourceHotspotImage" );

		if( ResourceManager::get()->hasResource( resourceHotspotImageName ) == false )
		{
			ResourceHotspotImage * resourceHotspotImage = ResourceManager::get()
				->createResourceT<ResourceHotspotImage>( Consts::get()->c_builtin_empty, Consts::get()->c_builtin_empty, resourceHotspotImageName, Consts::get()->c_ResourceHotspotImage );

			resourceHotspotImage->setImageResource( m_resourceName );
		}

		m_resourceHotspotImage = ResourceManager::get()
			->getResourceT<ResourceHotspotImage>( resourceHotspotImageName );

		if( m_resourceHotspotImage == NULL )
		{
			MENGE_LOG_ERROR( "Error: HotSpotImage can't get resource '%s'"
				, resourceHotspotImageName.c_str()
				);

			return false;
		}

		const mt::vec2f& bSize = m_resourceHotspotImage->getSize();

		m_polygon.clear();

		boost::geometry::append(m_polygon, mt::vec2f(0.f, 0.f) );
		boost::geometry::append(m_polygon, mt::vec2f(bSize.x, 0.f) );
		boost::geometry::append(m_polygon, mt::vec2f(bSize.x, bSize.y) );
		boost::geometry::append(m_polygon, mt::vec2f(0.f, bSize.y) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::_release()
	{
		if( m_resourceHotspotImage != 0 )
		{
			m_resourceHotspotImage->decrementReference();
			m_resourceHotspotImage = 0;
		}

		HotSpot::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testPolygon( const mt::mat3f& _transform, const Polygon& _screenPoly, const mt::mat3f& _screenTransform )
	{
		if( this->isActivate() == false )
		{
			return false;
		}

		if( HotSpot::testPolygon( _transform, _screenPoly, _screenTransform ) == false )
		{
			return false;
		}

		const Polygon::ring_type & ring = _screenPoly.outer();

		if( ring.size() == 1 )
		{
			mt::vec2f point;
			mt::mul_v2_m3( point, ring[0], _screenTransform );

			mt::mat3f invWM;
			mt::inv_m3( invWM, _transform );
			mt::vec2f pointIn;
			mt::mul_v2_m3( pointIn, point, invWM );

			bool result = m_resourceHotspotImage->testPoint( pointIn, m_alphaTest );

			return result;
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testRadius( const mt::mat3f& _transform, float _radius, const mt::mat3f& _screenTransform )
	{
		if( this->isActivate() == false )
		{
			return false;
		}
		
		mt::vec2f point;
		mt::mul_v2_m3( point, mt::vec2f(0.f, 0.f), _screenTransform );

		mt::mat3f invWM;
		mt::inv_m3( invWM, _transform );
		mt::vec2f pointIn;
		mt::mul_v2_m3( pointIn, point, invWM );

		bool result = m_resourceHotspotImage->testRadius( pointIn, _radius, m_alphaTest );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceHotspotImage * HotSpotImage::getResourseHotspotImage() const
	{
		return m_resourceHotspotImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::setImageResource( const ConstString& _resourceName )
	{
		if( m_resourceName == _resourceName )
		{
			return;
		}
		
		m_resourceName = _resourceName;

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & HotSpotImage::getImageResource() const
	{
		return m_resourceName;
	}
	//////////////////////////////////////////////////////////////////////////
	//void HotSpotImage::setFrame( size_t _frame )
	//{
	//	if( m_frame == _frame )
	//	{
	//		return;
	//	}
	//	
	//	m_frame = _frame;

	//	recompile();
	//}
	////////////////////////////////////////////////////////////////////////////
	//size_t HotSpotImage::getFrame() const
	//{
	//	return m_frame;
	//}
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
