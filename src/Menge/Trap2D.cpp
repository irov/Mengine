#	include "Trap2D.h"

#	include "ObjectImplement.h"

#	include "XmlParser/XmlParser.h"

#	include "RenderEngine.h"

#	include "ScriptEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Trap2D);
	//////////////////////////////////////////////////////////////////////////
	Trap2D::Trap2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap2D::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE_FOR_EACH( "Zone" )
			{
				XML_CHECK_NODE( "Point" )
				{
					mt::vec2f point;
					XML_VALUE_ATTRIBUTE("Value", point);
					m_polygon.add_point(point);
				}
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
	void Trap2D::setEnterZoneCallback(PyObject * _object)
	{
		registerEvent( "ENTER_ZONE", _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap2D::setLeaveZoneCallback(PyObject * _object)
	{
		registerEvent( "LEAVE_ZONE", _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap2D::update( float _timing )
	{
		for(TListSceneNodesNames::iterator i = m_sceneNodes.begin(); i != m_sceneNodes.end(); ++i)
		{
			SceneNode2D * node = this->getParent()->getChildren(*i);
			const mt::vec2f& pos = node->getWorldPosition();
			bool is_trapped = mt::is_point_inside_polygon(m_polygon, pos);

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
				if( PyObject * eventEnterZone = getEvent( "LEAVE_ZONE" ) )
				{
					Holder<ScriptEngine>::hostage()
						->callFunction( eventEnterZone, "(O)", (*it)->getScriptable() );
				}

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
				if( PyObject * eventLeaveZone = getEvent( "ENTER_ZONE" ) )
				{
					Holder<ScriptEngine>::hostage()
						->callFunction( eventLeaveZone, "(O)", (*it)->getScriptable() );
				}

				m_inZone.push_back(*it);
			}
			it++;
		}

		m_trapped.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap2D::debugRender() 
	{
		RenderEngine *renderEng = Holder<RenderEngine>::hostage();

		size_t size = m_polygon.num_points();

		for( size_t i = 0; i < size; ++i )
		{
			const mt::mat3f & wm = getWorldMatrix();
			
			mt::vec2f b;
			mt::vec2f e;

			mt::mul_v2_m3( b, m_polygon[ i ], wm );
			mt::mul_v2_m3( e, m_polygon[ (i+1) % size ], wm );

			renderEng->drawLine2D( b, e, 0xffff00ff );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}