#pragma once

#   include "Interface/ParticleSystemInterface.h"

#include "Kernel/ResourceReference.h"
#include "Kernel/ResourceImage.h"

#include "Utils/Core/Polygon.h"

namespace Mengine
{
	class ResourceParticle
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		ResourceParticle();

	public:
		void setFilePath( const FilePath& _filePath );
		const FilePath& getFilePath() const;

    public:
        void setConverterType( const ConstString & _converterType );
        const ConstString & getConverterType() const;

    public:
        void addResourceImage( const ResourceImagePtr & _resourceImage );
        uint32_t getResourceImageCount() const;
        const ResourceImagePtr & getResourceImage( uint32_t _index ) const;

	public:
		ParticleEmitterInterfacePtr createEmitter();

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;
		bool _convert() override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		bool _isValid() const override;

	protected:
		FilePath m_filePath;

		ConstString m_converterType;

		typedef stdex::vector<ResourceImagePtr> TVectorResourceImages;
		TVectorResourceImages m_resourceImages;

		ParticleEmitterContainerInterface2Ptr m_container;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceParticle> ResourceParticlePtr;
}
