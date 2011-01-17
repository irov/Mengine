#	pragma once

#	include "Node.h"

namespace Menge
{
	class Entity
		: public Node
	{
	public:
		void setPrototype( const ConstString & _prototype );
		const ConstString & getPrototype() const;

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;

	protected:
		void _embedding( PyObject * _embed ) override;

	public:
		void loader( BinParser * _parser ) override;

	protected:
		void _loaded() override;

	protected:
		ConstString m_prototype;
	};
}