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
		for( TMapTextureGrave::iterator
			it = m_textures.begin(),
			it_end = m_textures.end();
		it != it_end;
		++it )
		{
			RenderTextureGraveEntry * entry = *it;

			entry->image = nullptr;
		}
		
		m_textures.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Graveyard::update( float _timing )
	{
		m_textures.foreach( this, &Graveyard::updateTexture_, _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Graveyard::updateTexture_( RenderTextureGraveEntry * _entry, float _timing )
	{
		_entry->timing -= _timing;

		if( _entry->timing > 0.f )
		{
			return;
		}

		_entry->image = nullptr;

		m_textures.erase_node( _entry );
	}
	//////////////////////////////////////////////////////////////////////////
	void Graveyard::buryTexture( RenderTextureInterface * _texture )
	{
		const ConstString & category = _texture->getCategory();
		const FilePath & filePath = _texture->getFileName();

		if( filePath.empty() == true )
		{
			return;
		}

		if( mt::cmp_f_z( m_graveyardTime ) == true )
		{
			return;
		}

		RenderTextureGraveEntry * entry = m_textures.create();

		entry->category = category;
		entry->filePath = filePath;

		entry->image = _texture->getImage();
		entry->mipmaps = _texture->getMipmaps();
		entry->width = _texture->getWidth();
		entry->height = _texture->getHeight();
		entry->channels = _texture->getChannels();
		entry->timing = m_graveyardTime;

		m_textures.insert( entry, nullptr );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr Graveyard::resurrectTexture( const ConstString& _pakName, const FilePath & _filePath )
	{
		if( _filePath.empty() == true )
		{
			return nullptr;
		}

		if( mt::cmp_f_z( m_graveyardTime ) == true )
		{
			return nullptr;
		}

		RenderTextureGraveEntry * entry;
		if( m_textures.has( _pakName, _filePath, &entry ) == false )
		{
			return nullptr;
		}
		
		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE(m_serviceProvider)
			->createRenderTexture( entry->image, entry->mipmaps, entry->width, entry->height, entry->channels );

		m_textures.erase_node( entry );

		return texture;
	}
}