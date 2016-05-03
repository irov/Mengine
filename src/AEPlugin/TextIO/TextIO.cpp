#	include "TextIO.h"

#	include "TextOptions.h"

//#	include "AstralaxOptions.h"

//#	include "AstralaxRender.h"

//#	include "Emitter.h"

#	include <stdio.h>
#	include <stdlib.h>

#	include <fstream>

SERVICE_PROVIDER_EXTERN( ServiceProvider );

SERVICE_EXTERN( OptionsSystem );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( RenderSystem );
SERVICE_EXTERN( RenderService );
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( UnicodeService );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( ConfigService );

SERVICE_EXTERN( WindowsLayer );

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextIO::TextIO( struct SPBasicSuite * pica_basicP, AEGP_PluginID aegp_plugin_id )
		: AEIO( pica_basicP, aegp_plugin_id )
		, m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextIO::~TextIO()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextIO::initialize( const char * _name
		, AEIO_FileType _sig
		, A_long _width
		, A_long _height
		, const char _ext[3]
		, AEIO_ModuleFlags _flag
		)
	{
		if( AEIO::initialize( _name, _sig, _width, _height, _ext, _flag ) == false )
		{
			return false;
		}



		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextIO::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_AEGP_GetInSpecFilePath( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, std::wstring & _path )
	{
		AEGP_SuiteHandler suites( basic_dataP->pica_basicP );

		AEGP_MemHandle unicode_pathPH;

		suites.IOInSuite4()->AEGP_GetInSpecFilePath( specH
			, &unicode_pathPH );

		A_UTF16Char * unicode_path;
		suites.MemorySuite1()->AEGP_LockMemHandle( unicode_pathPH, (void**)&unicode_path );

		_path = (const wchar_t *)unicode_path;

		suites.MemorySuite1()->AEGP_UnlockMemHandle( unicode_pathPH );

		suites.MemorySuite1()->AEGP_FreeMemHandle( unicode_pathPH );

		return true;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err TextIO::IO_InitInSpecFromFile(
		AEIO_BasicData		*basic_dataP,
		const A_UTF16Char	*file_pathZ,
		AEIO_InSpecH		specH )
	{
		/*	Read the file referenced by the path. Use the
		file information to set all fields of the AEIO_InSpecH.
		*/
		AEIO_Handle optionsH = NULL;
		suites.MemorySuite1()->AEGP_NewMemHandle( basic_dataP->aegp_plug_id,
			"TextOptionsOptions",
			sizeof( Menge::TextOptions ),
			AEGP_MemFlag_CLEAR,
			&optionsH );

		Menge::TextOptions * headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void **>(&headerP) );

		if( headerP == NULL )
		{
			return A_Err_GENERIC;
		}

		bool successful = true;

		std::wstring ptc = (const wchar_t *)file_pathZ;
		//successful = s_InitInSpecFromFile( headerP, m_render, ptc );

		if( successful == true )
		{
			AEIO_Handle old_optionsH = NULL;
			suites.IOInSuite4()->AEGP_SetInSpecOptionsHandle( specH,
				optionsH,
				reinterpret_cast<void **>(&old_optionsH) );

			suites.IOInSuite4()->AEGP_SetInSpecDepth( specH, 32 );

			suites.IOInSuite4()->AEGP_SetInSpecDuration( specH, &headerP->duration );
			suites.IOInSuite4()->AEGP_SetInSpecDimensions( specH, headerP->width, headerP->height );

			A_Fixed	native_fps = FLOAT2FIX( 29.97 );
			suites.IOInSuite4()->AEGP_SetInSpecNativeFPS( specH, native_fps );
		}

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );

		if( successful == false )
		{
			return A_Err_GENERIC;
		}

		return A_Err_NONE;
	}
	//////////////////////////////////////////////////////////////////////////
	A_Err TextIO::IO_DisposeInSpec( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH )
	{
		AEIO_Handle	optionsH = NULL;
		suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle( specH, reinterpret_cast<void**>(&optionsH) );

		if( optionsH == NULL )
		{
			return A_Err_NONE;
		}

		Menge::TextOptions * old_headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void**>(&old_headerP) );

		//if( old_headerP->emitter != nullptr )
		//{
		//	m_render->destroyEmitter( old_headerP->emitter );
		//}

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );
		suites.MemorySuite1()->AEGP_FreeMemHandle( optionsH );

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err TextIO::IO_FlattenOptions( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, AEIO_Handle * flat_optionsPH )
	{
		AEIO_Handle optionsH = NULL;
		suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle( specH, reinterpret_cast<void**>(&optionsH) );

		Menge::TextOptions * headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void**>(&headerP) );


		suites.MemorySuite1()->AEGP_NewMemHandle( basic_dataP->aegp_plug_id,
			"TextFlatten",
			TEXT_IO_FLATTEN_OPTIONS_SIZE,
			AEGP_MemFlag_CLEAR,
			flat_optionsPH );

		unsigned char * flat_headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( *flat_optionsPH, (void**)&flat_headerP );

		if( headerP->writeFlatten( flat_headerP ) == false )
		{
			//logger.message( "AstralaxIO::IO_FlattenOptions headerP invalid writeFlatten"
			//	);
		}

		//std::wstring ptcPath;
		//s_AEGP_GetInSpecFilePath( basic_dataP, specH, ptcPath );

		//if( headerP->emitter != nullptr )
		//{
		//	m_render->destroyEmitter( headerP->emitter );

		//	headerP->emitter = nullptr;
		//}

		suites.MemorySuite1()->AEGP_UnlockMemHandle( *flat_optionsPH );

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err TextIO::IO_InflateOptions( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, AEIO_Handle flat_optionsH )
	{
		/*	During this function,

		+	Create a non-flat options structure, containing the info from the
		flat_optionsH you're passed.

		+	Set the options for the InSpecH using AEGP_SetInSpecOptionsHandle().
		*/

		if( flat_optionsH == nullptr )
		{
			return A_Err_NONE;
		}

		std::wstring ptcPath;
		s_AEGP_GetInSpecFilePath( basic_dataP, specH, ptcPath );

		if( ptcPath.empty() == true )
		{
			return A_Err_NONE;
		}

		unsigned char * flat_headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( flat_optionsH, (void**)&flat_headerP );

		if( flat_headerP == nullptr )
		{
			return A_Err_GENERIC;
		}

		AEIO_Handle optionsH = NULL;
		suites.MemorySuite1()->AEGP_NewMemHandle( basic_dataP->aegp_plug_id,
			"TextOptions",
			sizeof( Menge::TextOptions ),
			AEGP_MemFlag_CLEAR,
			&optionsH );

		if( optionsH == nullptr )
		{
			return A_Err_GENERIC;
		}

		Menge::TextOptions * new_headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void **>(&new_headerP) );

		if( new_headerP == nullptr )
		{
			return A_Err_GENERIC;
		}

		new_headerP->readFlatten( flat_headerP );

		bool successful = true;

		//bool successful = s_InitInSpecFromFile( new_headerP, m_render, ptcPath );

		if( successful == true )
		{
			AEIO_Handle old_optionsH = NULL;
			suites.IOInSuite4()->AEGP_SetInSpecOptionsHandle( specH,
				optionsH,
				reinterpret_cast<void **>(&old_optionsH) );

			suites.IOInSuite4()->AEGP_SetInSpecDepth( specH, 32 ); // always 32 bits
			suites.IOInSuite4()->AEGP_SetInSpecDuration( specH, &new_headerP->duration );
			suites.IOInSuite4()->AEGP_SetInSpecDimensions( specH, new_headerP->width, new_headerP->height );

			A_Fixed	native_fps = FLOAT2FIX( 29.97 );
			suites.IOInSuite4()->AEGP_SetInSpecNativeFPS( specH, native_fps );
		}

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );
		suites.MemorySuite1()->AEGP_UnlockMemHandle( flat_optionsH );

		if( successful == false )
		{
			return A_Err_GENERIC;
		}

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err TextIO::IO_GetInSpecInfo( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, AEIO_Verbiage	* verbiageP )
	{
		suites.ANSICallbacksSuite1()->strcpy( verbiageP->name, "Magic Particles" );
		suites.ANSICallbacksSuite1()->strcpy( verbiageP->type, "(SDK) Astralax Magic Particles File" );
		suites.ANSICallbacksSuite1()->strcpy( verbiageP->sub_type, "no particular subtype" );

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err TextIO::IO_DrawSparseFrame( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, const AEIO_DrawSparseFramePB * sparse_framePPB
		, PF_EffectWorld * wP
		, AEIO_DrawingFlags * draw_flagsP )
	{
		AEIO_Handle optionsH = NULL;
		suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle( specH, reinterpret_cast<void**>(&optionsH) );

		if( optionsH == NULL )
		{
			return A_Err_GENERIC;
		}

		Menge::TextOptions * headerP;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void**>(&headerP) );

		if( headerP == NULL )
		{
			//logger.message( "DrawSparseFrame headerP == NULL"
			//	);

			return A_Err_GENERIC;
		}

		double timing = (double( sparse_framePPB->tr.value ) / double( sparse_framePPB->tr.scale )) * 1000.0;

		//bool successful = s_DrawAstralaxFrame( wP, m_render, headerP, timing );

		bool successful = true;

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );

		if( successful == false )
		{
			return A_Err_NONE;
		}

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err TextIO::IO_GetDimensions( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, const AEIO_RationalScale * rs0
		, A_long * width0
		, A_long * height0 )
	{
		AEIO_Handle optionsH = NULL;
		suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle( specH, reinterpret_cast<void**>(&optionsH) );

		if( optionsH == NULL )
		{
			return A_Err_GENERIC;
		}

		Menge::TextOptions * headerP;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void**>(&headerP) );

		if( headerP == NULL )
		{
			return A_Err_GENERIC;
		}

		*width0 = headerP->width;
		*height0 = headerP->height;

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err TextIO::IO_GetDuration( AEIO_BasicData	*basic_dataP
		, AEIO_InSpecH specH
		, A_Time * tr )
	{
		AEIO_Handle optionsH = NULL;
		suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle( specH, reinterpret_cast<void**>(&optionsH) );

		if( optionsH == NULL )
		{
			return A_Err_GENERIC;
		}

		Menge::TextOptions * headerP;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void**>(&headerP) );

		if( headerP == NULL )
		{
			return A_Err_GENERIC;
		}

		*tr = headerP->duration;

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err TextIO::IO_GetTime( AEIO_BasicData	* basic_dataP
		, AEIO_InSpecH specH
		, A_Time * tr )
	{
		A_Time time_time;
		time_time.value = (A_long)0.0;
		time_time.scale = 1000;

		*tr = time_time;

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err TextIO::IO_DisposeOutputOptions(
		AEIO_BasicData	*basic_dataP,
		void			*optionsPV )
	{
		if( optionsPV == NULL )
		{
			return A_Err_NONE;
		}

		AEIO_Handle	optionsH = reinterpret_cast<AEIO_Handle>(optionsPV);

		Menge::TextOptions * headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void**>(&headerP) );

		if( headerP == nullptr )
		{
			return A_Err_GENERIC;
		}

		//Menge::Emitter * emitter = headerP->emitter;

		//if( emitter != nullptr )
		//{
		//	m_render->destroyEmitter( emitter );
		//}

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );

		suites.MemorySuite1()->AEGP_FreeMemHandle( optionsH );

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err TextIO::IO_GetDepths(
		AEIO_BasicData			*basic_dataP,
		AEIO_OutSpecH			outH,
		AEIO_SupportedDepthFlags		*which )
	{
		/*	Enumerate possible output depths by ORing
		together different AEIO_SupportedDepthFlags.
		*/

		*which = AEIO_SupportedDepthFlags_DEPTH_32;

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err TextIO::IO_VerifyFileImportable(
		AEIO_BasicData			*basic_dataP,
		AEIO_ModuleSignature	sig,
		const A_UTF16Char		*file_pathZ,
		A_Boolean				*importablePB )
	{
		//	This function is an appropriate place to do
		//	in-depth evaluation of whether or not a given
		//	file will be importable; AE already does basic
		//	extension checking. Keep in mind that this 
		//	function should be fairly speedy, to keep from
		//	bogging down the user while selecting files.

		//	-bbb 3/31/04

		//*importablePB = FALSE;

		//char ptcFile[AEGP_MAX_PATH_SIZE];
		//Menge::unicodeToUTF8( ptcFile, AEGP_MAX_PATH_SIZE, (const wchar_t *)file_pathZ, -1 );

		//HM_FILE mf = Magic_OpenFile( file_pathZ );
		////MAGIC_UNKNOWN
		//if( mf != 0 )
		//{
		//	Magic_CloseFile( mf );

		//	*importablePB = TRUE;
		//}

		*importablePB = TRUE;

		return A_Err_NONE;
	}
}
