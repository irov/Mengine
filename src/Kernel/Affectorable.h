#pragma once

#include "Kernel/AffectorType.h"

#include "stdex/intrusive_slug_list_size_ptr.h"
#include "stdex/intrusive_slug_ptr.h"

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

#include "math/vec3.h"

namespace Mengine
{ 
    //////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<class Affector> AffectorPtr;
    //////////////////////////////////////////////////////////////////////////
	class Affectorable
        : public Mixin
	{
	public:
		Affectorable();
		virtual ~Affectorable();

	public:
		AFFECTOR_ID addAffector( const AffectorPtr & _affector );
		bool stopAffector( AFFECTOR_ID _id );

	public:
		void stopAffectors( EAffectorType _type );
            
    public:
		void stopAllAffectors();
        
	public:
		void setAngularSpeed( float _angular );
		float getAngularSpeed() const;

		void setLinearSpeed( const mt::vec3f & _linearSpeed );
		const mt::vec3f & getLinearSpeed() const;

	public:
		void updateAffectors( float _current, float _timing );

	protected:
		void updateAffector_( const AffectorPtr & _affector, float _current, float _timing );
        
	protected:
		typedef stdex::intrusive_slug_list_size_ptr<Affector> VectorAffector;
        typedef stdex::intrusive_slug_ptr<Affector> TSlugAffector;

		VectorAffector m_affectors;

		float m_angularSpeed;
		mt::vec3f m_linearSpeed;

		uint32_t m_enumerator;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Affectorable> AffectorablePtr;
    //////////////////////////////////////////////////////////////////////////
}
