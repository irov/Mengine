#	pragma once

#	include "BurritoBison.h"
#	include "BurritoGround.h"
#	include "BurritoUnit.h"

#	include "Kernel/Node.h"
#	include "Endless.h"

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
		mt::vec3f parallax;
		
		TVectorBurritoUnit unitsAdd;
		TVectorBurritoUnit units;

		Endless * endless;

		TVectorBurritoUnitBounds unitBounds;
	};
	//////////////////////////////////////////////////////////////////////////
	class BurritoWorld		
	{
	public:
		BurritoWorld();
		~BurritoWorld();

	public:
		bool initialize();
		void finalize();

	public:
		void setDead();
		bool isDead() const;

	public:
		void setFreeze( bool _value );
		bool getFreeze() const;

	public:
		BurritoBison * createBison( Node * _node, RenderCameraOrthogonal * _camera, const Viewport & _renderport, const mt::vec3f & _position, float _radius );

	public:
		BurritoGround * createGround( float _x, float _y, float _z, float _d, const pybind::object & _cb );

	public:
		void addUnitBounds( float _value, bool _less, const pybind::object & _cb );

	public:
		void createLayer( const ConstString & _layerName, const mt::vec3f & _parallax, uint32_t _count, float _width, const pybind::object & _cb );
	
	public:
		BurritoUnit * addLayerUnit( const ConstString & _layerName, Node * _node, const mt::vec3f & _position, const mt::vec3f & _velocity, float _radius, bool _collide, const pybind::object & _cb );
		void removeLayerUnit( const ConstString & _layerName, Node * _node );

	public:
		bool addLayerBounds( const ConstString & _layerName, float _value, bool _less, const pybind::object & _cb );

	public:
		void update( float _time, float _timing );

	protected:
		BurritoBison * m_bison;

		typedef stdex::vector<BurritoGround *> TVectorBurritoGround;
		TVectorBurritoGround m_grounds;

		TVectorBurritoUnitBounds m_unitBounds;

		typedef stdex::vector<BurritoLayer> TVectorBurritoLayer;
		TVectorBurritoLayer m_layers;				

		TVectorBurritoUnit m_units;

		bool m_dead;
		bool m_freeze;
	};
}