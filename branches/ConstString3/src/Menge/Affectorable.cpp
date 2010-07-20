#	include "Affectorable.h"
#	include "Affector.h"

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

		m_affectorsToAdd.push_back( _affector );
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::stopAffectors( EAffectorType _type )
	{
		for( TVectorAffector::iterator 
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

		for( TVectorAffector::iterator 
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

		for( TVectorAffector::iterator 
			it = m_affectorsToProcess.begin();
			it != m_affectorsToProcess.end();
		/*++it*/ )
		{
			bool end = (*it)->affect( _timing );
			if( end == true )
			{
				delete *it;
				it = m_affectorsToProcess.erase( it );
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
		for( TVectorAffector::const_iterator
			it = m_affectorsToProcess.begin(),
			it_end = m_affectorsToProcess.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		m_affectorsToProcess.clear();

		for( TVectorAffector::const_iterator
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
