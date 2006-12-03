#	include "Arrow.h"
#	include "ObjectImplement.h"

#	include "RenderEngine.h"
#	include "InputEngine.h"

#	include "Sprite.h"

#	include "XmlParser.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT( Arrow )
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
	return true;
}
//////////////////////////////////////////////////////////////////////////
void Arrow::loader( TiXmlElement *_xml )
{
	Renderable::loader( _xml );

	SceneManager *sceneMgr = Keeper<SceneManager>::hostage();
	//<IdleSprite>
	//	<ImageMNG File = "output.mng" />	
	//	</IdleSprite>
	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_NODE("IdleSprite")
		{
			XML_DEF_ATTRIBUTES_NODE(Type);
			
			m_arrowIdle = createChildrenT<Sprite>("_Idle", Type);
			m_arrowIdle->loader(XML_CURRENT_NODE);
		}
	}	
};
//////////////////////////////////////////////////////////////////////////
void Arrow::_debugRender()
{
	//RenderEngine *renderEng = Keeper<RenderEngine>::hostage();

	//const mt::vec2f & pos = getWorldPosition();

	//renderEng->drawLine(pos,pos + mt::vec2f(10,-10),2,0xffff00ff);
};