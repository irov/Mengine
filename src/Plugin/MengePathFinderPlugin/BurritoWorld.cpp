#	include "BurritoWorld.h"

#	include "Math/ccd.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		class FBurritoNodeRemove
		{
		public:
			FBurritoNodeRemove( Node * _node )
				: node(_node)
			{
			}

		public:
			bool operator()( BurritoNode * _node ) const
			{
				return _node->node == node;
			}

		protected:
			Node * node;

		private:
			FBurritoNodeRemove & operator = ( const FBurritoNodeRemove & _name );
		};
		//////////////////////////////////////////////////////////////////////////
		class FBurritoUnitRemove
		{
		public:
			FBurritoUnitRemove( Node * _node )
				: node( _node )
			{
			}

		public:
			bool operator()( BurritoUnit * _unit ) const
			{
				Node * unit_node = _unit->getNode();

				return unit_node == node;
			}

		protected:
			Node * node;

		private:
			FBurritoUnitRemove & operator = (const FBurritoUnitRemove & _name);
		};
		//////////////////////////////////////////////////////////////////////////
		class FBurritoUnitDead
		{
		public:
			bool operator()( BurritoUnit * _unit ) const
			{
				return _unit->isDead();
			}

		private:
			FBurritoUnitDead & operator = (const FBurritoUnitDead & _name);
		};
		//////////////////////////////////////////////////////////////////////////
		static void s_burritoUnitBounds( const BurritoUnitBound & _bound, BurritoUnit * _unit, const mt::vec3f & _translate )
		{
			const mt::vec3f & unit_pos = _unit->getPosition();

			if( _bound.less == true )
			{
				if( unit_pos.x < _bound.value )
				{
					return;
				}

				if( unit_pos.x > _bound.value && unit_pos.x + _translate.x > _bound.value )
				{
					return;
				}
			}
			else
			{
				if( unit_pos.x > _bound.value )
				{
					return;
				}

				if( unit_pos.x < _bound.value && unit_pos.x + _translate.x < _bound.value )
				{
					return;
				}
			}

			Node * unit_node = _unit->getNode();

			_bound.cb( unit_node );
		}
	}	
	//////////////////////////////////////////////////////////////////////////
	BurritoWorld::BurritoWorld()
		: m_bison(nullptr)		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoWorld::~BurritoWorld()
	{
		delete m_bison;
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoBison * BurritoWorld::createBison( Node * _node, const mt::vec3f & _offset, float _bisonY, float _radius )
	{
		m_bison = new BurritoBison;

		m_bison->initialize( _node, _offset, _bisonY, _radius );

		return m_bison;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::createGround( float _x, float _y, float _z, float _d, const pybind::object & _cb )
	{
		m_ground = new BurritoGround();

		mt::planef p( _x, _y, _z, _d );
		
		m_ground->initialize( p, _cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::addUnitBounds( float _value, bool _less, const pybind::object & _cb )
	{
		BurritoUnitBound bound;
		bound.value = _value;
		bound.less = _less;
		bound.cb = _cb;

		m_unitBounds.push_back( bound );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoWorld::createLayer( const ConstString & _layerName, float _parallax, const mt::vec3f & _bounds, const pybind::object & _cb )
	{
		BurritoLayer layer;
		layer.name = _layerName;
		layer.parallax = _parallax;
		mt::ident_v3(layer.position);

		layer.bounds = _bounds;

		layer.cb = _cb;

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

			layer.nodesAdd.push_back( node );
			//layer.nodes.push_back( node );

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
	BurritoUnit * BurritoWorld::addLayerUnit( const ConstString & _layerName, Node * _node, const mt::vec3f & _position, const mt::vec3f & _velocity, float _radius, bool _collide, const pybind::object & _cb )
	{
		if( _node == nullptr )
		{
			return nullptr;
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
			unit->initialize( _node, _position, _velocity, _radius, _collide, _cb );
			
			layer.unitsAdd.push_back( unit );

			return unit;
		}

		return nullptr;
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

			if( it_erase == layer.units.end() )
			{
				continue;
			}

			(*it_erase)->setDead();
			
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

		for( TVectorBurritoLayer::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			BurritoLayer & layer = *it;

			layer.nodes.insert( layer.nodes.end(), layer.nodesAdd.begin(), layer.nodesAdd.end() );
			layer.nodesAdd.clear();

			layer.units.insert( layer.units.end(), layer.unitsAdd.begin(), layer.unitsAdd.end() );
			layer.unitsAdd.clear();
		}

		uint32_t iterate = 1;
		float next_timing = _timing;
		
		while( iterate-- != 0 )
		{
			float iterate_timing = next_timing;

			float bison_radius = m_bison->getRadius();

			mt::vec3f velocity;
			mt::vec3f offset;
			mt::vec3f offsetH;
			m_bison->update( _time, iterate_timing, velocity, offset, offsetH );

			mt::vec3f translate_position( 0.f, 0.f, 0.f );

			float sq_speed = velocity.sqrlength();

			if( mt::equal_f_z( sq_speed ) == false )
			{
				bool collision = false;
				float collisionTiming = 0.f;
				mt::vec3f newVelocity = velocity;

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

						if( unit->check_collision( iterate_timing, offsetH, bison_radius, collision_velocity, collisionTiming ) == true )
						{
							collision = true;

							break;
						}
					}
				}

				if( m_ground != nullptr && collision == false )
				{
					if( m_ground->check_collision( iterate_timing, offsetH, bison_radius, velocity, collisionTiming ) == true )
					{
						collision = true;												
					}
				}

				if( collision == true )
				{
					next_timing = iterate_timing - collisionTiming;
					iterate_timing = collisionTiming;
					
					++iterate;
				}
				
				mt::vec3f bison_translate = velocity * iterate_timing;
				
				m_bison->translate( bison_translate, translate_position );

				m_ground->translate( translate_position );

				for( TVectorBurritoLayer::iterator
					it = m_layers.begin(),
					it_end = m_layers.end();
				it != it_end;
				++it )
				{
					BurritoLayer & layer = *it;

					mt::vec3f layer_translate_position = translate_position * layer.parallax;

					layer.position += layer_translate_position;

					if( mt::equal_f_z( layer.bounds.x ) == false )
					{
						while( layer.position.x > layer.bounds.x )
						{
							layer.cb( layer.name, 0 );

							layer.position.x -= layer.bounds.x;
						}

						while( layer.position.x < -layer.bounds.x )
						{
							layer.cb( layer.name, 2 );

							layer.position.x += layer.bounds.x;
						}
					}

					if( mt::equal_f_z( layer.bounds.y ) == false )
					{
						while( layer.position.y > layer.bounds.y )
						{
							layer.cb( layer.name, 1 );

							layer.position.y -= layer.bounds.y;
						}

						while( layer.position.y < -layer.bounds.y )
						{
							layer.cb( layer.name, 3 );

							layer.position.y += layer.bounds.y;
						}
					}

					if( mt::equal_f_z( layer.bounds.z ) == false )
					{
						while( layer.position.z > layer.bounds.z )
						{
							layer.cb( layer.name, 4 );

							layer.position.z -= layer.bounds.z;
						}

						while( layer.position.z < -layer.bounds.z )
						{
							layer.cb( layer.name, 5 );

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

				mt::vec3f layer_translate_position = translate_position * layer.parallax;

				for( TVectorBurritoUnit::iterator
					it_unit = layer.units.begin(),
					it_unit_end = layer.units.end();
				it_unit != it_unit_end;
				++it_unit )
				{
					BurritoUnit * unit = *it_unit;

					if( unit->isDead() == true )
					{
						continue;
					}

					mt::vec3f unit_translate;
					unit->update( iterate_timing, unit_translate );

					unit_translate += layer_translate_position;

					for( TVectorBurritoUnitBounds::const_iterator
						it_bound = m_unitBounds.begin(),
						it_bound_end = m_unitBounds.end();
					it_bound != it_bound_end;
					++it_bound )
					{
						if( unit->isBound() == false )
						{
							break;
						}

						const BurritoUnitBound & bound = *it_bound;

						s_burritoUnitBounds( bound, unit, unit_translate );
					}

					unit->translate( unit_translate );
				}
				
				TVectorBurritoUnit::iterator it_erase = std::remove_if( layer.units.begin(), layer.units.end(), FBurritoUnitDead() );
				layer.units.erase( it_erase, layer.units.end() );
			}
		}
	}
}