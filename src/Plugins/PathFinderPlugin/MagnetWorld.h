#pragma once

#include "Kernel/Servant.h"

#include "Factory/Factorable.h"
#include "Factory/Factory.h"

#include "MagnetActor.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	typedef Vector<MagnetActorPtr> TVectorMagnetActor;
	//////////////////////////////////////////////////////////////////////////
	class MagnetWorld
		: public Servant
	{
	public:
		MagnetWorld();
		~MagnetWorld() override;

	public:
		bool initialize();

	public:
		void setIFFs( uint32_t _iff1, uint32_t _iff2, bool _value );
		bool getIFFs( uint32_t _iff1, uint32_t _iff2 ) const;

	public:
		MagnetActorPtr createActor( MagnetActorProviderInterface * _provider, float _radius, float _force, uint32_t _iff, bool _active );
		void removeActor( const MagnetActorPtr & _actor );

	public:
		void update( float _time, float _timing );

	public:
		void remove();
		bool isRemoved() const;

	protected:
		TVectorMagnetActor m_actors;
		TVectorMagnetActor m_actorsAdd;
		
		FactoryPtr m_factoryCollisionActor;

		bool m_iffs[16][16];

		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<MagnetWorld> MagnetWorldPtr;
}