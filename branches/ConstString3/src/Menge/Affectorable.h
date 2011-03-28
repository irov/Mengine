#	pragma once

#	include "AffectorType.h"

#	include "Core/IntrusiveList.h"

#	include "Math/vec2.h"

namespace Menge
{
	class Affector;

	class Affectorable
	{
	public:
		Affectorable();

	public:
		void addAffector( Affector* _affector );
		void stopAffectors( EAffectorType _type );
		void stopAllAffectors();

	public:
		void clear();

	public:
		void setAngularSpeed( float _angular );
		float getAngularSpeed() const;

		void setLinearSpeed( const mt::vec2f & _linearSpeed );
		const mt::vec2f & getLinearSpeed() const;

	public:
		void update( float _timing );

	protected:
		typedef IntrusiveList<Affector> TVectorAffector;
		TVectorAffector m_affectorsToProcess;
		TVectorAffector m_affectorsToAdd;

		float m_angularSpeed;
		mt::vec2f m_linearSpeed;
	};
}
