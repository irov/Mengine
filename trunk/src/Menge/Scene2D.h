#	pragma once

#	include "Scene.h"
#	include "Allocator2D.h"

namespace Menge
{

	class Scene2D 
		: public Scene
		, public Allocator2D
	{
		OBJECT_DECLARE(Scene2D);

		public:
			void loader( XmlElement *_xml) override;

		public:
			bool handleKeyEvent( size_t _key, size_t _char, bool _isDown ) override;
			bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
			bool handleMouseMove( int _x, int _y, int _whell ) override;

		protected:
			bool _activate() override;
			void _update( float _timing ) override;

			void _addChildren( Layer * _layer ) override;
	};

} // namespace Menge