#	include "Arrow.h"
#	include "ObjectImplement.h"

#	include "RenderEngine.h"
#	include "InputEngine.h"

#	include "Sprite.h"

#	include "XmlParser.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT( Arrow )
//////////////////////////////////////////////////////////////////////////
Arrow::Arrow()
: m_arrowIdle(0)
, m_arrowClick(0)
, m_offsetClick(0,0)
{

}
//////////////////////////////////////////////////////////////////////////
HotSpot * Arrow::pickHotSpot()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void Arrow::_update(float _timing)
{
	InputEngine *inputEng = Keeper<InputEngine>::hostage();

	int x = inputEng->GetPosition(0);
	int y = inputEng->GetPosition(1);

	setPosition( mt::vec2f(x, y) );
	
}
//////////////////////////////////////////////////////////////////////////
bool Arrow::_compile()
{
	m_arrowIdle->setOffset(m_offsetClick);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Arrow::loader( TiXmlElement *_xml )
{
	Renderable::loader( _xml );

	SceneManager *sceneMgr = Keeper<SceneManager>::hostage();

	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_NODE("IdleSprite")
		{
			XML_DEF_ATTRIBUTES_NODE(Type);
			
			m_arrowIdle = createChildrenT<Sprite>("_Idle", Type);
			m_arrowIdle->loader(XML_CURRENT_NODE);
		}

		XML_CHECK_VALUE_NODE("ClickOffset", "Value", m_offsetClick);
	}
};
//////////////////////////////////////////////////////////////////////////
void Arrow::_debugRender()
{
	//RenderEngine *renderEng = Keeper<RenderEngine>::hostage();

	//const mt::vec2f & pos = getWorldPosition();

	//renderEng->drawLine(pos,pos + mt::vec2f(10,-10),2,0xffff00ff);
};