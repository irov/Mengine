#pragma once

#include "Kernel/Node.h"

#include "AreaOfInterest.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
	class NodeAOIActor
		: public Node
		, public AOIActorProviderInterface
	{
	public:
		NodeAOIActor();
		~NodeAOIActor();

	public:
		void setRadius( float _radius );
		float getRadius() const;
		
	public:
		void setIFF( uint32_t _iff );
		uint32_t getIFF() const;

	public:
		void setAOI( AreaOfInterest * _aoi );
		AreaOfInterest * getAOI() const;

	public:
		void setActorUserData( const pybind::object & _data );
		const pybind::object & getActorUserData() const;

	protected:
		bool _activate() override;
		void _deactivate() override;
		
	protected:
		AreaOfInterest * m_aoi;
		AOIActorPtr m_actor;

		pybind::object m_data;
		
		float m_radius;

		uint32_t m_iff;
	};
}