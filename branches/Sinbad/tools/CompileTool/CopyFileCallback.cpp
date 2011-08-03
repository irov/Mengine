#	include "CopyFileCallback.h"
#	include "CharConvUtils.h"

//////////////////////////////////////////////////////////////////////////
CopyFileCallback::CopyFileCallback()
{
}
//////////////////////////////////////////////////////////////////////////
CopyFileCallback::~CopyFileCallback()
{
}
//////////////////////////////////////////////////////////////////////////
bool CopyFileCallback::execute( const std::string & _src, const std::string & _dst, Compiler * )
{
	Menge::String src = "..\\" + _src;

	BOOL result = CopyFile(CharConvUtils::UTF8ToWChar(src).c_str(), 
		CharConvUtils::UTF8ToWChar(_dst).c_str(), FALSE);

	return result == TRUE;
}
//////////////////////////////////////////////////////////////////////////