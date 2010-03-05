#	pragma once

#	include "Node.h"

#	include "FixedVertices.h"

namespace Menge
{
	class ResourceVideo;
	//class ResourceSound;
	class SoundEmitter;
	class SoundSourceInterface;
	class Texture;

	/*! xml - файл имеет следующую структуру:
	*	<Node Name = "имя_ноды" Type = "Video">
	*		<ImageMap Name = "имя_ресурса_видео"/>
	*		<AutoStart Value = "1/0"/>
	*	</Node>
	*/

	class Video
		: public Node
		, public QuadVertices
	{
	public:
		Video();

	public:
		virtual void play();
		virtual void stop();
		virtual void pause();

		virtual void setLooped( bool _loop );
		virtual bool getLooped() const;

	public:
		void setVideoResource( const String& _resource );
		const String & getVideoResource() const;

		void setSoundResource( const String& _resource );
		const String & getSoundResource() const;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;
		void _render( Camera2D * _camera ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;
		void _setListener() override;
		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;

	private:
		void play_();

	protected:
		void _updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices ) override;

	protected:
		ResourceVideo* m_resourceVideo;

		String m_resourceVideoName;
		String m_resourceSoundName;

		Texture* m_resourceImage;
		SoundEmitter* m_soundEmitter;

		Material* m_material;	

		bool m_autoStart;
		bool m_playing;
		bool m_looping;
		float m_timing;
		bool m_needUpdate;
	};
}
