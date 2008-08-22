#	include "AstralaxEmitterContainer.h"
#	include "AstralaxEmitter.h"
//////////////////////////////////////////////////////////////////////////
AstralaxEmitterContainer::AstralaxEmitterContainer()
{
};
//////////////////////////////////////////////////////////////////////////
AstralaxEmitterContainer::~AstralaxEmitterContainer()
{
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitterContainer::addEmitterId( const Menge::String & _name,  HM_EMITTER _id )
{
	m_emittersIds.insert( std::make_pair( _name, _id ) );
}
//////////////////////////////////////////////////////////////////////////
HM_EMITTER AstralaxEmitterContainer::getEmitter( const Menge::String & _name ) const
{
	std::map<Menge::String, HM_EMITTER>::const_iterator it = m_emittersIds.find( _name );

	if( it == m_emittersIds.end() )
	{
		return NULL;
	}

	HM_EMITTER id = it->second;

	if( id == 0 )
	{
		return NULL;
	}

	return id;
}
//////////////////////////////////////////////////////////////////////////