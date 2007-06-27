#	pragma once

#	include "NodeCore.h"

#	include "Allocator2D.h"
#	include "Renderable.h"

#	include <list>

namespace Menge
{
	class SceneNode2D
		: public NodeCore
		, public Allocator2D
		, public Renderable
	{
	public:
		SceneNode2D();

	public:
		void destroy() override;

	public:
		bool activate() override;
		void deactivate() override;

	public: 
		void setParent( SceneNode2D *node ) ;
		SceneNode2D * getParent();

	public:
		bool addChildren( SceneNode2D *_node );
		void removeChildren( SceneNode2D *_node );

		SceneNode2D * getChildren( const std::string & _name );

		template<class T>
		T * getChildrenT( const std::string & _name )
		{
			return static_cast< T * >( getChildren( _name ) );
		}

		bool isChildren( SceneNode2D *_node );

		SceneNode2D * createChildren( const std::string &_type );

		template< class T >
		T * createChildrenT( const std::string &_type )
		{
			return static_cast<T*>( createChildren( _type ) );
		}

	public:
		const mt::mat3f & getWorldMatrix() override;
		void changePivot() override;

		bool render( const Viewport & _viewPort );

	public:
		void update( float _timing ) override;
		void loader( TiXmlElement * _xml ) override;
		void debugRender() override;

	private:
		SceneNode2D * m_parent;

		typedef std::list< SceneNode2D * > TListSceneNode2D;
		TListSceneNode2D m_listSceneNode2D;
	};
}