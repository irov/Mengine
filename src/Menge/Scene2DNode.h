# pragma once

#	include "Eventable.h"

#	include "math/mat3.h"

#	include <list>

namespace lua_boost
{
	class lua_functor;
}

class TiXmlElement;

namespace Menge
{	
	class Visitor;
	class Scene2D;

	class Scene2DNode
		: public Eventable
	{
	public:
		typedef std::list<Scene2DNode *> TListChildren;

	public:
		Scene2DNode( Scene2D * _scene  );
		virtual ~Scene2DNode();

	public:
		virtual bool activate();
		virtual void deactivate();

		virtual bool isActive();

	public:
		virtual bool compile();
		virtual void release();

		virtual bool isCompile();

	public:
		virtual void setName(const std::string & _name);
		virtual const std::string & getName()const;

		virtual void setType(const std::string & _type);
		virtual const std::string & getType()const;

		virtual bool isExternal()const;

		virtual void setResource(const std::string &_file);
		virtual const std::string & getResource()const;

	public:
		virtual void setParent(Scene2DNode *node);
		virtual Scene2DNode * getParent();

		template<class T>
		T * getParentT()
		{
			return static_cast<T*>(getParent());
		}

		virtual bool isRoot();

	public:
		virtual Scene2DNode * createChildren(const std::string &_name, const std::string &_type);

		template<class T>
		T * createChildrenT(const std::string &_name, const std::string &_type)
		{
			return static_cast<T*>(createChildren(_name,_type));
		}

		virtual bool addChildren(Scene2DNode *_node);

		virtual bool isChildren(Scene2DNode *_node);
		virtual bool isChildren(const std::string &_name);

		virtual Scene2DNode * getChildren(const std::string &_name);

		template<class T>
		T * getChildrenT(const std::string &_name)
		{
			return static_cast<T*>(getChildren(_name));
		}

		virtual Scene2DNode * nextChildren();
		virtual Scene2DNode * beginChildren();

		virtual void visitChildren( Visitor *_visitor );

		virtual void removeChildren(Scene2DNode *_node);
		virtual void removeChildren(const std::string &_name);

	public:
		virtual void update(float _timing);
		virtual void loader(TiXmlElement *xml);

	public:
		virtual void debugRender();

	public:
		virtual void registerEvent( const std::string &_name, const lua_boost::lua_functor * _func  );
		virtual const lua_boost::lua_functor * event( const std::string &_name );

	protected:

		virtual void _update( float _timing );

		virtual bool _activate();
		virtual void _deactivate();

		virtual bool _compile();
		virtual void _release();

		virtual void _debugRender();

		virtual void _updateParent();
		virtual void _lostChildren(Scene2DNode *_node, bool _valid);

		virtual void _visit( Visitor * _visitor );

	protected:
		bool m_active;
		bool m_compile;

		bool m_childrenForeach;

		std::string m_name;
		std::string m_type;

		bool m_external;
		std::string m_resource;

		Scene2D * m_scene;

		Scene2DNode * m_parent;
		TListChildren m_listChildren;
		TListChildren::iterator m_iteratorChildren;

	private:
		TListChildren::iterator _findChildren(const std::string &_name);


	/******************************************************
	*	Description: Allocator
	*
	*
	******************************************************/

	public:
		virtual const mt::vec2f & getWorldPosition();
		virtual const mt::vec2f & getWorldDirection();
		virtual const mt::mat3f & getWorldMatrix();

		const mt::vec2f & getLocalPosition()const;
		mt::vec2f & getLocalPosition();

		const mt::vec2f & getLocalDirection()const;
		mt::vec2f & getLocalDirection();

		const mt::mat3f & getLocalMatrix()const;
		mt::mat3f & getLocalMatrix();

		void setPosition( const mt::vec2f &position );
		void setDirection( const mt::vec2f &direction );

		void translate( const mt::vec2f &delta );

	public:
		void changePivot();
		bool isChangePivot()const;

	protected:
		virtual void _changePivot();
		virtual bool _updateMatrix();

	protected:
		mt::mat3f m_localMatrix;
		mt::mat3f m_worldMatrix;

	private:
		bool m_changePivot;

	private:
		void updateMatrix();
	};
}