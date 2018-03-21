#pragma once

#include "Kernel/ResourceReference.h"

#include "Core/FilePath.h"

namespace Mengine
{
	class ResourceFile
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

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
	typedef stdex::intrusive_ptr<ResourceFile> ResourceFilePtr;
}
