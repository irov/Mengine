#	pragma once

#	include "NodeSinglethon.h"

#	include "Renderable2D.h"

#	include "ResourceMessageStorage.h"

#	include <list>

#	include <string>

class RenderFontInterface;
class FileDataInterface;

namespace	Menge
{
	class ResourceMessageStorage;

	class TextField
		: public NodeSinglethon, Renderable2D
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
		void	start();
	private:
		ResourceMessageStorage * m_resourceMessageStore;

		std::string m_fontFilename;
		std::string m_resourceName;

		typedef std::list<int> TListMessage;
		typedef std::list<std::string>	TListLine;

		TListLine m_lines;

		TListMessage m_messageSpots;
		TListMessage::iterator m_currentMessageSpot;

		RenderFontInterface * m_dialogFont;

		bool	m_isPaused;
		bool	m_nextDialog;

		void	createFormattedMessage(const std::string& _text, RenderFontInterface* _font, float _width);
	};
}