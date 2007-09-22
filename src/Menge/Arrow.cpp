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
	void Arrow::_update( float _timing )
	{
		SceneNode2D::_update( _timing );

		InputEngine *inputEng = Holder<InputEngine>::hostage();

		const mt::vec3f &dmv = inputEng->getPosition();

		mt::vec2f pos(dmv.x, dmv.y);
		setPosition( pos + m_offsetClick );
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
		RenderEngine *renderEng = Holder<RenderEngine>::hostage();

		const mt::vec2f & pos = getWorldPosition();

		renderEng->drawLine2D(pos,pos + mt::vec2f(10,-10),0xffff00ff);
	}
	//////////////////////////////////////////////////////////////////////////
}