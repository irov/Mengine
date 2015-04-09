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
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoWorld::initialize( const mt::vec3f & _bounds, PyObject * _cb )
	{
		m_bounds = _bounds;

		pybind::decref( m_cb );
		m_cb = _cb;
		pybind::incref( m_cb );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoBison * BurritoWorld::createBison()
	{
		m_bison = new BurritoBison;

		return m_bison;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::createLayer( const ConstString & _name, Node * _node, float _parallax )
	{
		BurritoLayer layer;
		layer.name = _name;
		layer.node = _node;
		layer.parallax = _parallax;

		m_layers.push_back( layer );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::addLayerNode( const ConstString & _name, Node * _node )
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

			BurritoNode node;
			node.node = _node;

			layer.nodes.push_back( node );

			break;
		}
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

		mt::vec3f translate_position = velocity * _timing;

		for( TVectorBurritoLayer::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			BurritoLayer & layer = *it;

			mt::vec3f layer_translate_position = - translate_position * layer.parallax;

			layer.position += layer_translate_position;

			while( layer.position.x > m_bounds.x )
			{
				pybind::call( m_cb, "Ni"
					, pybind::ptr(layer.name)
					, 0
					);

				layer.position.x -= m_bounds.x;
			}

			while( layer.position.x < -m_bounds.x )
			{
				pybind::call( m_cb, "Ni"
					, pybind::ptr(layer.name)
					, 2
					);

				layer.position.x += m_bounds.x;
			}

			while( layer.position.y > m_bounds.y )
			{
				pybind::call( m_cb, "Ni"
					, pybind::ptr(layer.name)
					, 1
					);

				layer.position.y -= m_bounds.y;
			}

			while( layer.position.y < -m_bounds.y )
			{
				pybind::call( m_cb, "Ni"
					, pybind::ptr(layer.name)
					, 3
					);

				layer.position.y += m_bounds.y;
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