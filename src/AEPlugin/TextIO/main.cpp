#	include "entry.h"

#	include "TextIO.h"

// This entry point is exported through the PiPL (.r file)
extern "C" DllExport AEGP_PluginInitFuncPrototype EntryPointFunc;

//////////////////////////////////////////////////////////////////////////
A_Err EntryPointFunc( struct SPBasicSuite		*pica_basicP,
	A_long				 	major_versionL,
	A_long					minor_versionL,
	AEGP_PluginID			aegp_plugin_id,
	AEGP_GlobalRefcon		*global_refconP )
{
	AEGP_SuiteHandler	suites( pica_basicP );

	Menge::TextIO * io = new Menge::TextIO( pica_basicP, aegp_plugin_id );

	AEIO_ModuleFlags flags =
		AEIO_MFlag_INPUT |
		AEIO_MFlag_OUTPUT |
		AEIO_MFlag_FILE |
		AEIO_MFlag_VIDEO |
		AEIO_MFlag_NO_TIME |
		//AEIO_MFlag_HAS_LAYERS				|
		AEIO_MFlag_INPUT_OPTIONS |
		//AEIO_MFlag_HOST_FRAME_START_DIALOG	|
		//AEIO_MFlag_HAS_AUX_DATA |
		//AEIO_MFlag_HAS_META_DATA |
		//AEIO_MFlag_NO_OPTIONS				|
		AEIO_MFlag_VIDEO;

	if( io->initialize( "Astralax IO files (SDK)", 'PTC_', 2048, 2048, "ptc", flags ) == false )
	{
		return A_Err_GENERIC;
	}

	*global_refconP = (AEGP_GlobalRefcon)io;

	return A_Err_NONE;
}
