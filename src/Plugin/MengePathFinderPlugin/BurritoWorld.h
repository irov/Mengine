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
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<BurritoNode *> TVectorBurritoNode;
	typedef stdex::vector<BurritoUnit *> TVectorBurritoUnit;
	//////////////////////////////////////////////////////////////////////////	
	struct BurritoLayer
	{
		ConstString name;
		float parallax;
				
		TVectorBurritoNode nodes;
		TVectorBurritoUnit units;

		mt::vec3f position;
		mt::vec3f bounds;

		PyObject * cb;
	};
	//////////////////////////////////////////////////////////////////////////
	class BurritoWorld		
	{
	public:
		BurritoWorld();
		~BurritoWorld();

	public:
		BurritoBison * createBison( Node * _node, float _radius );

	public:
		void createGround( float _x, float _y, float _z, float _d, PyObject * _cb );

	public:
		void addUnitBounds( const mt::vec3f & _min, const mt::vec3f & _max, PyObject * _cb );

	public:
		void createLayer( const ConstString & _layerName, float _parallax, const mt::vec3f & _bounds, PyObject * _cb );

	public:
		bool addLayerNode( const ConstString & _layerName, Node * _node );
		void removeLayerNode( const ConstString & _layerName, Node * _node );

	public:
		bool addLayerUnit( const ConstString & _layerName, Node * _node, const mt::vec3f & _position, const mt::vec3f & _velocity, float _radius, PyObject * _cb );
		void removeLayerUnit( const ConstString & _layerName, Node * _node );

	public:
		void update( float _time, float _timing );

	protected:
		BurritoBison * m_bison;

		BurritoGround * m_ground;

		typedef stdex::vector<BurritoLayer> TVectorBurritoLayer;
		TVectorBurritoLayer m_layers;

		mt::vec3f m_bounds_min;
		mt::vec3f m_bounds_max;

		PyObject * m_bounds_cb;

		TVectorBurritoUnit m_units;
	};
}