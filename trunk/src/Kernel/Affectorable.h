#	pragma once

#	include "Kernel/AffectorType.h"

#	include "stdex/intrusive_list.h"
#   include "stdex/intrusive_slug.h"

#	include "Math/vec3.h"

namespace Menge
{
	class Affector;

	class Affectorable
	{
	public:
		Affectorable();

	public:
		size_t addAffector( Affector* _affector );
		void stopAffectors( EAffectorType _type );

    public:
		void stopAllAffectors();
        
	public:
		void setAngularSpeed( float _angular );
		float getAngularSpeed() const;

		void setLinearSpeed( const mt::vec3f & _linearSpeed );
		const mt::vec3f & getLinearSpeed() const;

	public:
		void update( float _current, float _timing );

    protected:
        void updateAdd_();

	protected:
        typedef stdex::intrusive_list<Affector> TVectorAffector;
        typedef stdex::intrusive_slug<Affector> TSlugAffector;

		TVectorAffector m_affectorsToProcess;
		TVectorAffector m_affectorsToAdd;

		float m_angularSpeed;
		mt::vec3f m_linearSpeed;

		size_t m_enumerator;
	};
}
