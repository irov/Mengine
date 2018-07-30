#pragma once

#include "Kernel/AffectorType.h"

#include "Kernel/Factorable.h"

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
		void setAffectorType( EAffectorType _type ) noexcept;
		EAffectorType getAffectorType() const noexcept;

	public:
		void setId( AFFECTOR_ID _id ) noexcept;
		AFFECTOR_ID getId() const noexcept;

	public:
		void setFreeze( bool _value ) noexcept;
		bool getFreeze() const noexcept;

	protected:
		virtual void _setFreeze( bool _value );

	public:
		void setSpeedFactor( float _speed ) noexcept;
		float getSpeedFactor() const noexcept;

	public:
		bool prepare();

	protected:
		virtual bool _prepare();

	public:
		virtual bool affect( float _current, float _time );

	protected:
		virtual bool _affect( float _current, float _time ) = 0;

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
