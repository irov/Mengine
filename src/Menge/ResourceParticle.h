#	pragma once

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"

#	include "Kernel/ResourceReference.h"
#	include "Kernel/ResourceImage.h"

#	include "Utils/Core/Polygon.h"

namespace Menge
{
	class ResourceParticle
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		ResourceParticle();

	public:
		void setFilePath( const FilePath& _path );
		const FilePath& getFilePath() const;

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
		FilePath m_fileName;

		ConstString m_converterType;

		typedef stdex::vector<ConstString> TVectorResourceImageName;
		TVectorResourceImageName m_resourceImageNames;

		ParticleEmitterContainerInterface2Ptr m_container;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceParticle> ResourceParticlePtr;
}
