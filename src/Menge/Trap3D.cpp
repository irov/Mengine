#	include "Trap3D.h"

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Trap3D);
	//////////////////////////////////////////////////////////////////////////
	Trap3D::Trap3D()
		: m_enterZoneCallback(0)
		, m_leaveZoneCallback(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap3D::loader( TiXmlElement * _xml )
	{
		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE( "Zone" )
			{
				XML_VALUE_ATTRIBUTE("MinEdge", m_boundingZone.MinEdge);
				XML_VALUE_ATTRIBUTE("MaxEdge", m_boundingZone.MaxEdge);
			}	

			XML_CHECK_NODE_FOR_EACH( "SceneNodes" )
			{
				XML_CHECK_NODE( "SceneNode" )
				{
					std::string name;
					XML_VALUE_ATTRIBUTE("Value", name);
					m_sceneNodes.push_back(name);
				}
			}		
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap3D::setEnterZoneCallback(EnterZoneCallback3D _callback)
	{
		m_enterZoneCallback = _callback;
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap3D::setLeaveZoneCallback(LeaveZoneCallback3D _callback)
	{
		m_leaveZoneCallback = _callback;
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap3D::update( float _timing )
	{
		if(m_enterZoneCallback == NULL || m_leaveZoneCallback == NULL)
		{
			return;
		}

		for(TListSceneNodesNames::iterator i = m_sceneNodes.begin(); i != m_sceneNodes.end(); ++i)
		{
			SceneNode3D * node = this->getParent()->getChildren(*i);
			const mt::vec3f& pos = node->getWorldPosition();

			bool is_trapped = true;

			if ((pos[0] < m_boundingZone.MinEdge[0] || pos[0] > m_boundingZone.MaxEdge[0]) ||
				(pos[1] < m_boundingZone.MinEdge[1] || pos[1] > m_boundingZone.MaxEdge[1] ||
				(pos[2] < m_boundingZone.MinEdge[2] || pos[2] > m_boundingZone.MaxEdge[2]))) is_trapped = false;

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
				m_leaveZoneCallback(*it);
				it = m_inZone.erase(it);
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
				m_enterZoneCallback(*it);
				m_inZone.push_back(*it);
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

		renderEng->drawBox(m_boundingZone.MinEdge,m_boundingZone.MaxEdge, 0xaaff00ff);
	}
}