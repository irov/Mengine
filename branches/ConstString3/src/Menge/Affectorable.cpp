#	include "Affectorable.h"
#	include "Affector.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Affectorable::Affectorable()
		: m_angularSpeed(0.f)
		, m_linearSpeed(0.f,0.f)
		, m_enumerator(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Affectorable::addAffector( Affector* _affector )
	{
		if( _affector == NULL )
		{
			return 0;
		}

		std::size_t id = ++m_enumerator;

		_affector->setId(id);

		m_affectorsToAdd.push_back( _affector );

		return id;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::stopAffectors( EAffectorType _type )
	{
		for( TVectorAffector::iterator 
			it = m_affectorsToProcess.begin(), 
			it_end = m_affectorsToProcess.end();
		it != it_end; 
		/*++it*/ )
		{
			if( (*it)->getType() == _type )
			{
				(*it)->stop();

				Affector * affector = *it;
				it = m_affectorsToProcess.erase( it );
				delete affector;
			}
			else
			{
				++it;
			}
		}

		for( TVectorAffector::iterator 
			it = m_affectorsToAdd.begin(), 
			it_end = m_affectorsToAdd.end(); 
		it != it_end; 
		/*++it*/ )
		{
			if( (*it)->getType() == _type )
			{
				(*it)->stop();

				Affector * affector = *it;
				it = m_affectorsToAdd.erase( it );
				delete affector;
			}
			else
			{
				++it;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Affectorable::stopAllAffectors()
	{
		for( TVectorAffector::iterator 
			it = m_affectorsToProcess.begin(), 
			it_end = m_affectorsToProcess.end();
		it != it_end; 
		/*++it*/ )
		{
			(*it)->stop();

			Affector * aff = *it;
			it = m_affectorsToProcess.erase( it );
			delete aff;
		}

		for( TVectorAffector::iterator 
			it = m_affectorsToAdd.begin(), 
			it_end = m_affectorsToAdd.end(); 
		it != it_end; 
		/*++it*/ )
		{
			(*it)->stop();

			Affector * aff = *it;
			it = m_affectorsToAdd.erase( it );
			delete aff;
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
				Affector * affector = *it;
				it = m_affectorsToProcess.erase( it );
				delete affector;
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
		for( TVectorAffector::iterator
			it = m_affectorsToProcess.begin(),
			it_end = m_affectorsToProcess.end();
		it != it_end;
		/*++it*/ )
		{
			Affector * aff = *it;
			it = m_affectorsToProcess.erase( it );
			delete aff;
		}

		for( TVectorAffector::iterator
			it = m_affectorsToAdd.begin(),
			it_end = m_affectorsToAdd.end();
		it != it_end;
		/*++it*/ )
		{
			Affector * aff = *it;
			it = m_affectorsToAdd.erase( it );
			delete aff;
		}
	}
}
