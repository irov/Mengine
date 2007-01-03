#	include "Actor.h"
#	include "ObjectImplement.h"

#	include "SceneManager.h"

#	include "Avatar.h"

#	include "XmlParser.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Actor)
//////////////////////////////////////////////////////////////////////////
Actor::Actor()
: m_avatar(0)
{

};
//////////////////////////////////////////////////////////////////////////
void Actor::loader( TiXmlElement * _xml )
{
	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_NODE("Avatar")
		{
			XML_DEF_ATTRIBUTES_NODE(Name);
			XML_DEF_ATTRIBUTES_NODE(Type);

			m_avatar = SceneManager::createNodeT<Avatar>(Name,Type);
		}
	}
}