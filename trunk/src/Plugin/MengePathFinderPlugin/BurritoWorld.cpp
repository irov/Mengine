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

			bool operator()( BurritoNode * _node ) const
			{
				return _node->node == node;
			}

			Node * node;

		private:
			FBurritoNodeRemove & operator = ( const FBurritoNodeRemove & _name );
		};
		//////////////////////////////////////////////////////////////////////////
		struct FBurritoUnitRemove
		{
			FBurritoUnitRemove( Node * _node )
				: node( _node )
			{
			}

			bool operator()( BurritoUnit * _unit ) const
			{
				Node * unit_node = _unit->getNode();

				return unit_node == node;
			}

			Node * node;

		private:
			FBurritoUnitRemove & operator = (const FBurritoUnitRemove & _name);
		};
		//////////////////////////////////////////////////////////////////////////
		struct FBurritoUnitBounds
		{
			FBurritoUnitBounds( const mt::vec3f & _bounds_min, const mt::vec3f & _bounds_max, PyObject * _cb )
				: bounds_min( _bounds_min )
				, bounds_max( _bounds_max )
				, cb(_cb)
			{
			}

			bool operator()( BurritoUnit * _unit ) const
			{				
				if( _unit->isDead() == true )
				{
					return true;
				}

				const mt::vec3f & unit_pos = _unit->getPosition();

				if( bounds_min.x < unit_pos.x && bounds_min.y < unit_pos.y && bounds_max.x > unit_pos.x && bounds_max.y > unit_pos.y )
				{
					return false;
				}

				Node * unit_node = _unit->getNode();

				pybind::call( cb, "(N)"
					, pybind::ptr( unit_node )
					);

				return true;
			}

			mt::vec3f bounds_min;
			mt::vec3f bounds_max;
			PyObject * cb;

		private:
			FBurritoUnitBounds & operator = (const FBurritoUnitBounds & _name);
		};
	}	
	//////////////////////////////////////////////////////////////////////////
	BurritoWorld::BurritoWorld()
		: m_bison(nullptr)
		, m_position( 0.f, 0.f, 0.f )
		, m_bounds_min( 0.f, 0.f, 0.f )
		, m_bounds_max( 0.f, 0.f, 0.f )
		, m_bounds_cb( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoWorld::~BurritoWorld()
	{
		delete m_bison;

		pybind::decref( m_bounds_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoBison * BurritoWorld::createBison( Node * _node, const mt::vec3f & _offset, float _radius )
	{
		m_bison = new BurritoBison;

		m_bison->initialize( _node, _offset, _radius );

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
	void BurritoWorld::addUnitBounds( const mt::vec3f & _min, const mt::vec3f & _max, PyObject * _cb )
	{
		m_bounds_min = _min;
		m_bounds_max = _max;

		m_bounds_cb = _cb;
		pybind::incref( m_bounds_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::createLayer( const ConstString & _layerName, float _parallax, const mt::vec3f & _bounds, PyObject * _cb )
	{
		BurritoLayer layer;
		layer.name = _layerName;
		layer.parallax = _parallax;
		mt::ident_v3(layer.position);

		layer.bounds = _bounds;

		layer.cb = _cb;
		pybind::incref( layer.cb );

		m_layers.push_back( layer );
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoWorld::addLayerNode( const ConstString & _layerName, Node * _node )
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

			if( layer.name != _layerName )
			{
				continue;
			}

			BurritoNode * node = new BurritoNode;

			node->node = _node;

			layer.nodes.push_back( node );

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::removeLayerNode( const ConstString & _layerName, Node * _node )
	{
		for( TVectorBurritoLayer::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			BurritoLayer & layer = *it;

			if( layer.name != _layerName )
			{
				continue;
			}
			
			TVectorBurritoNode::iterator it_erase = std::find_if( layer.nodes.begin(), layer.nodes.end(), FBurritoNodeRemove(_node) );

			delete *it_erase;

			layer.nodes.erase( it_erase );

			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoWorld::addLayerUnit( const ConstString & _layerName, Node * _node, const mt::vec3f & _position, const mt::vec3f & _velocity, float _radius, PyObject * _cb )
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

			if( layer.name != _layerName )
			{
				continue;
			}

			BurritoUnit * unit = new BurritoUnit;
			unit->initialize( _node, _position, _velocity, _radius, _cb );
			
			layer.units.push_back( unit );

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::removeLayerUnit( const ConstString & _layerName, Node * _node )
	{
		for( TVectorBurritoLayer::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			BurritoLayer & layer = *it;

			if( layer.name != _layerName )
			{
				continue;
			}

			TVectorBurritoUnit::iterator it_erase = std::find_if( layer.units.begin(), layer.units.end(), FBurritoUnitRemove( _node ) );

			delete *it_erase;

			layer.units.erase( it_erase );

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

		float bison_radius = m_bison->getRadius();

		
		mt::vec3f velocity;
		mt::vec3f offset;
		m_bison->update( _time, _timing, velocity, offset );

		//position = mt::vec3f(0, 0, 0);
		//velocity = mt::vec3f( 0, 0, 0 );

		mt::vec3f translate_position(0.f, 0.f, 0.f);

		float sq_speed = velocity.sqrlength();

		if( mt::equal_f_z( sq_speed ) == false )
		{
			bool collision = false;
			float collisionTiming = 0.f;
			mt::vec2f collisionFactor;

			for( TVectorBurritoLayer::const_iterator
				it = m_layers.begin(),
				it_end = m_layers.end();
			it != it_end;
			++it )
			{
				const BurritoLayer & layer = *it;

				for( TVectorBurritoUnit::const_iterator
					it_unit = layer.units.begin(),
					it_unit_end = layer.units.end();
				it_unit != it_unit_end;
				++it_unit )
				{
					const BurritoUnit * unit = *it_unit;
										
					mt::vec3f collision_velocity = velocity * layer.parallax;

					if( unit->check_collision( _timing, offset, bison_radius, collision_velocity, collisionTiming, collisionFactor ) == true )
					{
						collision = true;

						break;
					}
				}
			}

			if( m_ground != nullptr && collision == false )
			{
				mt::vec3f collision_position = m_position + offset;

				//printf( "%f %f\n", collision_position.y, m_position.y );

				collision = m_ground->check_collision( _timing, collision_position, bison_radius, velocity, collisionTiming, collisionFactor );
			}

			if( collision == true )
			{
				mt::vec3f reflect_velocity;
				m_bison->reflect( collisionFactor, reflect_velocity );

				float reflectTiming = _timing - collisionTiming;

				translate_position = velocity * collisionTiming + reflect_velocity * reflectTiming;
			}
			else
			{
				translate_position = velocity * _timing;
			}

			m_position += translate_position;

			for( TVectorBurritoLayer::iterator
				it = m_layers.begin(),
				it_end = m_layers.end();
			it != it_end;
			++it )
			{
				BurritoLayer & layer = *it;

				mt::vec3f layer_translate_position = -translate_position * layer.parallax;

				layer.position += layer_translate_position;

				if( mt::equal_f_z( layer.bounds.x ) == false )
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

				if( mt::equal_f_z( layer.bounds.y ) == false )
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

				if( mt::equal_f_z( layer.bounds.z ) == false )
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
					BurritoNode * node = *it_node;

					node->node->translate( layer_translate_position );
				}
			}
		}

		for( TVectorBurritoLayer::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			BurritoLayer & layer = *it;

			mt::vec3f layer_translate_position = -translate_position * layer.parallax;

			for( TVectorBurritoUnit::iterator
				it_unit = layer.units.begin(),
				it_unit_end = layer.units.end();
			it_unit != it_unit_end;
			++it_unit )
			{
				BurritoUnit * unit = *it_unit;

				unit->update( _timing, layer_translate_position );
			}

			if( m_bounds_cb != nullptr )
			{
				TVectorBurritoUnit::iterator it_erase = std::remove_if( layer.units.begin(), layer.units.end(), FBurritoUnitBounds( m_bounds_min, m_bounds_max, m_bounds_cb ) );
				layer.units.erase( it_erase, layer.units.end() );
			}
		}
	}
}