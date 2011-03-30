#	pragma once

#	include "Node.h"

namespace Menge
{
	class ResourceModel;

	class Model
		: public Node
	{
	public:
		Model();
		~Model();

	public:
		void setModelResource( const ConstString & _resourceName );
		const ConstString & getModelResource() const;

	public:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		ConstString m_resourceName;
		ResourceModel * m_resourceModel;
	};
}