#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class Animation;

	class AnimationGroup
		: public SceneNode2D
	{
		OBJECT_DECLARE(AnimationGroup)
	public:
		AnimationGroup();

	public:
		void play( const std::string & _name );
		void pause( const std::string & _name );
		void stop( const std::string & _name );

		void setLooped( const std::string & _name, bool _looped );
		bool getLooped( const std::string & _name );

		void setAnimationListener( const std::string & _name, PyObject * _listener );
		void setAnimationResource( const std::string & _name, const std::string & _resource );

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

	private:

		Animation * m_currentAnimation;

		void setCurrentAnimation( const std::string & _name );
	};
}