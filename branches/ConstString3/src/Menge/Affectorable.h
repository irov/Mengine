#	pragma once

#	include "AffectorType.h"

#	include "Core/IntrusiveList.h"

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
		void stopAllAffectors();

	public:
		void clear();

	public:
		void setAngularSpeed( float _angular );
		float getAngularSpeed() const;

		void setLinearSpeed( const mt::vec3f & _linearSpeed );
		const mt::vec3f & getLinearSpeed() const;

	public:
		void update( float _current, float _timing );

	protected:
		typedef IntrusiveList<Affector> TVectorAffector;
		TVectorAffector m_affectorsToProcess;
		TVectorAffector m_affectorsToAdd;

		float m_angularSpeed;
		mt::vec3f m_linearSpeed;

		size_t m_enumerator;
	};
}
