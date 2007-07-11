//#	pragma once
//
////#	include "MengeExport.h"
//
//#	include "Interface/CodecInterface.h"
//
//#	include "FileEngine.h"
//#	include "SystemDLL.h"
//
//#	include <map>
//
//namespace Menge
//{
//	class Decoder
//	{
//	public:
//		struct DecoderComponent : public SystemDLL<DecoderInterface>
//		{
//			DecoderComponent(const std::string& _dllModule)
//				: SystemDLL<DecoderInterface>(_dllModule)
//			{}
//			DecoderInterface* getDecoder(){return m_interface;};
//		};
//
//		typedef std::map<std::string, DecoderComponent*> DecoderMap; 
//
//		struct DecoderData
//		{
//			virtual ~DecoderData(){};
//		};
//		static void	regDecoder(const std::string& _extension, DecoderComponent* _newDecoder);
//		static void	unregDecoder(const std::string& _extension);
//		static DecoderInterface* getDecodec(const std::string& _extension);
//	private:
//		static DecoderMap m_codecs;
//	};
//}