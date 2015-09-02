#	include "Graveyard.h"

#	include "Interface/ConfigInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Graveyard, Menge::GraveyardInterface, Menge::Graveyard );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Graveyard::Graveyard()
		: m_serviceProvider(nullptr)
		, m_count(0)
		, m_graveyardTime(1000.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////		
	Graveyard::~Graveyard()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Graveyard::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * Graveyard::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Graveyard::initialize()
	{
		m_graveyardTime = CONFIG_VALUE(m_serviceProvider, "Engine", "GraveyardTime", 1000.f);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Graveyard::finalize()
	{
		this->clearTextures();
	}
	//////////////////////////////////////////////////////////////////////////
	void Graveyard::clearTextures()
	{
		for( TVectorTextureGrave::iterator
			it = m_textures.begin(),
			it_end = m_textures.end();
		it != it_end;
		++it )
		{
			RenderTextureGraveEntry & entry = *it;

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
	void Graveyard::update( float _timing )
	{
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
		const ConstString & category = _texture->getCategory();
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
		entry.mipmaps = _texture->getMipmaps();
		entry.width = _texture->getWidth();
		entry.height = _texture->getHeight();
		entry.channels = _texture->getChannels();
		entry.timing = m_graveyardTime;

		m_textures.push_back( entry );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class FGraveyardFind
		{
		public:
			FGraveyardFind( const ConstString& _pakName, const FilePath & _filePath )
				: m_pakName(_pakName)
				, m_filePath(_filePath)
			{
			}

		public:
			bool operator() ( const RenderTextureGraveEntry & _entry )
			{
				if( _entry.category != m_pakName )
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
			const ConstString& m_pakName;
			const FilePath & m_filePath;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr Graveyard::resurrectTexture( const ConstString& _pakName, const FilePath & _filePath )
	{
		if( _filePath.empty() == true )
		{
			return nullptr;
		}

		if( mt::equal_f_z( m_graveyardTime ) == true )
		{
			return nullptr;
		}					

		TVectorTextureGrave::iterator it_found = std::find_if( m_textures.begin(), m_textures.end(), FGraveyardFind( _pakName, _filePath ) );

		if( it_found == m_textures.end() )
		{
			return nullptr;
		}

		--m_count;

		RenderTextureGraveEntry & entry = *it_found;

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE( m_serviceProvider )
			->createRenderTexture( entry.image, entry.mipmaps, entry.width, entry.height, entry.channels );

		entry.image = nullptr;

		*it_found = m_textures.back();
		m_textures.pop_back();

		return texture;
	}
}