#	pragma once
#	include "ResourceReference.h"

namespace Menge
{
	class ResourceCursorICO
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceCursorICO )

	public:
		const ConstString& getPath() const;

	protected:
		void loader( BinParser * _parser ) override;

	protected:
		ConstString m_path;

	};
}