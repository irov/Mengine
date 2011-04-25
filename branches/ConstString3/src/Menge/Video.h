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

	struct MaterialGroup;

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

		virtual void setLoop( bool _loop );
		virtual bool getLoop() const;

	public:
		void setVideoResource( const ConstString& _resource );
		const ConstString & getVideoResource() const;

		void setSoundResource( const ConstString& _resource );
		const ConstString & getSoundResource() const;

	public:
		void loader( BinParser * _parser ) override;

	protected:
		void _update( float _timing ) override;
		void _render( Camera2D * _camera ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;
		void _setEventListener( PyObject * _listener ) override;
		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;

	private:
		void play_();

	protected:
		void _updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices ) override;

	protected:
		ResourceVideo* m_resourceVideo;

		ConstString m_resourceVideoName;
		ConstString m_resourceSoundName;

		const Texture * m_textures[1];
		SoundEmitter* m_soundEmitter;

		const MaterialGroup * m_materialGroup;
		const Material * m_material;	

		bool m_autoStart;
		bool m_playing;
		bool m_loop;
		float m_timing;
		bool m_needUpdate;
	};
}
