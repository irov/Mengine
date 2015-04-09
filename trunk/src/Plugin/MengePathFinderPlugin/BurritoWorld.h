#	pragma once

#	include "BurritoBison.h"

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
		Node * node;
		float parallax;
				
		TVectorBurritoNode nodes;

		mt::vec3f position;
	};
	//////////////////////////////////////////////////////////////////////////
	class BurritoWorld		
	{
	public:
		BurritoWorld();
		~BurritoWorld();

	public:
		bool initialize( const mt::vec3f & _bounds, PyObject * _cb );

	public:
		BurritoBison * createBison();

	public:
		void createLayer( const ConstString & _name, Node * _node, float _parallax );

	public:
		void addLayerNode( const ConstString & _name, Node * _node );
		void removeLayerNode( const ConstString & _name, Node * _node );

	public:
		void update( float _time, float _timing );

	protected:
		mt::vec3f m_bounds;
		PyObject * m_cb;

		BurritoBison * m_bison;

		typedef stdex::vector<BurritoLayer> TVectorBurritoLayer;
		TVectorBurritoLayer m_layers;

		mt::vec3f m_position;
	};
}