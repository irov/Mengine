#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class Animation;

	//! AnimationGroup - анимационная группа. 

    /*! xml - файл имеет следующую структуру:
	*	<Node Name = "имя_ноды" Type = "AnimationGroup">
	*		<Animations>
	*			<Node Name = "имя_ноды0" Type = "Animation">
	*				<ImageMap Name = "имя_ресурса_спрайта"/>
	*				<Animation Name = "имя_ресурса_анимации"/>
	*				<AutoStart Value = "1/0"/>
	*				<Looping Value = "1/0"/>
	*				<Transformation Value = "1;0;0;1;512;0"/>
	*			</Node>
	*			 ...
	*			<Node Name = "имя_нодыN" Type = "Animation">
	*				<ImageMap Name = "имя_ресурса_спрайта"/>
	*				<Animation Name = "имя_ресурса_анимации"/>
	*				<AutoStart Value = "1/0"/>
	*				<Looping Value = "1/0"/>
	*				<Transformation Value = "1;0;0;1;512;0"/>
	*			</Node>
	*		</Animations>
	*	</Node>
	*/

	class AnimationGroup
		: public SceneNode2D
	{
		OBJECT_DECLARE(AnimationGroup)
	public:
		AnimationGroup();

	public:
		//! Проиграть анимацию.
		/*!
			param _name название анимации
		*/
		void play( const std::string & _name );

		//! Остановить анимацию.
		/*!
		*/
		void stop();

		//! Поставить на паузу анимацию.
		/*!
		*/
		void pause();

		//! Установка зацикленности.
		/*!
		\param _loop флаг зацикливания.
		*/
		void setLooped( bool _loop );

		//! Возвращает флаг зацикливания
		/*!
		\return флаг зацикливания
		*/
		bool getLooped() const;

		//! Установка слушателя.
		/*!
		\param _listener слушатель
		*/
		void setAnimationListener( PyObject * _listener );

		//! Установка ресурса анимации.
		/*!
		\param _resource имя ресурса.
		*/
		void setAnimationResource( const std::string & _resource );

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

	private:
		Animation * m_currentAnimation;

		void setCurrentAnimation( const std::string & _name );
	};
}