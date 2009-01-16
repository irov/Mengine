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
		OBJECT_DECLARE(Light2D)

	public:
	
		Light2D();

	public:
		void loader( XmlElement * _xml ) override;

		void _render( unsigned int _debugMask ) override;
	protected:

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
