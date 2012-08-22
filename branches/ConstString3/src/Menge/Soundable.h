#	pragma once

namespace Menge
{
	class Soundable
	{
	public:
		void setVolume( float _volume );
		float getVolume() const;

	protected:
		virtual void _setVolume( float _volume );
		virtual float _getVolume() const;
	};
}