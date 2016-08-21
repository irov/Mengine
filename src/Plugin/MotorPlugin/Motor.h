#	pragma once

#	include "Kernel/Affector.h"

#	include "Core/ConstString.h"
#	include "Math/vec3.h"

#	include "stdex/stl_map.h"

namespace Menge
{
	class ServiceProviderInterface;

	class Node;

	class Motor
		: public Affector
	{
	public:
		Motor();
		~Motor();

	public:
		void setServiceProvider(ServiceProviderInterface * m_serviceProvider);
		ServiceProviderInterface * getServiceProvider() const;

	public:
		void setNode(Node * _node);
		Node * getNode() const;

	public:
		void addVelocity(const ConstString & _name, const mt::vec3f & _velocity);
		bool hasVelocity(const ConstString & _name) const;
		mt::vec3f getVelocity(const ConstString & _name) const;

	protected:
		bool _affect( float _timing ) override;
		void complete() override;
		void stop() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		Node * m_node;

		struct VelocityDesc
		{
			mt::vec3f velocity;
		};

		typedef stdex::map<ConstString, VelocityDesc> TMapVelocity;
		TMapVelocity m_velocities;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<Motor> MotorPtr;
}