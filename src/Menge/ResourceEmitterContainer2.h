//#	pragma once
//
//#   include "Interface/RenderSystemInterface.h"
//#   include "Interface/ParticleSystemInterface2.h"
//
//#	include "Kernel/ResourceReference.h"
//
//#	include "Utils/Core/Polygon.h"
//
//#	include <map>
//
//namespace Menge
//{
//	class ResourceImage;
//
//	class ResourceEmitterContainer2
//		: public ResourceReference
//	{
//		RESOURCE_DECLARE( ResourceEmitterContainer2 )
//
//	public:
//		ResourceEmitterContainer2();
//
//	public:
//		void setFilePath( const FilePath& _path );
//		const FilePath& getFilePath() const;
//		
//	public:
//		const ParticleEmitterContainerInterface2Ptr & getContainer() const;
//
//	public:
//		ResourceImage * getAtlasImageResource( size_t _atlasId ) const;
//		size_t getAtlasImageCount() const;
//
//	public:
//		bool _loader( const Metabuf::Metadata * _parser ) override;
//		bool _convert() override;
//
//	protected:
//		bool _compile() override;
//		void _release() override;
//
//    protected:
//        bool _isValid() const override;
//
//	private:
//		FilePath m_fileName;
//		
//		ConstString m_converterType;
//
//		typedef std::vector<ConstString> TVectorResourceImageName;
//		TVectorResourceImageName m_resourceImageNames;
//		
//		ParticleEmitterContainerInterface2Ptr m_container;
//
//		typedef std::vector<ResourceImage *> TVectorResourceImage;
//		TVectorResourceImage m_resourceImages;
//	};
//}
