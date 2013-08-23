#	include "ResourceEmitterContainer.h"
#	include "Kernel/ResourceImplement.h"

#	include "ResourceImageDefault.h"

#	include "Metacode.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceEmitterContainer );
	//////////////////////////////////////////////////////////////////////////
	ResourceEmitterContainer::ResourceEmitterContainer()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::setFilePath( const FilePath& _path )
	{
		m_filename = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath& ResourceEmitterContainer::getFilePath() const
	{
		return m_filename;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::setFolderPath( const FilePath& _folder )
	{
		m_folder = _folder;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath& ResourceEmitterContainer::getFolderPath() const
	{
		return m_folder;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitterContainer::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceEmitterContainer * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceEmitterContainer *>(_meta);

        metadata->swap_File_Path( m_filename );
        metadata->swap_Folder_Path( m_folder );

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceEmitterContainer::_isValid() const
    {
        const ConstString & category = this->getCategory();

        ParticleEmitterContainerInterfacePtr container = PARTICLE_SERVICE(m_serviceProvider)
            ->createEmitterContainerFromFile( category, m_filename );

        if( container == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceEmitterContainer::_isValid %s can't create container file '%s'"
                , m_name.c_str()
                , m_filename.c_str() 
                );

            return false;
        }

        const TVectorParticleEmitterAtlas & atlas = container->getAtlas();

        for( TVectorParticleEmitterAtlas::const_iterator
            it = atlas.begin(),
            it_end = atlas.end();
        it != it_end;
        ++it )
        {            
            const FilePath & filename = it->file;

            ConstString filepath = this->makeTexturePath_( filename );

            const ConstString & category = this->getCategory();

            InputStreamInterfacePtr stream = 
                FILE_SERVICE(m_serviceProvider)->openInputFile( category, filepath );

            if( stream == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)( "ResourceEmitterContainer::_isValid %s can't create texture '%s'"
                    , m_name.c_str()
                    , filepath.c_str() 
                    );

                return false;
            }

            const ConstString & codecType = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( filepath );

            ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(m_serviceProvider)
                ->createDecoderT<ImageDecoderInterfacePtr>( codecType, stream );

            if( imageDecoder == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)( "ResourceEmitterContainer::_isValid %s can't create decoder '%s'"
                    , m_name.c_str()
                    , codecType.c_str() 
                    );

                return false;
            }
        }

        if( container->isValid() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceEmitterContainer::_isValid %s can't valid container '%s'"
                , m_name.c_str()
                , m_filename.c_str() 
                );

            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitterContainer::_compile()
	{
		const ConstString & category = this->getCategory();

		m_container = PARTICLE_SERVICE(m_serviceProvider)
			->createEmitterContainerFromFile( category, m_filename );

		if( m_container == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourceEmitterContainer::_compile %s can't create container file '%s'"
                , m_name.c_str()
				, m_filename.c_str() 
				);

			return false;
		}
		
		const TVectorParticleEmitterAtlas & atlas = m_container->getAtlas();
		
		for( TVectorParticleEmitterAtlas::const_iterator
			it = atlas.begin(),
			it_end = atlas.end();
		it != it_end;
		++it )
		{
			const FilePath & filename = it->file;

            ConstString filepath = this->makeTexturePath_( filename );

            const ConstString & category = this->getCategory();

            const ConstString & codecType = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( filepath );

            RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE(m_serviceProvider)
                ->loadTexture( category, filepath, codecType, 0, 0 );

            if( texture == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)( "ResourceEmitterContainer::_compile %s can't create texture '%s'"
                    , m_name.c_str()
                    , filepath.c_str() 
                    );

                return false;
            }

			m_atlasRenderTextures.push_back( texture );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::createResource_( const ConstString & _fullname, const FilePath & _path )
	{
		const ConstString & category = this->getCategory();
		const ConstString & group = this->getGroup();

		ResourceImageDefault * image = RESOURCE_SERVICE(m_serviceProvider)
			->createResourceT<ResourceImageDefault>( category, group, _fullname, CONST_STRING(m_serviceProvider, ResourceImageDefault) );

		image->setImagePath( _path );
	}
    //////////////////////////////////////////////////////////////////////////
    ConstString ResourceEmitterContainer::makeTexturePath_( const FilePath & _filepath ) const
    {        
        static String cache_path;
        cache_path.assign( m_folder.c_str(), m_folder.size() );
        cache_path += MENGE_FOLDER_RESOURCE_DELIM;
        cache_path.append( _filepath.c_str(), _filepath.size() );

        ConstString name = Helper::stringizeString( m_serviceProvider, cache_path );

        return name;
    }
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::_release()
	{
		m_atlasRenderTextures.clear();

		m_container = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderTextureInterfacePtr & ResourceEmitterContainer::getAtlasTexture( size_t _atlasId )
	{
        const RenderTextureInterfacePtr & texture = m_atlasRenderTextures[_atlasId];

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	const ParticleEmitterContainerInterfacePtr & ResourceEmitterContainer::getContainer() const
	{
		return m_container;
	}
	//////////////////////////////////////////////////////////////////////////
}
