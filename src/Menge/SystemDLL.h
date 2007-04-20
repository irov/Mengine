#	pragma once

#	include <string>

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <assert.h>

namespace Menge
{
	template<class T>
	class SystemDLL
	{
	public:
		typedef bool (*FInterfaceInitial)(T**);
		typedef void (*FInterfaceRelease)(T*);

		SystemDLL(const std::string &_filename)
		{
			m_hInst = LoadLibrary(_filename.c_str());

			if(m_hInst == NULL)
			{
				assert(!"DLL load failure!");
				if(FreeLibrary(m_hInst) == 0)
				{
					assert(!"FreeLibrary failure!");
				}
			}

			m_fInitFunction = (FInterfaceInitial)GetProcAddress((HMODULE)m_hInst, "initInterfaceSystem" );

			if(m_fInitFunction == NULL)
			{
				assert(!"Init Function failure!");
			}

			m_fReleaseFunction = (FInterfaceRelease)GetProcAddress((HMODULE)m_hInst, "releaseInterfaceSystem");

			if(m_fReleaseFunction == NULL)
			{
				assert(!"Release Function failure!");
			}

			m_fInitFunction(&m_interface);
		}

		~SystemDLL()
		{
			m_fReleaseFunction(m_interface);
			if(FreeLibrary(m_hInst) == 0)
			{
				assert(!"FreeLibrary failure!");
			}
		}

	protected:
		T * m_interface;

	private:
		HMODULE m_hInst;
		FInterfaceRelease m_fReleaseFunction;
		FInterfaceInitial m_fInitFunction;
	};
}