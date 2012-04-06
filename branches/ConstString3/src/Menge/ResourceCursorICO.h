#	pragma once
#	include "ResourceReference.h"

namespace Menge
{
	class ResourceCursorICO
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceCursorICO )

	public:
		const WString& getPath() const;

	protected:
		void loader( BinParser * _parser ) override;

	protected:
		WString m_path;

	};
}