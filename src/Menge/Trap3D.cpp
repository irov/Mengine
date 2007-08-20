#	include "Trap3D.h"

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "ScriptEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Trap3D);
	//////////////////////////////////////////////////////////////////////////
	Trap3D::Trap3D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap3D::loader( TiXmlElement * _xml )
	{
		SceneNode3D::loader(_xml);

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE( "Zone" )
			{
				XML_VALUE_ATTRIBUTE("MinEdge", m_bbox.MinEdge);
				XML_VALUE_ATTRIBUTE("MaxEdge", m_bbox.MaxEdge);
			}	

			XML_CHECK_NODE_FOR_EACH( "SceneNodes" )
			{
				XML_CHECK_NODE( "SceneNode" )
				{
					std::string name;
					XML_VALUE_ATTRIBUTE("Value", name);
					m_sceneNodesNames.push_back(name);
				}
			}		
		}
	}
	////////////////////////////////////////////////////////////////////////////
	//void Trap3D::setEnterZoneCallback(PyObject * _object)
	//{
	//	registerEvent( "ENTER_ZONE", _object );
	//}
	////////////////////////////////////////////////////////////////////////////
	//void Trap3D::setLeaveZoneCallback(PyObject * _object)
	//{
	//	registerEvent( "LEAVE_ZONE", _object );
	//}
	//////////////////////////////////////////////////////////////////////////
	bool Trap3D::_activate()
	{
		for(TListSceneNodesNames::iterator it = m_sceneNodesNames.begin(); it != m_sceneNodesNames.end(); ++it)
		{
			SceneNode3D * node = this->getParent()->getChildren(*it);

			if(node == NULL)
			{
				return false;
			}

			m_sceneNodes.push_back(node);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap3D::_deactivate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap3D::update( float _timing )
	{
		for(TListSceneNodes::iterator i = m_sceneNodes.begin(); i != m_sceneNodes.end(); ++i)
		{
			SceneNode3D * node = *i;

			bool is_trapped =  mt::is_intersect(m_bbox, node->getBoundingBox());

			if(is_trapped)
			{
				m_trapped.push_back(node);
			}
		}

		TListInZone::iterator it = m_inZone.begin();

		while(it != m_inZone.end())
		{
			TListTrapped::iterator founded = std::find(m_trapped.begin(),m_trapped.end(),*it);

			if(founded == m_trapped.end())
			{
				//if( PyObject * eventEnterZone = getEvent( "LEAVE_ZONE" ) )
				//{
				//	Holder<ScriptEngine>::hostage()
				//		->callFunction( eventEnterZone, "(O)", (*it)->getScriptable() );
				//}
				//it = m_inZone.erase(it);
			}
			else
			{
				it++;
			}
		}

		it = m_trapped.begin();

		while(it != m_trapped.end())
		{
			TListInZone::iterator founded = std::find(m_inZone.begin(),m_inZone.end(),*it);

			if(founded == m_inZone.end())
			{
				//if( PyObject * eventEnterZone = getEvent( "ENTER_ZONE" ) )
				//{
				//	Holder<ScriptEngine>::hostage()
				//		->callFunction( eventEnterZone, "(O)", (*it)->getScriptable() );
				//}
				//m_inZone.push_back(*it);
			}
			it++;
		}

		m_trapped.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap3D::debugRender() 
	{		
		RenderEngine * renderEng = Holder<RenderEngine>::hostage();

		mt::mat4f id;
		mt::ident_m4(id);
		renderEng->setWorldMatrix(id);

		renderEng->drawBox(m_bbox.MinEdge,m_bbox.MaxEdge, 0xaaff00ff);
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap3D::setEnterZoneCallback(PyObject * _object)
	{
		registerEvent( "ENTER_ZONE", _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap3D::setLeaveZoneCallback(PyObject * _object)
	{
		registerEvent( "LEAVE_ZONE", _object );
	}
	//////////////////////////////////////////////////////////////////////////
}
