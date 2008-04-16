#	include "Arrow.h"

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "InputEngine.h"

#	include "HotSpot.h"

#	include "XmlEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Arrow )
	//////////////////////////////////////////////////////////////////////////
	Arrow::Arrow()
	: m_offsetClick(0,0)
	, m_currentHotSpot(0)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setOffsetClick( const mt::vec2f & _offsetClick )
	{
		m_offsetClick = _offsetClick;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Arrow::getOffsetClick() const
	{
		return m_offsetClick;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_update( float _timing )
	{
		SceneNode2D::_update( _timing );

		InputEngine * inputEng = Holder<InputEngine>::hostage();

		float mx = inputEng->getMouseX();
		float my = inputEng->getMouseY();
		
		mt::vec2f pos( mx, my );
		setLocalPosition( pos + m_offsetClick );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_activate()
	{
		SceneNode2D::_activate();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_compile()
	{
		m_currentHotSpot = new HotSpot();
		m_currentHotSpot->addPoint( mt::vec2f::zero_v2 );
		m_currentHotSpot->setName("MainHotSpotArrow");

		bool result = this->addChildren( m_currentHotSpot );

		if( result == false )
		{
			return false;
		}

		result = m_currentHotSpot->activate();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Arrow" )
			{
				XML_PARSE_ELEMENT( this, &Arrow::loaderArrow_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////	
	void Arrow::loaderArrow_( XmlElement * _xml )
	{
		SceneNode2D::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ClickOffset", "Value", m_offsetClick );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_renderBegin()
	{
		Holder<RenderEngine>::hostage()
			->beginLayer();

		Viewport viewport;
		viewport.begin = mt::vec2f( 0.f, 0.f );
		viewport.end = mt::vec2f( 1024.f, 768.f );

		Holder<RenderEngine>::hostage()
			->setRenderViewport( viewport );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_renderEnd()
	{
		Holder<RenderEngine>::hostage()
			->endLayer();
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::addHotSpot( HotSpot * _hotspot )
	{
		if( m_currentHotSpot != NULL )
		{
			this->removeChildren( m_currentHotSpot );
		}

		bool result = this->addChildren( _hotspot );

		if( result == false )
		{
			assert(!"can't add hotspot to arrow");
		}

		m_currentHotSpot = _hotspot;
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot * Arrow::getCurrentHotSpot() const
	{
		return m_currentHotSpot;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_debugRender()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
}