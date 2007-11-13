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
		reset( m_bbox, 0, 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap2D::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader( _xml );

		mt::vec2f point;

		std::string name;		
					
		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE_FOR_EACH( "Zone" )
			{
				XML_CHECK_NODE( "Point" )
				{
					XML_VALUE_ATTRIBUTE( "Value", point );

					m_polygon.add_point( point );
					mt::add_internal_point( m_bbox, point );
				}
			}	

			XML_CHECK_NODE_FOR_EACH( "SceneNodes" )
			{
				XML_CHECK_NODE( "SceneNode" )
				{
					XML_VALUE_ATTRIBUTE("Value", name);
					m_sceneNodesNames.push_back( name );
				}
			}		
		}
	}
	////////////////////////////////////////////////////////////////////////////
	void Trap2D::setEnterZoneCallback( PyObject * _event )
	{
		registerEventCallback( "ENTER_ZONE", _event );
	}
	////////////////////////////////////////////////////////////////////////////
	void Trap2D::setLeaveZoneCallback( PyObject * _event )
	{
		registerEventCallback( "LEAVE_ZONE", _event );
	}
	////////////////////////////////////////////////////////////////////////////
	bool Trap2D::_activate()
	{
		if( SceneNode2D::_activate() == false )
		{
			return false;
		}

		for each( const TListSceneNodesNames::value_type & it in m_sceneNodesNames )
		{
			SceneNode2D * node = this->getParent()->getChildrenT<SceneNode2D>( it, true );

			if( node == NULL )
			{
				return false;
			}

			m_sceneNodes.push_back( node );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap2D::_deactivate()
	{
		SceneNode2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Trap2D::update( float _timing )
	{
		SceneNode2D::_update( _timing );

		for each( SceneNode2D * node in m_sceneNodes )
		{
			bool is_trapped =  mt::is_intersect( m_bbox, node->getBoundingBox() );
			
			if( is_trapped )
			{
				bool is_intersect = mt::intersect_poly_poly( m_polygon, node->getPolygon() );
		
				if( is_intersect )
				{
					m_trapped.push_back( node );
				}
			}
		}

		TListInZone::iterator it = m_inZone.begin();

		while( it != m_inZone.end() )
		{
			TListTrapped::iterator founded = std::find( m_trapped.begin(), m_trapped.end(), *it );

			if( founded == m_trapped.end() )
			{
				callEvent( "LEAVE_ZONE", "()" );

				it = m_inZone.erase(it);
			}
			else
			{
				it++;
			}
		}

		it = m_trapped.begin();

		while( it != m_trapped.end() )
		{
			TListInZone::iterator founded = std::find( m_inZone.begin(), m_inZone.end(), *it );

			if( founded == m_inZone.end() )
			{
				callEvent( "ENTER_ZONE", "()" );

				m_inZone.push_back( *it );
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
			//const mt::mat3f & wm = getWorldMatrix();
			//
			//mt::vec2f b;
			//mt::vec2f e;

			//mt::mul_v2_m3( b, m_polygon[ i ], wm );
			//mt::mul_v2_m3( e, m_polygon[ (i+1) % size ], wm );

			//renderEng->drawLine2D( b, e, 0xffff00ff );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
