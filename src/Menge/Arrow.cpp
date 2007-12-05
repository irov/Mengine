#	include "Arrow.h"

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "InputEngine.h"

#	include "HotSpot.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Arrow )
	//////////////////////////////////////////////////////////////////////////
	Arrow::Arrow()
		: m_offsetClick(0,0)
		, m_currentHotSpot(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setOffsetClick( const mt::vec2f & _offsetClick )
	{
		m_offsetClick = _offsetClick;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Arrow::getOffsetClick()
	{
		return m_offsetClick;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_update( float _timing )
	{
		SceneNode2D::_update( _timing );

		InputEngine * inputEng = Holder<InputEngine>::hostage();

		int mx = inputEng->getMouseX();
		int my = inputEng->getMouseY();
		
		mt::vec2f pos( (float)mx, (float)my );
		setLocalPosition( pos + m_offsetClick );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_activate()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_compile()
	{
		m_currentHotSpot = new HotSpot();
		m_currentHotSpot->addPoint( mt::vec2f::zero_v2 );

		bool result = this->addChildren( m_currentHotSpot );

		m_currentHotSpot->activate();

		if( result == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE("ClickOffset", "Value", m_offsetClick);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_debugRender()
	{
		//RenderEngine *renderEng = Holder<RenderEngine>::hostage();
		//const mt::vec2f & pos = getWorldPosition();
		//renderEng->drawLine2D(pos,pos + mt::vec2f(10,-10),0xffff00ff);
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
		this->addChildren( _hotspot );
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot * Arrow::getCurrentHotSpot()
	{
		return m_currentHotSpot;
	}
	//////////////////////////////////////////////////////////////////////////
}