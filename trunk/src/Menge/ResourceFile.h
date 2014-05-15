#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "Core/FilePath.h"

namespace Menge
{
	class ResourceFile
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceFile )

	public:
		ResourceFile();
		~ResourceFile();

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	public:
		bool _isValid() const override;
        
	public:
		const FilePath & getFilePath() const;

	protected:
		FilePath m_path;
	};
}
