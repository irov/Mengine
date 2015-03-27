#	pragma once

#	include "Kernel/AffectorType.h"
#   include "Kernel/Affector.h"

#	include "stdex/intrusive_slug_list_size.h"
#	include "stdex/intrusive_slug.h"

#	include "Math/vec3.h"

namespace Menge
{
	class Affectorable
	{
	public:
		Affectorable();

	public:
		AFFECTOR_ID addAffector( Affector * _affector );
		void stopAffector( AFFECTOR_ID _id );

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
		void updateAffector_( Affector * _affector, float _current, float _timing );

    protected:
        void updateAdd_();

	protected:
		typedef stdex::intrusive_slug_list_size<Affector> TVectorAffector;
        typedef stdex::intrusive_slug<TVectorAffector> TSlugAffector;
             
		TVectorAffector m_affectors;

		float m_angularSpeed;
		mt::vec3f m_linearSpeed;

		uint32_t m_enumerator;
	};
}
