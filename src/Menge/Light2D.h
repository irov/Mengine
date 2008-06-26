#	pragma once
#	include "math\vec2.h"
#	include "math\polygon.h"
#	include "ShadowCaster2D.h"

#	include "SceneNode2D.h"

namespace Menge
{
	class Light2D
		: public SceneNode2D
	{
		OBJECT_DECLARE(Light2D)

	public:
	
		Light2D();

	public:
		virtual bool isVisible( const Viewport & _viewPort );

		void loader( XmlElement * _xml ) override;

	protected:
		void _render( bool _enableDebug ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

	public:
		void setShadowLength(float _length);
		void setPenumbraRadius(float _radius);

		float getShadowLength() const;
		float getPenumbraRadius() const;

	private:
		float m_shadowLength;
		float m_shadowPenumbraR;
	};
};