#	pragma once

#	include "Sprite.h"

namespace Menge
{
	class ResourceAnimation;

	//! Animation - ��������. ����������� �� Sprite, � ��� ������������� ����� ������������� ����������� ����� Sprite::setImageIndex. ��. �������� Sprite.

    /*! xml - ���� ����� ��������� ���������:
	*	<Node Name = "���_����" Type = "Animation">
	*		<ImageMap Name = "���_�������_�������"/>
	*		<Animation Name = "���_�������_��������"/>
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
		//! �����������.
		/*!
		*/
		Animation();

	public:
		//! ��������� ��������.
		/*!
		*/
		virtual void play();

		//! ���������� ��������.
		/*!
		*/
		virtual void stop();

		//! ��������� �� ����� ��������.
		/*!
		*/
		virtual void pause();

		//! ��������� �������������.
		/*!
		\param _loop ���� ������������.
		*/
		virtual void setLooped( bool _loop );

		//! ���������� ���� ������������
		/*!
		\return ���� ������������
		*/
		virtual bool getLooped() const;

		//! ��������� ���������.
		/*!
		\param _listener ���������
		*/
		//void setAnimationListener( PyObject * _listener );

		//! ��������� ������� ��������.
		/*!
		\param _resource ��� �������.
		*/
		void setAnimationResource( const std::string & _resource );

		//! �������� �� ������� ���������� ������ �������� ��������.
		/*!
		\param _factor ��������
		*/

		void setAnimationFactor( float _factor );

		//! ��������� ��������
		/*!
		\return _factor ��������
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

	private:
		void play_();

	protected:
		ResourceAnimation * m_resourceAnimation;
		std::string m_resourceAnimationName;
	
		float m_delay;

		bool m_autoStart;
		bool m_playing;
		bool m_looping;
		unsigned int m_currentFrame;

		float m_animationFactor;
		virtual void _onSetListener();
		//PyObject * m_listener;
	};
}