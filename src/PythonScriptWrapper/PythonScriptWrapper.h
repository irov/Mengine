#pragma once

namespace Mengine
{
	class PythonScriptWrapper
	{
	public:	
        static void constsWrap();
		static void helperWrap();
		static void mathWrap();
		static void nodeWrap();
        static void entityWrap();
        static void engineWrap();        
		static void soundWrap();
	};
}
