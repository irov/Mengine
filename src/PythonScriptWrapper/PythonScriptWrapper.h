#pragma once

namespace Mengine
{
	class PythonScriptWrapper
	{
	public:
		static void entityWrap();
		static void helperWrap();
		static void mathWrap();
		static void nodeWrap();
        static void constsWrap();
		static void soundWrap();
	};
}
