#	pragma once

#	include "ResourceReference.h"

namespace Menge
{
	class Node;

	class ResourceInternalObject
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceInternalObject )

	public:
		ResourceInternalObject();
		~ResourceInternalObject();

	public:
		const ConstString & getInternalName() const;
		const ConstString & getInternalGroup() const;

	protected:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;
	
	protected:
		ConstString m_internalName;
		ConstString m_internalGroup;
	};
}