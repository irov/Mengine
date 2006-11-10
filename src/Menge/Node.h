#	pragma once

#	include <list>

class TiXmlElement;

namespace luabind
{
	namespace adl
	{
		class object;
	}
}

namespace Menge
{
	class Visitor;

	//pure-virtual interface
	class Node 
	{
	public:
		virtual bool activate() = 0;
		virtual void deactivate() = 0;

		virtual bool isActive() = 0;
		
	public:
		virtual bool compile() = 0;
		virtual void release() = 0;

		virtual bool isCompile() = 0;

	public:
		virtual void setName(const std::string &name) = 0;
		virtual const std::string & getName()const = 0;

		virtual void setType(const std::string &type) = 0;
		virtual const std::string & getType()const = 0;

		virtual size_t getTypeId()const = 0;

		virtual bool isExternal()const = 0;

		virtual void setResource(const std::string &_file) = 0;
		virtual const std::string & getResource()const = 0;

	public:
		virtual void setParent(Node *node) = 0;
		virtual Node * getParent() = 0;

		virtual bool isRoot() = 0;

	public:
		virtual Node * createChildren(const std::string &_name, const std::string &_type) = 0;

		virtual bool addChildren(Node *_node) = 0;
	
		virtual bool isChildren(Node *_node) = 0;
		virtual bool isChildren(const std::string &_name) = 0;

		virtual Node * getChildren(const std::string &_name) = 0;

		typedef void (*TForEachFunc)( Node *);

		virtual void foreachFunc(TForEachFunc _func) = 0;

		virtual void removeChildren(Node *_node) = 0;
		virtual void removeChildren(const std::string &_name) = 0;

	public:
		virtual void update(float _timing) = 0;
		virtual void loader(TiXmlElement *xml) = 0;
		virtual void visit(Visitor *visitor) = 0;

	public:
		virtual void debugRender() = 0;

	public:
		virtual luabind::adl::object * getScriptable() = 0;

	public:
		virtual void _lostChildren(Node *_node, bool _valid) = 0;
	};

}