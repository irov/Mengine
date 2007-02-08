#	pragma once

//#	include "MengeExport.h"

#	include "FileEngine.h"

#	include <map>

namespace Menge
{
	class Decoder
	{
	public:
		typedef std::map<std::string, Decoder*> DecoderMap; 
		struct DecoderData
		{
			virtual ~DecoderData(){};
		};
		virtual DecoderData*	decode(FileDataInterface* _filedata) = 0;
		static void	regDecoder(const std::string& _extension, Decoder* _newDecoder);
		static void	unregDecoder(const std::string& _extension);
		static Decoder* getDecodec(const std::string& _extension);
	private:
		static DecoderMap m_codecs;
	};
};