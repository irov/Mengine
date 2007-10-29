#	pragma once

#	include <string>

class TiXmlElement;

namespace Menge
{	
	class NodeForeach;
	class Scriptable;
	class Viewport;

	//! Node - base pure virtual class
	/*! 
	* 
	*/
	class Node 
	{
	public:
		//! Node virtual destructor.
		/*!
		*/
		virtual ~Node(){};

		//! Node pure virtual. need call this, if you want delete this object
		/*!
		*/
		virtual void destroy() = 0;

	public:
		//! Node pure virtual. set node name
		/*!
		\param _name the first argument.
		*/
		virtual void setName(const std::string & _name) = 0;

		//! Node pure virtual. get node name
		/*!
		\return name
		*/
		virtual const std::string & getName() const = 0;

		//! Node pure virtual. set node type
		/*!
		\param type the first argument.
		*/
		virtual void setType( const std::string & _type ) = 0;

		//! Node pure virtual. get node type
		/*!
		\return type
		*/
		virtual const std::string & getType() const = 0;

	public:

		//! Node pure virtual. activate node, setup resource for work
		/*!
		*/
		virtual bool activate() = 0;

		//! Node pure virtual. deactivate node, remove resource for work
		/*!
		*/
		virtual void deactivate() = 0;

		//! Node pure virtual. check activate node
		/*!
		\return result of activate
		*/
		virtual bool isActive() = 0;

	public:
		virtual bool compile() = 0;

		virtual void release() = 0;

		virtual bool isCompile() = 0;

	public:

		//! Node pure virtual. set parent node
		/*!
		\param _node the first argument, parent node.
		*/
		virtual void setParent( Node * _node ) = 0;

		//! Node pure virtual. add children
		/*!
		\param _node the first argument, children node.
		*/
		virtual bool addChildren( Node * _node ) = 0;

		//! Node pure virtual. remove children
		/*!
		\param _node the first argument, children node.
		*/
		virtual void removeChildren( Node * _node ) = 0;

		virtual Node * getChildren( const std::string & _name, bool _recursion ) = 0;

		//! Node pure virtual. check is children
		/*!
		\param _node the first argument, pointer of children
		\return is children?
		*/
		virtual bool isChildren( Node * _node ) = 0;

		//! Node pure virtual. apply functor for all children in node
		/*!
		\param _foreach the first argument, functor NodeForeach
		*/
		virtual void foreachChildren( NodeForeach & _foreach ) = 0;

	public:
		virtual Scriptable * getScriptable() = 0; 

	public:
		//! Node pure virtual. update node
		/*!
		\param _timing the first argument, timing for need update
		*/
		virtual void update( size_t _timing, const Viewport & _viewport ) = 0;

		//! Node pure virtual. setup node from xml
		/*!
		\param xml the first argument, xml element [TiXmlElement]
		*/
		virtual void loader( TiXmlElement * xml ) = 0;

	public:

		//! Node pure virtual. debug render this node
		/*!
		*/
		virtual void debugRender() = 0;
	};
}
