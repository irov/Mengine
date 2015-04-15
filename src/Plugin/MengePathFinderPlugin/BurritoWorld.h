#	pragma once

#	include "BurritoBison.h"
#	include "BurritoGround.h"

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
	typedef stdex::vector<BurritoNode> TVectorBurritoNode;
	//////////////////////////////////////////////////////////////////////////	
	struct BurritoLayer
	{
		ConstString name;
		float parallax;
				
		TVectorBurritoNode nodes;

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

		void createGround( float _x, float _y, float _z, float _d, PyObject * _cb );

	public:
		void createLayer( const ConstString & _name, float _parallax, const mt::vec3f & _bounds, PyObject * _cb );

	public:
		bool addLayerNode( const ConstString & _name, Node * _node );
		void removeLayerNode( const ConstString & _name, Node * _node );

	public:
		void update( float _time, float _timing );

	protected:
		mt::vec3f m_bounds;
		PyObject * m_cb;

		BurritoBison * m_bison;

		BurritoGround * m_ground;

		typedef stdex::vector<BurritoLayer> TVectorBurritoLayer;
		TVectorBurritoLayer m_layers;
	};
}