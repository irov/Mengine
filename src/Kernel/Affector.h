#pragma once

#include "Kernel/AffectorType.h"

#include "Factory/Factorable.h"

#include "stdex/intrusive_slug_linked_ptr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AffectorCallbackInterface
        : public Factorable
    {
    public:
        virtual void onAffectorEnd( AFFECTOR_ID _id, bool _isEnd ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AffectorCallbackInterface> AffectorCallbackInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class Affector
		: public stdex::intrusive_slug_linked_ptr<Affector>
		, public Factorable
	{
	public:
		Affector();
		~Affector() override;

	public:
		void setAffectorType( EAffectorType _type );
		EAffectorType getAffectorType() const;

	public:
		void setId( AFFECTOR_ID _id );
		AFFECTOR_ID getId() const;

	public:
		void setFreeze( bool _value );
		bool getFreeze() const;

	protected:
		virtual void _setFreeze( bool _value );

	public:
		void setSpeedFactor( float _speed );
		float getSpeedFactor() const;

	public:
		bool prepare();

	protected:
		virtual bool _prepare();

	public:
		virtual bool affect( float _timing );

	protected:
		virtual bool _affect( float _timing ) = 0;

	public:
		virtual void complete() = 0;
		virtual void stop() = 0;

	protected:
		EAffectorType m_type;
		AFFECTOR_ID m_id;

		float m_speedFactor;

		bool m_freeze;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Affector> AffectorPtr;
    //////////////////////////////////////////////////////////////////////////
	
}	
