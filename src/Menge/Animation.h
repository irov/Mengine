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
		OBJECT_DECLARE(Animation)
	public:
		//! Конструктор.
		/*!
		*/
		Animation();

	public:
		//! Проиграть анимацию.
		/*!
		*/
		virtual void play();

		//! Остановить анимацию.
		/*!
		*/
		virtual void stop();

		//! Поставить на паузу анимацию.
		/*!
		*/
		virtual void pause();

		//! Установка зацикленности.
		/*!
		\param _loop флаг зацикливания.
		*/
		virtual void setLooped( bool _loop );

		//! Возвращает флаг зацикливания
		/*!
		\return флаг зацикливания
		*/
		virtual bool getLooped() const;

		//! Установка слушателя.
		/*!
		\param _listener слушатель
		*/
		//void setAnimationListener( PyObject * _listener );

		//! Установка ресурса анимации.
		/*!
		\param _resource имя ресурса.
		*/
		void setAnimationResource( const std::string & _resource );

		//! Значение на которое умножается каждая задержка анимации.
		/*!
		\param _factor значение
		*/

		void setAnimationFactor( float _factor );

		//! Получение значения
		/*!
		\return _factor значение
		*/

		float getAnimationFactor() const;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _setListener() override;

	private:
		void play_();

	protected:
		ResourceAnimation * m_resourceAnimation;
		std::string m_resourceAnimationName;
	
		float m_delay;

		bool m_autoStart;
		bool m_randomStart;
		bool m_playing;
		bool m_looping;
		std::size_t m_currentFrame;

		float m_animationFactor;

		bool m_onEndFrameEvent;
		bool m_onEndAnimationEvent;

		//virtual void _onSetListener();
		//PyObject * m_listener;
	};
}