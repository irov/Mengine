//#	pragma once
//
//#	include <string>
//
//namespace NError
//{
//	class CError
//	{
//	public:
//		inline CError(const char * File, int Line)
//			:	m_File(File)
//			,	m_Line(Line)
//		{
//		}
//
//	protected:
//		const char * m_File;
//		int m_Line;
//	};
//
//	class CMessageError
//		: public CError
//	{
//	public:
//		CMessageError(const char * File, int Line)
//			: CError(File,Line)
//		{
//		}
//
//	public:
//		void operator() (const char * Message, ...);
//	};
//
//	class CExceptionError
//		: public CError
//	{
//	public:
//		CExceptionError(const char * File, int Line)
//			: CError(File,Line)
//		{
//		}
//
//	public:
//		struct Data
//		{
//			Data(const std::string &Message)
//				: m_Message(Message)
//			{
//			}
//
//			std::string m_Message;
//		};
//
//		void operator() (const char * Message, ...);
//	};
//}
//
//#	define ErrorMessage NError::CMessageError(__FUNCTION__,0)
//
//#	define ErrorException NError::CExceptionError(__FUNCTION__,0)
//
//#	define ErrorTry try{ 
//
//#	define ErrorBeginCatch }catch(const NError::CExceptionError::Data &Data){\
//
//
//#	define ErrorUnknow {throw;}
//
//#	define ErrorEndCatch }catch(...){throw;}