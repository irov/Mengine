#	include "BurritoWorld.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FBurritoNodeRemove
		{
			FBurritoNodeRemove( Node * _node )
				: node(_node)
			{
			}

			bool operator()( const BurritoNode & _node ) const
			{
				return _node.node == node;
			}

			Node * node;

		private:
			FBurritoNodeRemove & operator = ( const FBurritoNodeRemove & _name );
		};
	}	
	//////////////////////////////////////////////////////////////////////////
	BurritoWorld::BurritoWorld()
		: m_bison(nullptr)
		, m_bounds(0.f, 0.f, 0.f)
		, m_cb(nullptr)
		, m_position(0.f, 0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoWorld::~BurritoWorld()
	{
		if( m_cb != nullptr )
		{
			pybind::decref( m_cb );
			m_cb = nullptr;
		}

		delete m_bison;
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoBison * BurritoWorld::createBison( Node * _node, float _radius )
	{
		m_bison = new BurritoBison;

		m_bison->initialize( _node, _radius );

		return m_bison;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::createGround( float _x, float _y, float _z, float _d, PyObject * _cb )
	{
		m_ground = new BurritoGround();

		mt::planef p( _x, _y, _z, _d );
		
		m_ground->initialize( p, _cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::createLayer( const ConstString & _name, float _parallax, const mt::vec3f & _bounds, PyObject * _cb )
	{
		BurritoLayer layer;
		layer.name = _name;
		layer.parallax = _parallax;
		mt::ident_v3(layer.position);

		layer.bounds = _bounds;

		layer.cb = _cb;
		pybind::incref( layer.cb );

		m_layers.push_back( layer );
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoWorld::addLayerNode( const ConstString & _name, Node * _node )
	{
		if( _node == nullptr )
		{
			return false;
		}

		for( TVectorBurritoLayer::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			BurritoLayer & layer = *it;

			if( layer.name != _name )
			{
				continue;
			}

			BurritoNode node;
			node.node = _node;

			layer.nodes.push_back( node );

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::removeLayerNode( const ConstString & _name, Node * _node )
	{
		for( TVectorBurritoLayer::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			BurritoLayer & layer = *it;

			if( layer.name != _name )
			{
				continue;
			}
			
			TVectorBurritoNode::iterator it_erase = std::find_if( layer.nodes.begin(), layer.nodes.end(), FBurritoNodeRemove(_node) );
			layer.nodes.erase( it_erase );

			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::update( float _time, float _timing )
	{
		if( m_bison == nullptr )
		{
			return;
		}

		mt::vec3f velocity;
		m_bison->update_velocity( _time, _timing, velocity );

		mt::vec3f translate_position;
		if( m_ground != nullptr )
		{
			float bison_radius = m_bison->getRadius();

			float collisionTiming;
			float collisionFactor;
			if( m_ground->check_collision( m_position, bison_radius, velocity, _timing, collisionTiming, collisionFactor ) == false )
			{
				translate_position = velocity * _timing;
			}
			else
			{
				m_bison->reflect_velocity( collisionFactor );

				translate_position = -velocity * collisionTiming;
			}
		}
		else
		{
			translate_position = velocity * _timing;
		}

		

		for( TVectorBurritoLayer::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			BurritoLayer & layer = *it;

			mt::vec3f layer_translate_position = - translate_position * layer.parallax;

			layer.position += layer_translate_position;

			if( mt::cmp_f_z( layer.bounds.x ) == false )
			{
				while( layer.position.x > layer.bounds.x )
				{
					pybind::call( layer.cb, "(Ni)"
						, pybind::ptr( layer.name )
						, 0
						);

					layer.position.x -= layer.bounds.x;
				}

				while( layer.position.x < -layer.bounds.x )
				{
					pybind::call( layer.cb, "(Ni)"
						, pybind::ptr( layer.name )
						, 2
						);

					layer.position.x += layer.bounds.x;
				}
			}

			if( mt::cmp_f_z( layer.bounds.y ) == false )
			{
				while( layer.position.y > layer.bounds.y )
				{
					pybind::call( layer.cb, "(Ni)"
						, pybind::ptr( layer.name )
						, 1
						);

					layer.position.y -= layer.bounds.y;
				}

				while( layer.position.y < -layer.bounds.y )
				{
					pybind::call( layer.cb, "(Ni)"
						, pybind::ptr( layer.name )
						, 3
						);

					layer.position.y += layer.bounds.y;
				}
			}

			if( mt::cmp_f_z( layer.bounds.z ) == false )
			{
				while( layer.position.z > layer.bounds.z )
				{
					pybind::call( layer.cb, "(Ni)"
						, pybind::ptr( layer.name )
						, 4
						);

					layer.position.z -= layer.bounds.z;
				}

				while( layer.position.z < -layer.bounds.z )
				{
					pybind::call( layer.cb, "(Ni)"
						, pybind::ptr( layer.name )
						, 5
						);

					layer.position.z += layer.bounds.z;
				}
			}
			
			for( TVectorBurritoNode::iterator
				it_node = layer.nodes.begin(),
				it_node_end = layer.nodes.end();
			it_node != it_node_end;
			++it_node )
			{
				BurritoNode & node = *it_node;

				node.node->translate( layer_translate_position );
			}			
		}
	}
}