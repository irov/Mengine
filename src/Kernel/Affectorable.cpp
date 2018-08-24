#include "Kernel/Affectorable.h"
#include "Kernel/Affector.h"

#include "Interface/TimelineInterface.h"

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
    AFFECTOR_ID Affectorable::addAffector( const AffectorPtr & _affector )
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
            AffectorPtr affector = *it;

			it.next_shuffle();

			if( affector->getId() == _id )
			{
				m_affectors.remove( affector );

				affector->stop();

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
            AffectorPtr affector = *it;

			it.next_shuffle();

            if( affector->getAffectorType() == _type )
            {
				m_affectors.remove( affector );

				affector->stop();
            }
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::stopAllAffectors()
	{
        for( TSlugAffector it(m_affectors); it.eof() == false; )
        {
            AffectorPtr affector = *it;

			it.next_shuffle();

			m_affectors.remove( affector );

			affector->stop();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::updateAffectors( const UpdateContext * _context )
	{
		AffectorPtr single_affector = m_affectors.single();

		if( single_affector != nullptr )
		{
			this->updateAffector_( single_affector, _context );
		}
		else
		{
			for( TSlugAffector it(m_affectors); it.eof() == false; )
			{
				AffectorPtr affector = *it;

				it.next_shuffle();

				this->updateAffector_( affector, _context );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::updateAffector_( const AffectorPtr & _affector, const UpdateContext * _context )
	{
        float used = 0.f;
		bool end = _affector->affect( _context, &used );

		if( end == true )
		{
			m_affectors.remove( _affector );

            TIMELINE_SERVICE()
                ->beginOffset( used );

			_affector->complete();

            TIMELINE_SERVICE()
                ->endOffset();
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
