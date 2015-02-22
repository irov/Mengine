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
		m_graveyardTime = CONFIG_VALUE(m_serviceProvider, "Engine", "Gravey1ardTime", 1000.f);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Graveyard::finalize()
	{
		for( TMapTextureGrave::iterator
			it = m_textures.begin(),
			it_end = m_textures.end();
		it != it_end;
		++it )
		{
			RenderTextureGraveDesc & desc = m_textures.get_value( it );

			desc.image = nullptr;
		}

		m_textures.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Graveyard::update( float _timing )
	{
		for( TMapTextureGrave::iterator it = m_textures.begin(); it != m_textures.end(); )
		{
			RenderTextureGraveDesc & desc = m_textures.get_value( it );

			desc.timing -= _timing;

			if( desc.timing > 0.f )
			{
				++it;

				continue;
			}

			desc.image = nullptr;

			it = m_textures.erase( it );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Graveyard::buryTexture( const FilePath & _path, RenderTextureInterface * _texture )
	{
		if( _path.empty() == true )
		{
			return;
		}

		if( mt::cmp_f_z( m_graveyardTime ) == true )
		{
			return;
		}

		RenderTextureGraveDesc desc;

		desc.image = _texture->getImage();
		desc.mipmaps = _texture->getMipmaps();
		desc.width = _texture->getWidth();
		desc.height = _texture->getHeight();
		desc.channels = _texture->getChannels();
		desc.timing = m_graveyardTime;

		m_textures.insert( _path, desc );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr Graveyard::resurrectTexture( const FilePath & _path )
	{
		if( _path.empty() == true )
		{
			return nullptr;
		}

		if( mt::cmp_f_z( m_graveyardTime ) == true )
		{
			return nullptr;
		}

		TMapTextureGrave::iterator it_found = m_textures.find( _path );

		if( it_found == m_textures.end() )
		{
			return nullptr;
		}
		
		RenderTextureGraveDesc & desc = m_textures.get_value( it_found );

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE(m_serviceProvider)
			->createRenderTexture( desc.image, desc.mipmaps, desc.width, desc.height, desc.channels );

		m_textures.erase( it_found );

		return texture;
	}
}