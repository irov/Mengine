#	pragma once

#	include "NodeSinglethon.h"

#	include <vector>

#	include <string>

class RenderFontInterface;
class SoundSourceInterface;
class FileDataInterface;

namespace	Menge
{
	class MessageSpot;

	typedef std::vector<MessageSpot*> TVecMessageSpot;

	typedef std::list<std::string>	TListLine;

	class Dialog
		: public NodeSinglethon
	{
		OBJECT_DECLARE(Dialog);
	public:
		Dialog();

	protected:
		void	_update(float _timing) override;
		
		bool	_activate() override;
		void	_deactivate() override;

	public:
		void	loader(TiXmlElement * _xml) override;
	public:
		void	start();
		void	nextMessageSpot();
	private:
		void	createFormattedMessage(const std::string& _text, RenderFontInterface* _font, float _width);
		void	loadCurrentMessageSpot();
	private:
		std::string m_fontFilename;

		TListLine m_lines;

		TVecMessageSpot m_messageSpots;
		TVecMessageSpot::iterator m_currentMessageSpot;

		RenderFontInterface* m_dialogFont;
		SoundSourceInterface* m_soundSource;
		FileDataInterface* m_fileData;
		bool m_isUpdate;
	};
};