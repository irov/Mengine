#	include "AstralaxEmitterContainer.h"
#	include "AstralaxEmitter.h"
#	include "Utils/Core/String.h"
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer::AstralaxEmitterContainer()
	{
		m_metaData.size.x = 0.f;
		m_metaData.size.y = 0.f;
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
			EmitterPool & pool = it->second;

			for( TVectorEmittersId::iterator 
				it_pool = pool.emitters.begin(),
				it_pool_end = pool.emitters.end();
			it_pool != it_pool_end;
			++it_pool )
			{
				Magic_UnloadEmitter( *it_pool );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::addEmitterIds( const String & _name, TVectorEmittersId _emitters )
	{
		if( _emitters.empty() == true )
		{
			return;
		}

		TMapEmitters::iterator it_found = m_emittersIds.find( _name );

		if( it_found != m_emittersIds.end() )
		{
			return;
		}

		EmitterPool pool;
		pool.emitters = _emitters;
		pool.dublicate = _emitters.size() == 1;

		m_emittersIds.insert( std::make_pair( _name, pool ) );
	}
	//////////////////////////////////////////////////////////////////////////
	HM_EMITTER AstralaxEmitterContainer::getEmitterId( const String & _name )
	{
		TMapEmitters::iterator it = m_emittersIds.find( _name );

		if( it == m_emittersIds.end() )
		{
			return 0;
		}

		EmitterPool & pool = it->second;

		HM_EMITTER result_id;

		if( pool.dublicate == true )
		{
			HM_EMITTER etalon_id = pool.emitters[0];

			if( etalon_id == 0 )
			{
				return 0;
			}

			HM_EMITTER duplicated_id = Magic_DuplicateEmitter( etalon_id );
		
			result_id = duplicated_id;
		}
		else
		{
			if( pool.emitters.empty() == true )
			{
				return 0;
			}

			HM_EMITTER pool_id = pool.emitters.back();
			pool.emitters.pop_back();

			result_id = pool_id;
		}

		return result_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::releaseEmitterId( const String & _name, HM_EMITTER _id )
	{
		TMapEmitters::iterator it = m_emittersIds.find( _name );

		if( it == m_emittersIds.end() )
		{
			return;
		}

		if( it->second.dublicate == true )
		{
			Magic_UnloadEmitter( _id );
		}
		else
		{
			it->second.emitters.push_back( _id );
		}
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
	//////////////////////////////////////////////////////////////////////////
	EmitterInterface * AstralaxEmitterContainer::createEmitter( const String & _name )
	{
		HM_EMITTER id = this->getEmitterId( _name );

		if( id == 0 )
		{
			return NULL;
		}

		AstralaxEmitter * emitter = new AstralaxEmitter( this, id, _name );
		
		//m_emitters.push_back(emitter);
		return emitter;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::releaseEmitter( EmitterInterface * _emitter )
	{
		AstralaxEmitter * emitter = static_cast<AstralaxEmitter*>(_emitter);

		const String & name = emitter->getName();
		HM_EMITTER id = emitter->getId();
		AstralaxEmitterContainer * container = emitter->getContainer();

		container->releaseEmitterId( name, id );

		delete emitter;
	}
	////////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::visitContainer( EmitterContainerVisitor * visitor )
	{
		for( TMapEmitters::iterator 
			it = m_emittersIds.begin(), 
			it_end = m_emittersIds.end();
		it != it_end;
		++it )
		{
			visitor->visitEmitterName(it->first);
		}
		for (TVectorAtlas::iterator
			it = m_atlas.begin(),
			it_end =m_atlas.end();
		it!=it_end;
		it++)
		{
			visitor->visitAtlas(*(it));
		}
	}
	////////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitterContainer::isMetaData( const char * _data )
	{
		if( strstr( _data, "[" ) != NULL )
		{
			return true;
		}

		return false;
	}
	////////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::setMetaData( const char * _data )
	{
		String data(_data);
		int posOpen = data.find_first_of('[');
		int posClose = data.find_first_of(']');
		
		if( posOpen == String::npos || posClose == String::npos )
		{
			return;
		}

		String values = data.substr( posOpen + 1, posClose - posOpen - 1 );
		Utils::trim(values);

		TVectorString keysAndValues; 
		Utils::split(keysAndValues, values, false, ";");
		if( keysAndValues.empty() == true )
		{
			keysAndValues.push_back(values);
		}
		
		for(TVectorString::iterator 
			it = keysAndValues.begin()
			, it_end = keysAndValues.end();
			it != it_end;
			it ++)
		{
			TVectorString keyValue;
			Utils::split(keyValue, (*it), false, "=");
			
			if(keyValue.size() != 2)
			{
				return;
			}
			
			String key = keyValue.at(0);
			String val = keyValue.at(1);
			Utils::trim(key);
			Utils::trim(val);
			if( key.compare("Size") == 0 )
			{
				TVectorString widthHeight;
				Utils::split(widthHeight, val, false, ",");
				if( widthHeight.size() != 2 )
				{
					return;
				}

				String width = widthHeight.at(0);
				String height = widthHeight.at(1);
				Utils::trim(width);
				Utils::trim(height);
				m_metaData.size.x = (float) atof(width.c_str());
				m_metaData.size.y = (float) atof(height.c_str());
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////
	const EmitterContainerMetaData& AstralaxEmitterContainer::getMetaData() const 
	{
		return m_metaData;
	}
	////////////////////////////////////////////////////////////////////////////
}