#	pragma once

#	include "Sprite.h"

namespace Menge
{
	class ResourceImageVideoStream;

	/*! xml - файл имеет следующую структуру:
	*	<Node Name = "имя_ноды" Type = "Video">
	*		<ImageMap Name = "имя_ресурса_видео"/>
	*		<AutoStart Value = "1/0"/>
	*	</Node>
	*/

	class Video
		: public Sprite
	{
		OBJECT_DECLARE(Video)
	public:
		//! Конструктор.
		/*!
		*/
		Video();

	public:
		//! Проиграть видео.
		/*!
		*/
		virtual void play();

		//! Остановить видео.
		/*!
		*/
		virtual void stop();

		//! Поставить на паузу видео.
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

		//! Установка ресурса анимации.
		/*!
		\param _resource имя ресурса.
		*/
		void setAnimationResource( const String& _resource );

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
		ResourceImageVideoStream* m_resourceVideo;
		String m_resourceVideoName;

		bool m_autoStart;
		bool m_playing;
		bool m_looping;
	};
}