#	include "AstralaxEmitterContainer.h"
#	include "AstralaxEmitter.h"
//////////////////////////////////////////////////////////////////////////
AstralaxEmitterContainer::AstralaxEmitterContainer( HM_FILE _hFile )
: m_hfile( _hFile )
{
};
//////////////////////////////////////////////////////////////////////////
AstralaxEmitterContainer::~AstralaxEmitterContainer()
{
	/*for( TMapEmitters::iterator it = m_emittersIds.begin(), it_end = m_emittersIds.end();
		it != it_end;
		it++ )
	{
		Magic_UnloadEmitter( it->second );
	}*/
	Magic_CloseFile( m_hfile );
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitterContainer::addEmitterId( const Menge::String & _name,  HM_EMITTER _id )
{
	m_emittersIds.insert( std::make_pair( _name, _id ) );
}
//////////////////////////////////////////////////////////////////////////
HM_EMITTER AstralaxEmitterContainer::getEmitter( const Menge::String & _name ) const
{
	TMapEmitters::const_iterator it = m_emittersIds.find( _name );

	if( it == m_emittersIds.end() )
	{
		return 0;
	}

	HM_EMITTER id = it->second;

	if( id == 0 )
	{
		return 0;
	}

	return id;
}
//////////////////////////////////////////////////////////////////////////
