#	pragma once

#	include "SceneNode2D.h"

#	include "math/mat3.h"

#	include <vector>

class RenderImageInterface;
class EmitterInterface;

namespace Menge
{
	class ResourceEmitterContainer;

	class Emitter
		: public SceneNode2D
	{
		OBJECT_DECLARE(Emitter);
	public:
		Emitter();
		~Emitter();

	public:
		void play( bool _leftVisible );
		void stop();
		bool isPlaying() const;
	
	public:
		virtual bool isVisible( const Viewport & _viewPort );

		void loader( TiXmlElement * _xml ) override;

	protected:
		void _render() override;
		void _debugRender() override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

	private:
		std::string m_resourceName;
		std::string m_emitterName;

		EmitterInterface * m_interface;

		ResourceEmitterContainer * m_resourceEmitter;

		RenderImageInterface * m_image;
	};
}