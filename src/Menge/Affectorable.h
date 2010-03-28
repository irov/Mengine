#	pragma once

#	include "Affector.h"

#	include "Math/vec2.h"

namespace Menge
{
	class Affectorable
	{
	public:
		Affectorable();

	public:
		void addAffector( Affector* _affector );
		void stopAffectors( ETypeAffector _type );

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
		typedef std::vector<Affector*> TAffectorVector;
		TAffectorVector m_affectorsToProcess;
		TAffectorVector m_affectorsToAdd;

		float m_angularSpeed;
		mt::vec2f m_linearSpeed;
	};
}
