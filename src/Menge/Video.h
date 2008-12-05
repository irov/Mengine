#	pragma once

#	include "Sprite.h"

namespace Menge
{
	class ResourceVideo;
	//class ResourceSound;
	class SoundEmitter;
	class SoundSourceInterface;

	/*! xml - ���� ����� ��������� ���������:
	*	<Node Name = "���_����" Type = "Video">
	*		<ImageMap Name = "���_�������_�����"/>
	*		<AutoStart Value = "1/0"/>
	*	</Node>
	*/

	class Video
		: public Sprite
	{
		OBJECT_DECLARE(Video)
	public:
		//! �����������.
		/*!
		*/
		Video();

	public:
		//! ��������� �����.
		/*!
		*/
		virtual void play();

		//! ���������� �����.
		/*!
		*/
		virtual void stop();

		//! ��������� �� ����� �����.
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

		//! ��������� ������� ��������.
		/*!
		\param _resource ��� �������.
		*/
		void setAnimationResource( const String& _resource );

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;
		void _render( unsigned int _debugMask ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;
		void _setListener() override;

	private:
		void play_();

	protected:
		ResourceVideo* m_resourceVideo;

		String m_resourceVideoName;
		String m_resourceSoundName;

		RenderImageInterface* m_renderImage;
		SoundEmitter* m_soundEmitter;

		bool m_autoStart;
		bool m_playing;
		bool m_looping;
		float m_timing;
		bool m_needUpdate;
	};
}