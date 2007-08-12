#	pragma once

#	include "SceneNode2D.h"

#	include "ResourceMessageStorage.h"

#	include <list>

#	include <string>

class RenderFontInterface;
class FileDataInterface;

namespace	Menge
{
	class ResourceMessageStorage;

	class TextField
		: public SceneNode2D
	{
		OBJECT_DECLARE(TextField);
	public:
		TextField();

	protected:
		void	_update(float _timing) override;
		
		bool	_activate() override;
		void	_deactivate() override;

	protected:
		void	_render( const mt::mat3f &rwm, const Viewport & _viewPort ) override;
		void	_debugRender() override;

	public:
		void	loader(TiXmlElement * _xml) override;

	public:
		virtual const Message * start();
		virtual const Message * getNextMessage();
		bool	isPlaying() const;

	private:
		std::string m_fontFilename;
		std::string m_resourceName;

		typedef std::list<int> TListMessage;
		TListMessage m_messageSpots;
		TListMessage::iterator m_currentMessageSpot;

		RenderFontInterface * m_dialogFont;
		ResourceMessageStorage * m_resourceMessageStore;

		float			m_total_delay;
		float			m_delay;

		typedef std::list<std::string>	TListLine;
		TListLine		m_renderLines;

		void	_createFormattedMessage(const std::string& _text, RenderFontInterface* _font, float _width);

	protected:
		bool	m_isPlaying;
	};
}