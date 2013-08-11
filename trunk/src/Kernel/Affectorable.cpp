#	include "Kernel/Affectorable.h"
#	include "Kernel/Affector.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Affectorable::Affectorable()
		: m_angularSpeed(0.f)
		, m_linearSpeed(0.f, 0.f, 0.f)
		, m_enumerator(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Affectorable::addAffector( Affector * _affector )
	{
		size_t id = ++m_enumerator;

		_affector->setId( id );

		m_affectors.push_back( _affector );

		return id;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::stopAffectors( EAffectorType _type )
	{
        for( TSlugAffector it(m_affectors); it.eof() == false; it.next_shuffle() )
        {
            Affector * affector = *it;

            if( affector->getType() == _type )
            {
                affector->unlink();
                affector->stop();

                affector->destroy();
            }
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::stopAllAffectors()
	{
        for( TSlugAffector it(m_affectors); it.eof() == false; it.next_shuffle() )
        {
            Affector * affector = *it;

            affector->unlink();
			affector->stop();

            affector->destroy();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::updateAffectors( float _current, float _timing )
	{
        (void)_current;

        for( TSlugAffector it(m_affectors); it.eof() == false; it.next_shuffle() )
        {
            Affector * affector = *it;
            
			bool end = affector->affect( _timing );

			if( end == true )
			{
                affector->unlink();
                affector->complete();

                affector->destroy();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::setAngularSpeed( float _angular )
	{
		m_angularSpeed = _angular;
	}
	//////////////////////////////////////////////////////////////////////////
	float Affectorable::getAngularSpeed() const
	{
		return m_angularSpeed;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::setLinearSpeed( const mt::vec3f & _linearSpeed )
	{
		m_linearSpeed = _linearSpeed;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Affectorable::getLinearSpeed() const
	{
		return m_linearSpeed;
	}
}
