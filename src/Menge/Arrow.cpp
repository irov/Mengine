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
	: m_arrowIdle(0)
	, m_arrowClick(0)
	, m_offsetClick(0,0)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_update(float _timing)
	{
		InputEngine *inputEng = Holder<InputEngine>::hostage();

		const mt::vec3f &dmv = inputEng->getPosition();

		setPosition( mt::vec2f(dmv.x, dmv.y) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_activate()
	{
		m_arrowIdle->setOffset(m_offsetClick);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE("IdleSprite")
			{
				XML_DEF_ATTRIBUTES_NODE(Type);
				
				m_arrowIdle = createChildrenT<Sprite>( Type );
				m_arrowIdle->loader(XML_CURRENT_NODE);
			}

			XML_CHECK_VALUE_NODE("ClickOffset", "Value", m_offsetClick);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_debugRender()
	{
		RenderEngine *renderEng = Holder<RenderEngine>::hostage();

		const mt::vec2f & pos = getWorldPosition();

		renderEng->drawLine(pos,pos + mt::vec2f(10,-10),2,0xffff00ff);
	}
}