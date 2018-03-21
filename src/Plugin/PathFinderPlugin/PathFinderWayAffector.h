#pragma once

#include "Kernel/Affector.h"
#include "Kernel/Node.h"

#include "math/vec2.h"

#include "Core/ValueFollower.h"

#include "fastpathfinder/pathfinder.h"
#include "fastpathfinder/map.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class PathFinderWayAffector
		: public Affector
		, public pybind::bindable
	{
	public:
		PathFinderWayAffector();
		~PathFinderWayAffector() override;
		
	public:
		bool initialize( Node * _node, const pybind::list & _satellite, float _offset, float _speed, const pybind::list & _way, bool _preparePosition, const pybind::object & _cb, const pybind::args & _args );
		
	public:
		Node * getNode() const;

	public:
		float getLength() const;
		float getWayLength() const;
		const mt::vec3f & getLocalPosition() const;

	public:
		bool purge( float _reduce, float _speed, float _acceleration );
		void unpurge();

	public:
		mt::vec3f getTimePosition( float _time ) const;

		pybind::tuple predictionLinearBullet( pybind::kernel_interface * _kernel, const mt::vec3f & _offset, const mt::vec3f & _position, float _speed ) const;
		pybind::tuple predictionParabolicBullet( pybind::kernel_interface * _kernel, const mt::vec3f & _offset, const mt::vec3f & _position, const mt::vec3f & _height, float _speed ) const;

	public:
		bool _prepare() override;
		bool _affect( float _timing ) override;
		void complete() override;
		void stop() override;

	protected:
		bool stepNextPoint_( const mt::vec3f & _pos, float _step, mt::vec3f & _out, mt::vec3f & _dir, uint32_t & _iterator ) const;
		bool step_( float _length, mt::vec3f & _pos, mt::vec3f & _dir );		
		float calcWayLength_() const;

	protected:
		void updatePosition_( const mt::vec3f & _pos );

	protected:
		void _setFreeze( bool _value ) override;

	protected:
		PyObject * _embedded() override;

	protected:
		Node * m_node;
		pybind::list m_satellite;
		pybind::list m_way;

		bool m_preparePosition;

		float m_speed;

		ValueFollowerAcceleration<float> m_followerPurge;

		float m_offset;
		float m_length;
		
		float m_wayLength;

		pybind::object m_cb;
		pybind::args m_args;

		uint32_t m_iterator;
		uint32_t m_wayCount;
	};
}

