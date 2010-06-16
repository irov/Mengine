#	pragma once

#	include "math/vec2.h"
#	include "math/polygon.h"
#	include "ShadowCaster2D.h"

#	include "Node.h"

namespace Menge
{
	class Light2D
		: public Node
	{
	public:
		Light2D();

	public:
		void setShadowLength(float _length);
		void setPenumbraRadius(float _radius);

		float getShadowLength() const;
		float getPenumbraRadius() const;

	public:
		void loader( XmlElement * _xml ) override;
		void parser( BinParser * _parser ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		void _render( Camera2D * _camera ) override;

	private:
		float m_shadowLength;
		float m_shadowPenumbraR;
	};
};
