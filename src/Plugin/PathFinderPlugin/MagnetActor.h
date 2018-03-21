#pragma once

#include "Factory/FactorablePtr.h"

#include "math/vec3.h"

#include "stdex/intrusive_ptr.h"
#include "stdex/stl_vector.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class MagnetActorProviderInterface
	{
	public:
		virtual void onMagnetPositionProvider( mt::vec3f & _position ) const = 0;
		virtual void onMagnetAffect( const mt::vec3f & _velocity ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class MagnetActor> MagnetActorPtr;
	//////////////////////////////////////////////////////////////////////////
	class MagnetActor
		: public FactorablePtr
	{
	public:
		MagnetActor();

	public:
		void setMagnetActorProvider( MagnetActorProviderInterface * _provider );
		MagnetActorProviderInterface * getMagnetActorProvider() const;

	public:
		void setRadius( float _radius );
		float getRadius() const;

	public:
		void setForce( float _force );
		float getForce() const;

	public:
		void setActive( bool _active );
		bool isActive() const;
	
	public:
		void setIFF( uint32_t _iff );
		uint32_t getIFF() const;

	public:
		void getSphere( mt::vec3f & _position, float & _radius ) const;

	public:
		void addException( const MagnetActorPtr & _actor );
		void removeException( const MagnetActorPtr & _actor );
		bool isException( const MagnetActorPtr & _actor ) const;

	public:
		void addVelocity( const mt::vec3f & _velocity );
		void updateVelocity();

	public:
		void remove();
		bool isRemoved() const;

	protected:
		MagnetActorProviderInterface * m_provider;

		float m_radius;
		float m_force;

		uint32_t m_iff;

		typedef stdex::vector<MagnetActorPtr> TVectorActorException;
		TVectorActorException m_exceptions;

		mt::vec3f m_velocity;
		bool m_affect;

		bool m_active;
		bool m_remove;
	};
}