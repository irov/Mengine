#	pragma once

#	include "Holder.h"

#	include "InputHandler.h"

#	include "Math/vec3.h"

#	include <string>


namespace Menge
{	
	class Scene;
	class Avatar;
	class Arrow;

	class Camera2D;
	class Camera3D;

	class Player
		: public InputHandler
	{
	public:
		Player();
		~Player();

	public:
		void setCurrentScene( const std::string & _scene );
		Scene * getCurrentScene();

		void setArrow( Arrow * _arrow );
		Arrow * getArrow();

		const mt::vec2f & getPositionClick() const;

	public:
		void setRenderCamera2D( Camera2D * _camera);
		Camera2D * getRenderCamera2D();

		void setRenderCamera3D( Camera3D * _camera);
		Camera3D * getRenderCamera3D();

		void setCamera2DPosition( const mt::vec2f & _pos );
		void setCamera3DPosition( const mt::vec3f & _pos );

	public:
		void init();
		void update( size_t _timing );
		void render();

		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;

		void debugRender();
	
	private:
		Avatar * m_avatar;
		Scene * m_scene;
		Arrow * m_arrow;
		
		Camera2D * m_renderCamera2D;
		Camera3D * m_renderCamera3D;

		bool m_switchScene;
		Scene * m_nextScene;
	};
}