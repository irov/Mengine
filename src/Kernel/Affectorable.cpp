#include "Kernel/Affectorable.h"
#include "Kernel/Affector.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Affectorable::Affectorable()
		: m_angularSpeed(0.f)
		, m_linearSpeed(0.f, 0.f, 0.f)
		, m_enumerator(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Affectorable::~Affectorable()
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	AFFECTOR_ID Affectorable::addAffector( Affector * _affector )
	{
		if( _affector == nullptr )
		{
			return INVALID_AFFECTOR_ID;
		}

		AFFECTOR_ID id = ++m_enumerator;

		_affector->setId( id );
		
		if( _affector->prepare() == false )
		{
			return INVALID_AFFECTOR_ID;
		}

		m_affectors.push_back( _affector );

		return id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Affectorable::stopAffector( uint32_t _id )
	{
		for( TSlugAffector it(m_affectors); it.eof() == false; )
		{
			Affector * affector = *it;

			it.next_shuffle();

			if( affector->getId() == _id )
			{
				m_affectors.remove( affector );

				affector->stop();
				affector->destroy();

				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::stopAffectors( EAffectorType _type )
	{
        for( TSlugAffector it(m_affectors); it.eof() == false; )
        {
            Affector * affector = *it;

			it.next_shuffle();

            if( affector->getAffectorType() == _type )
            {
				m_affectors.remove( affector );

				affector->stop();
                affector->destroy();
            }
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::stopAllAffectors()
	{
        for( TSlugAffector it(m_affectors); it.eof() == false; )
        {
            Affector * affector = *it;

			it.next_shuffle();

			m_affectors.remove( affector );

			affector->stop();
            affector->destroy();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::updateAffectors( float _current, float _timing )
	{
        (void)_current;

		Affector * single_affector = m_affectors.single();

		if( single_affector != nullptr )
		{
			this->updateAffector_( single_affector, _current, _timing );
		}
		else
		{
			for( TSlugAffector it(m_affectors); it.eof() == false; )
			{
				Affector * affector = *it;

				it.next_shuffle();

				this->updateAffector_( affector, _current, _timing );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::updateAffector_( Affector * _affector, float _current, float _timing )
	{
		(void)_current;

		bool end = _affector->affect( _timing );

		if( end == true )
		{
			m_affectors.remove( _affector );

			_affector->complete();
			_affector->destroy();
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
