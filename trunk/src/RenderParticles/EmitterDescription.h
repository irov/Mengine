#	pragma once
#	include	"Utils/Core/Rect.h"
#	include	"Utils/Core/String.h"
#	include "RenderParticlesInterface.h"

namespace Menge
{
	class EmitterDescription
		: public EmitterDescriptionInterface
	{
	public:
		EmitterDescription( );
		void setName( const String& _name );
		void setLeftBorder( float _interval1 );
		void setRightBorder( float _interval2 );
		void setDuration( float _duration ); 
		void setEmitterId( size_t _id );
		void setSize( const Rect & _rect );
		void setRelative( bool _relative );

		float getDuration() const override;
		float getLeftBorder() const override;
		float getRightBorder() const override;
		int getEmitterId() const override;
		const String& getName() const override;
		const Rect& getSize() const override;
		bool getRelative() const override;

	protected:
		String m_name;
		double m_leftBorder;
		double m_rightBorder;
		double m_duration;
		bool m_relative;
		int m_emitterId;
		Rect m_sizes;
	};
}
