#	pragma once

#	include "Node.h"

namespace Menge
{
	class ResourceVideo;
	//class ResourceSound;
	class SoundEmitter;
	class SoundSourceInterface;
	class ResourceImage;

	/*! xml - ���� ����� ��������� ���������:
	*	<Node Name = "���_����" Type = "Video">
	*		<ImageMap Name = "���_�������_�����"/>
	*		<AutoStart Value = "1/0"/>
	*	</Node>
	*/

	class Video
		: public Node
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
		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;

	private:
		void play_();

	protected:
		ResourceVideo* m_resourceVideo;

		String m_resourceVideoName;
		String m_resourceSoundName;

		ResourceImage* m_resourceImage;
		SoundEmitter* m_soundEmitter;

		RenderObject* m_renderObject;

		mt::vec2f m_size;
		void updateVertices_();

		bool m_autoStart;
		bool m_playing;
		bool m_looping;
		float m_timing;
		bool m_needUpdate;
	};
}
