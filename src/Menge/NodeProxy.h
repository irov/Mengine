#	pragma once

#	include "Node.h"

namespace Menge
{
	class NodeProxy
		: public Node
	{
	public:
		NodeProxy( Node *_node);
		virtual ~NodeProxy();

	public:
		virtual void destroy() override;

	public:
		bool activate() override;
		void deactivate() override;

		bool isActive() override;

	public:
		void setName(const std::string &name) override;
		const std::string & getName()const override;

		void setType(const std::string &type) override;
		const std::string & getType()const override;

	public:
		void update(float _timing) override;
		void loader(TiXmlElement *xml) override;

	public:
		void debugRender() override;

	public:
		virtual void registerEvent( const std::string &_name, PyObject * _func  );
		virtual PyObject * getEvent( const std::string &_name );

		virtual	void setScriptable( PyObject * _scriptable );
		virtual PyObject * getScriptable();
		virtual bool isScriptable() const;

	protected:
		Node *m_node;
	};
}