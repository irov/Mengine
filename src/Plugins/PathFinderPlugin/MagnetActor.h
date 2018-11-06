#pragma once

#include "Factory/Factorable.h"

#include "Core/IntrusivePtr.h"

#include "Config/Vector.h"

#include "math/vec3.h"

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
	typedef IntrusivePtr<class MagnetActor> MagnetActorPtr;
	//////////////////////////////////////////////////////////////////////////
	class MagnetActor
		: public Factorable
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

		typedef Vector<MagnetActorPtr> TVectorActorException;
		TVectorActorException m_exceptions;

		mt::vec3f m_velocity;
		bool m_affect;

		bool m_active;
		bool m_remove;
	};
}