#include "Graveyard.h"

#include "Interface/RenderImageInterface.h"
#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderTextureServiceInterface.h"

#include "Interface/ConfigInterface.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Graveyard, Mengine::Graveyard );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Graveyard::Graveyard()
		: m_count(0)
		, m_graveyardTime(1000.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////		
	Graveyard::~Graveyard()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Graveyard::_initializeService()
	{
		m_graveyardTime = CONFIG_VALUE("Engine", "GraveyardTime", 1000.f);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Graveyard::_finalizeService()
	{
		this->clearTextures();
	}
	//////////////////////////////////////////////////////////////////////////
	void Graveyard::clearTextures()
	{
        for( RenderTextureGraveEntry & entry : m_textures )
		{
			m_count--;

			entry.image = nullptr;
		}
		
		m_textures.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class FGraveyardDead
		{
		public:
			FGraveyardDead( float _timing )
				: m_timing( _timing )
			{
			}

		public:
			bool operator ()( RenderTextureGraveEntry & _entry ) const
			{
				_entry.timing -= m_timing;

				if( _entry.timing > 0.f )
				{
					return false;
				}			

				return true;
			}

		protected:
			float m_timing;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void Graveyard::tick( float _time, float _timing )
	{
        (void)_time;

		TVectorTextureGrave::iterator it_erase = std::remove_if( m_textures.begin(), m_textures.end(), FGraveyardDead( _timing ) );

		for( TVectorTextureGrave::iterator
			it = it_erase,
			it_end = m_textures.end();
		it != it_end;
		++it )
		{
			RenderTextureGraveEntry & entry = *it;

			entry.image = nullptr;

			m_count--;
		}

		m_textures.erase( it_erase, m_textures.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Graveyard::buryTexture( RenderTextureInterface * _texture )
	{
		if( this->isInitializeService() == false )
		{
			return false;
		}

		const FileGroupInterfacePtr & category = _texture->getCategory();
		const FilePath & filePath = _texture->getFileName();

		if( filePath.empty() == true )
		{
			return false;
		}

		if( mt::equal_f_z( m_graveyardTime ) == true )
		{
			return false;
		}

		m_count++;

		RenderTextureGraveEntry entry;

		entry.category = category;
		entry.filePath = filePath;

		entry.image = _texture->getImage();
		entry.width = _texture->getWidth();
		entry.height = _texture->getHeight();

		entry.timing = m_graveyardTime;

		m_textures.emplace_back( entry );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class FGraveyardFind
		{
		public:
			FGraveyardFind( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath )
				: m_fileGroup(_fileGroup)
				, m_filePath(_filePath)
			{
			}

		public:
			bool operator() ( const RenderTextureGraveEntry & _entry )
			{
				if( _entry.category != m_fileGroup )
				{
					return false;
				}

				if( _entry.filePath != m_filePath )
				{
					return false;
				}

				return true;
			}

		protected:
			void operator = (const FGraveyardFind & _find)
			{
				(void)_find;
			}

		protected:
            const FileGroupInterfacePtr& m_fileGroup;
			const FilePath & m_filePath;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr Graveyard::resurrectTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath )
	{
		if( _filePath.empty() == true )
		{
			return nullptr;
		}

		if( mt::equal_f_z( m_graveyardTime ) == true )
		{
			return nullptr;
		}					

		TVectorTextureGrave::iterator it_found = std::find_if( m_textures.begin(), m_textures.end(), FGraveyardFind( _fileGroup, _filePath ) );

		if( it_found == m_textures.end() )
		{
			return nullptr;
		}

		--m_count;

		RenderTextureGraveEntry & entry = *it_found;

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
			->createRenderTexture( entry.image, entry.width, entry.height );

		entry.image = nullptr;

		*it_found = m_textures.back();
		m_textures.pop_back();

		return texture;
	}
}