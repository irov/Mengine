#	pragma once

#	include "Sprite.h"

namespace Menge
{
	class ResourceAnimation;

	//! Animation - анимация. Наследуется от Sprite, и для прогрываемого кадра устанавливает изображение через Sprite::setImageIndex. см. описание Sprite.

    /*! xml - файл имеет следующую структуру:
	*	<Node Name = "имя_ноды" Type = "Animation">
	*		<ImageMap Name = "имя_ресурса_спрайта"/>
	*		<Animation Name = "имя_ресурса_анимации"/>
	*		<AutoStart Value = "1/0"/>
	*		<Looping Value = "1/0"/>
	*		<Transformation Value = "1;0;0;1;512;0"/>
	*	</Node>
	*/

	class Animation
		: public Sprite
	{
	public:
		Animation();

	public:
		virtual void play();
		virtual void stop();
		virtual void pause();
		virtual void resume();

		virtual void setLooped( bool _loop );
		virtual bool getLooped() const;

		void setAnimationResource( const String& _resource );
		void setAnimationFactor( float _factor );
		float getAnimationFactor() const;

		std::size_t getCurrentFrame() const;
		std::size_t getFrameCount() const;
		void setCurrentFrame( std::size_t _frame );

	public:
		void loader( XmlElement * _xml ) override;
		void parser( BinParser * _parser ) override;

	protected:
		void _update( float _timing ) override;

		bool _activate() override;

		bool _compile() override;
		void _release() override;

		void _setListener() override;

	private:
		void stop_();
		void resume_();

	protected:
		ResourceAnimation * m_resourceAnimation;
		String m_resourceAnimationName;
	
		float m_delay;

		bool m_autoStart;
		bool m_randomStart;
		bool m_playing;
		bool m_looping;
		std::size_t m_currentFrame;

		float m_animationFactor;

		bool m_onEndFrameEvent;
		bool m_onEndFrameTick;
		bool m_onEndAnimationEvent;
	};
}
