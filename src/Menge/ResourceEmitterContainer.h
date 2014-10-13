#	pragma once

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"

#	include "Kernel/ResourceReference.h"

#	include "Core/Polygon.h"

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
		void setFilePath( const ConstString & _path );
		const ConstString & getFilePath() const;

		void setFolderPath( const ConstString & _path );
		const ConstString & getFolderPath() const;
		
	public:
		const ParticleEmitterContainerInterfacePtr & getContainer() const;

	public:
		const RenderTextureInterfacePtr & getAtlasTexture( uint32_t _atlasId );
		uint32_t getAtlasTextureCount() const;

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;
	
	protected:
		bool _compile() override;
		void _release() override;

    protected:
        bool _isValid() const override;

    protected:
        ConstString makeTexturePath_( const ConstString & _filepath ) const;

	protected:
		ParticleEmitterContainerInterfacePtr compileContainer_( const FilePath & _path );

	private:
		ConstString m_fileName;
		ConstString m_folder;
		
		ParticleEmitterContainerInterfacePtr m_container;

		typedef stdex::vector<RenderTextureInterfacePtr> TVectorAtlasRenderTextures;
		TVectorAtlasRenderTextures m_atlasRenderTextures;
	};
}
