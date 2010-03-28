#	include "Affectorable.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Affectorable::Affectorable()
		: m_angularSpeed(0.f)
		, m_linearSpeed(0.f,0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::addAffector( Affector* _affector )
	{
		if( _affector == NULL )
		{
			return;
		}

		ETypeAffector affectorType = _affector->getType();
		stopAffectors( affectorType );

		m_affectorsToAdd.push_back( _affector );
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::stopAffectors( ETypeAffector _type )
	{
		for( TAffectorVector::iterator 
			it = m_affectorsToProcess.begin(), 
			it_end = m_affectorsToProcess.end();
		it != it_end; 
		++it )
		{
			if( (*it)->getType() == _type )
			{
				(*it)->stop();
			}
		}

		for( TAffectorVector::iterator 
			it = m_affectorsToAdd.begin(), 
			it_end = m_affectorsToAdd.end(); 
		it != it_end; 
		++it )
		{
			if( (*it)->getType() == _type )
			{
				(*it)->stop();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::update( float _timing )
	{
		if( m_affectorsToAdd.empty() == false )
		{
			m_affectorsToProcess.insert( m_affectorsToProcess.end()
				, m_affectorsToAdd.begin()
				, m_affectorsToAdd.end()
				);

			m_affectorsToAdd.clear();
		}

		for( TAffectorVector::iterator 
			it = m_affectorsToProcess.begin();
			it != m_affectorsToProcess.end();
		/*++it*/ )
		{
			bool end = (*it)->affect( _timing );
			if( end == true )
			{
				delete (*it);
				*it = m_affectorsToProcess.back();
				m_affectorsToProcess.pop_back();
			}
			else
			{
				++it;
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
	void Affectorable::setLinearSpeed( const mt::vec2f & _linearSpeed )
	{
		m_linearSpeed = _linearSpeed;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Affectorable::getLinearSpeed() const
	{
		return m_linearSpeed;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::clear()
	{
		for( TAffectorVector::const_iterator
			it = m_affectorsToProcess.begin(),
			it_end = m_affectorsToProcess.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		m_affectorsToProcess.clear();

		for( TAffectorVector::const_iterator
			it = m_affectorsToAdd.begin(),
			it_end = m_affectorsToAdd.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		m_affectorsToAdd.clear();
	}
}
