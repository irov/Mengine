#pragma once

#include "Kernel/Resource.h"

#include "Core/FilePath.h"

namespace Mengine
{
	class ResourceFile
		: public Resource
	{
		DECLARE_VISITABLE( Resource );

	public:
		ResourceFile();
		~ResourceFile() override;

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	public:
		bool _isValid() const override;
        
	public:
		const FilePath & getFilePath() const;

	protected:
		FilePath m_path;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ResourceFile> ResourceFilePtr;
}
