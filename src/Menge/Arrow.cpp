#	include "Arrow.h"

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "InputEngine.h"

#	include "Sprite.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Arrow )
	//////////////////////////////////////////////////////////////////////////
	Arrow::Arrow()
	: m_offsetClick(0,0)
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
	void Arrow::_update( size_t _timing, const Viewport & _viewport )
	{
		SceneNode2D::_update( _timing, _viewport );

		InputEngine *inputEng = Holder<InputEngine>::hostage();

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
}