#	pragma once

#	include "BurritoBison.h"
#	include "BurritoGround.h"
#	include "BurritoUnit.h"

#	include "Kernel/Node.h"

#	include "Core/ConstString.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	struct BurritoNode
	{
		Node * node;

		bool dead;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<BurritoNode *> TVectorBurritoNode;
	typedef stdex::vector<BurritoUnit *> TVectorBurritoUnit;
	//////////////////////////////////////////////////////////////////////////
	struct BurritoUnitBound
	{
		float value;
		bool less;

		pybind::object cb;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<BurritoUnitBound> TVectorBurritoUnitBounds;	
	//////////////////////////////////////////////////////////////////////////	
	struct BurritoLayer
	{
		ConstString name;
		float parallax;
		
		TVectorBurritoNode nodesAdd;
		TVectorBurritoNode nodes;
		TVectorBurritoUnit unitsAdd;
		TVectorBurritoUnit units;

		mt::vec3f position;
		mt::vec3f bounds;

		pybind::object cb;

		TVectorBurritoUnitBounds unitBounds;
	};
	//////////////////////////////////////////////////////////////////////////
	class BurritoWorld		
	{
	public:
		BurritoWorld();
		~BurritoWorld();

	public:
		void setDead();
		bool isDead() const;

	public:
		void setFreeze( bool _value );
		bool getFreeze() const;

	public:
		BurritoBison * createBison( Node * _node, const mt::vec3f & _offset, float _bisonY, float _radius );

	public:
		void createGround( float _x, float _y, float _z, float _d, const pybind::object & _cb );

	public:
		void addUnitBounds( float _value, bool _less, const pybind::object & _cb );

	public:
		void createLayer( const ConstString & _layerName, float _parallax, const mt::vec3f & _bounds, const pybind::object & _cb );

	public:
		bool addLayerNode( const ConstString & _layerName, Node * _node );
		void removeLayerNode( const ConstString & _layerName, Node * _node );

	public:
		BurritoUnit * addLayerUnit( const ConstString & _layerName, Node * _node, const mt::vec3f & _position, const mt::vec3f & _velocity, float _radius, bool _collide, const pybind::object & _cb );
		void removeLayerUnit( const ConstString & _layerName, Node * _node );

	public:
		bool addLayerBounds( const ConstString & _layerName, float _value, bool _less, const pybind::object & _cb );

	public:
		void update( float _time, float _timing );

	protected:
		BurritoBison * m_bison;

		BurritoGround * m_ground;

		TVectorBurritoUnitBounds m_unitBounds;

		typedef stdex::vector<BurritoLayer> TVectorBurritoLayer;
		TVectorBurritoLayer m_layers;				

		TVectorBurritoUnit m_units;

		bool m_dead;
		bool m_freeze;
	};
}