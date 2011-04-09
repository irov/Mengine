#	include "AstralaxEmitterContainer.h"
#	include "AstralaxEmitter.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer::AstralaxEmitterContainer()
	{
	};
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer::~AstralaxEmitterContainer()
	{
		for( TMapEmitters::iterator 
			it = m_emittersIds.begin(), 
			it_end = m_emittersIds.end();
		it != it_end;
		++it )
		{
			Magic_UnloadEmitter( it->second );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::addEmitterId( const String & _name,  HM_EMITTER _id )
	{
		m_emittersIds.insert( std::make_pair( _name, _id ) );
	}
	//////////////////////////////////////////////////////////////////////////
	HM_EMITTER AstralaxEmitterContainer::getEmitter( const String & _name ) const
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
	void AstralaxEmitterContainer::addAtlas( const EmitterAtlas & _atlas )
	{
		m_atlas.push_back( _atlas );
	}
	//////////////////////////////////////////////////////////////////////////
	const EmitterContainerInterface::TVectorAtlas & AstralaxEmitterContainer::getAtlas() const
	{
		return m_atlas;
	}
}