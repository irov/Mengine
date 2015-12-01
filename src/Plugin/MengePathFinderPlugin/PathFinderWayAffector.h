#	pragma once

#	include "Kernel/Affector.h"
#	include "Kernel/Node.h"

#	include "Math/vec2.h"

#	include "fastpathfinder/pathfinder.h"
#	include "fastpathfinder/map.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class PathFinderWayAffector
		: public Affector
	{
	public:
		PathFinderWayAffector();
		~PathFinderWayAffector();
		
	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool initialize( Node * _node, float _offset, float _speed, const pybind::list & _way, const pybind::object & _cb );
		
	public:
		Node * getNode() const;

	public:
		float getLength() const;
		float getWayLength() const;
		const mt::vec3f & getLocalPosition() const;

	public:
		mt::vec3f getTimePosition( float _time ) const;

		pybind::tuple predictionLinearBullet( const mt::vec3f & _offset, const mt::vec3f & _position, float _speed ) const;
		pybind::tuple predictionIsometricLinearBullet( const mt::vec3f & _offset, const mt::vec3f & _position, float _speed ) const;
		pybind::tuple predictionIsometricParabolicBullet( const mt::vec3f & _offset, const mt::vec3f & _position, const mt::vec3f & _height, float _speed ) const;

	public:
		bool prepare() override;
		bool _affect( float _timing ) override;
		void complete() override;
		void stop() override;

	protected:
		bool stepNextPoint_( const mt::vec3f & _pos, float _step, mt::vec3f & _out, mt::vec3f & _dir, uint32_t & _iterator ) const;
		bool step_( float _length, mt::vec3f & _pos, mt::vec3f & _dir );		
		float calcWayLength_() const;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		Node * m_node;
		pybind::list m_way;

		float m_speed;
		float m_offset;
		float m_length;
		
		float m_wayLength;

		pybind::object m_cb;

		uint32_t m_iterator;
		uint32_t m_wayCount;
	};
}

