
// Precompiler options
#include "D3D7Prerequisites.h"

namespace Menge 
{
	// Encapsulates enumeration of DirectDraw drivers. Utility class for D3DRenderSystem.
    class DDDriverList
    {
    public:
        DDDriverList( LogSystemInterface* _logInterface );
        ~DDDriverList(); // Default desctructor
        BOOL enumerate(); //Forces an enumeration / re-enumeration

		// Returns the item at the specified position. Throws an
        // std::out_of_range if the index is greater then the size.
		DDDriver* item( std::size_t _index );

		std::size_t count() const;

        // Non-member callbacks
        friend static BOOL WINAPI DDEnumCallbackEx(GUID FAR *lpGUID,
                        LPSTR     lpDriverDescription,
                        LPSTR     lpDriverName,
                        LPVOID    lpContext,
                        HMONITOR  hm);


        friend static BOOL WINAPI DDEnumCallback(GUID FAR *lpGUID,
                                   LPSTR  lpDriverDescription,
                                   LPSTR  lpDriverName,
                                   LPVOID lpContext);

	private:
		// STL list of drivers (Vector used to allow random access)
		std::vector<DDDriver> m_driverList;
		LogSystemInterface*	m_logInterface;

		void addDriver(GUID FAR *lpGuid,
			LPSTR lpDriverDesc,
			LPSTR lpDriverName);

    };
}	// namespace Menge
