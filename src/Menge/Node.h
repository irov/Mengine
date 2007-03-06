#	pragma once

#	include <string>

namespace lua_boost
{
	class lua_functor;
}

class TiXmlElement;

namespace Menge
{
	class Visitor;

	//pure-virtual interface
	class Node 
	{
	public:
		virtual ~Node(){};

	public:
		virtual void setName(const std::string &name) = 0;
		virtual const std::string & getName() const = 0;

		virtual void setType(const std::string &type) = 0;
		virtual const std::string & getType() const = 0;

	public:
		virtual bool activate() = 0;
		virtual void deactivate() = 0;

		virtual bool isActive() = 0;
		
	public:
		virtual bool compile() = 0;
		virtual void release() = 0;

		virtual bool isCompile() = 0;

	public:
		virtual void setResource(const std::string &_file) = 0;
		virtual const std::string & getResource()const = 0;

		virtual bool isExternal()const = 0;

	public:
		virtual void setParent(Node *node) = 0;
		virtual Node * getParent() = 0;

		virtual bool isRoot() = 0;

	public:
		virtual bool addChildren(Node *_node) = 0;
		virtual bool isChildren(Node *_node) = 0;
		virtual void visitChildren( Visitor *_visitor ) = 0;

		virtual Node * nextChildren() = 0;
		virtual Node * beginChildren() = 0;

		virtual void removeChildren(Node *_node) = 0;

	public:
		virtual void registerEvent( const std::string &_name, const lua_boost::lua_functor * _func  ) = 0;
		virtual const lua_boost::lua_functor * event( const std::string &_name ) = 0;

	public:
		virtual void update(float _timing) = 0;
		virtual void loader(TiXmlElement *xml) = 0;
		virtual void visit(Visitor *visitor) = 0;

	public:
		virtual void debugRender() = 0;

	public:
		virtual void _lostChildren(Node *_node, bool _valid) = 0;
	};

}