//#	pragma once
//
//#	include "Sprite.h"
//
//namespace Menge
//{
//	class SpriteProxy
//		: public SceneNode2D
//	{
//	public:
//		SpriteProxy(Sprite * _spr);
//		~SpriteProxy();
//
//	public:
//		void setImageIndex( size_t _index );
//		size_t getImageIndex() const;
//
//		void setImageResource( const std::string & _name );
//		const std::string & getImageResource() const;
//
//	public:
//		bool isVisible( const Viewport & _viewPort );
//
//		void loader( TiXmlElement * _xml ) override;
//
//	protected:
//		bool _activate() override;
//		void _deactivate() override;
//
//		void _render( const mt::mat3f &rwm, const Viewport & _viewPort ) override;
//		void _debugRender() override;
//
//	private:
//		Sprite* m_sprite;
//	};
//}