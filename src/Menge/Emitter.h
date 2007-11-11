#	pragma once

#	include "SceneNode2D.h"

#	include "math/mat3.h"

#	include <vector>

class RenderImageInterface;

namespace Menge
{
	class ResourceImage;

	class Emitter
		: public SceneNode2D
	{
		OBJECT_DECLARE(Emitter);
	public:
		Emitter();
		~Emitter();

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
	};
}