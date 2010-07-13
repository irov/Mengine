#	pragma once

#	include "ResourceReference.h"

namespace Menge
{
	class ResourceBinary
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceBinary )

	public:
		const TBlobject & getBlobject() const;

	public:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;

	protected:
		ConstString m_path;
		TBlobject m_blobject;
	};
}