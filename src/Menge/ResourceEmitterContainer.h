#	pragma once

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"

#	include "Kernel/ResourceReference.h"

#	include "Utils/Core/Polygon.h"

#	include <map>

namespace Menge
{
	class ResourceImageDefault;

	class ResourceEmitterContainer
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceEmitterContainer )

	public:
		ResourceEmitterContainer();

	public:
		void setFilePath( const FilePath& _path );
		const FilePath& getFilePath() const;

		void setFolderPath( const FilePath& _path );
		const FilePath& getFolderPath() const;
		
	public:
		const ParticleEmitterContainerInterfacePtr & getContainer() const;

	public:
		const RenderTextureInterfacePtr & getAtlasTexture( size_t _atlasId );

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;
	
	protected:
		bool _compile() override;
		void _release() override;

    protected:
        bool _isValid() const override;

	protected:
		void createResource_( const ConstString & _fullname, const FilePath & _path );

    protected:
        ConstString makeTexturePath_( const FilePath & _filepath ) const;

	private:
		FilePath m_filename;
		FilePath m_folder;
		
		ParticleEmitterContainerInterfacePtr m_container;

		typedef std::vector<RenderTextureInterfacePtr> TVectorAtlasRenderTextures;
		TVectorAtlasRenderTextures m_atlasRenderTextures;
	};
}
