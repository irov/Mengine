#	include "MDLIO.h"

#	include "MDLOptions.h"

#	include "MDLRender.h"

#	include "Model.h"

#	include <stdio.h>
#	include <stdlib.h>

#	include <fstream>

/*	Statics and globals are bad. Never use them in production code.  */
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MDLIO::MDLIO( struct SPBasicSuite * pica_basicP, AEGP_PluginID aegp_plugin_id )
		: AEIO( pica_basicP, aegp_plugin_id )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MDLIO::~MDLIO()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MDLIO::initialize( const char * _name
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

		MDLRender * render = new MDLRender();

		if( render->initialize() == false )
		{
			delete render;

			return false;
		}

		m_render = render;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MDLIO::finalize()
	{
		delete m_render;
		m_render = nullptr;
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
	static bool s_InitInSpecFromFile( Menge::MDLOptions * _headerP
		, Menge::MDLRender * _render
		, const std::wstring & _ptcFile )
	{
		Menge::Model * model = _render->createModel( _ptcFile );

		if( model == nullptr )
		{
			return false;
		}

		A_long width = (A_long)model->getCameraWidth();
		A_long height = (A_long)model->getCameraHeight();

		double duration = model->getDuration();

		A_Time time_duration;
		time_duration.value = (A_long)duration;
		time_duration.scale = 1000;

		_headerP->model = model;
		_headerP->duration = time_duration;

		_headerP->width = width;
		_headerP->height = height;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	A_Err MDLIO::IO_InitInSpecFromFile(
		AEIO_BasicData		*basic_dataP,
		const A_UTF16Char	*file_pathZ,
		AEIO_InSpecH		specH )
	{
		/*	Read the file referenced by the path. Use the
		file information to set all fields of the AEIO_InSpecH.
		*/

		AEIO_Handle optionsH = NULL;
		suites.MemorySuite1()->AEGP_NewMemHandle( basic_dataP->aegp_plug_id,
			"MDLOptions",
			sizeof( Menge::MDLOptions ),
			AEGP_MemFlag_CLEAR,
			&optionsH );

		Menge::MDLOptions * headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void **>(&headerP) );

		if( headerP == NULL )
		{
			return A_Err_GENERIC;
		}

		Menge::MDLRender * render = (Menge::MDLRender *)basic_dataP->aegp_refconPV;

		bool successful = true;

		std::wstring ptc = (const wchar_t *)file_pathZ;
		successful = s_InitInSpecFromFile( headerP, render, ptc );

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
	A_Err MDLIO::IO_DisposeInSpec( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH )
	{
		AEIO_Handle	optionsH = NULL;
		suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle( specH, reinterpret_cast<void**>(&optionsH) );

		if( optionsH == NULL )
		{
			return A_Err_NONE;
		}

		Menge::MDLOptions * old_headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void**>(&old_headerP) );

		if( old_headerP->model != nullptr )
		{
			Menge::MDLRender * render = (Menge::MDLRender *)basic_dataP->aegp_refconPV;
			render->destroyModel( old_headerP->model );
		}

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );
		suites.MemorySuite1()->AEGP_FreeMemHandle( optionsH );

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err MDLIO::IO_FlattenOptions( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, AEIO_Handle * flat_optionsPH )
	{
		AEIO_Handle optionsH = NULL;
		suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle( specH, reinterpret_cast<void**>(&optionsH) );

		Menge::MDLOptions * headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void**>(&headerP) );


		suites.MemorySuite1()->AEGP_NewMemHandle( basic_dataP->aegp_plug_id,
			"MDLFlatten",
			MDL_IO_FLATTEN_OPTIONS_SIZE,
			AEGP_MemFlag_CLEAR,
			flat_optionsPH );

		unsigned char * flat_headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( *flat_optionsPH, (void**)&flat_headerP );

		if( headerP->writeFlatten( flat_headerP ) == false )
		{
			//logger.message( "MDLIO_FlattenOptions headerP invalid writeFlatten"
			//	);
		}

		if( headerP->model != nullptr )
		{
			Menge::MDLRender * render = (Menge::MDLRender *)basic_dataP->aegp_refconPV;

			render->destroyModel( headerP->model );

			headerP->model = nullptr;
		}

		suites.MemorySuite1()->AEGP_UnlockMemHandle( *flat_optionsPH );

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err MDLIO::IO_InflateOptions( AEIO_BasicData * basic_dataP
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
			"MDLOptions",
			sizeof( Menge::MDLOptions ),
			AEGP_MemFlag_CLEAR,
			&optionsH );

		if( optionsH == nullptr )
		{
			return A_Err_GENERIC;
		}

		Menge::MDLOptions * new_headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void **>(&new_headerP) );

		if( new_headerP == nullptr )
		{
			return A_Err_GENERIC;
		}

		new_headerP->readFlatten( flat_headerP );

		Menge::MDLRender * render = (Menge::MDLRender *)basic_dataP->aegp_refconPV;

		bool successful = s_InitInSpecFromFile( new_headerP, render, ptcPath );

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
	A_Err MDLIO::IO_GetInSpecInfo( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, AEIO_Verbiage	* verbiageP )
	{		
		suites.ANSICallbacksSuite1()->strcpy( verbiageP->name, "MDL" );
		suites.ANSICallbacksSuite1()->strcpy( verbiageP->type, "(SDK) MDL File" );
		suites.ANSICallbacksSuite1()->strcpy( verbiageP->sub_type, "no particular subtype" );

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	static bool s_DrawMDLFrame( PF_EffectWorld * wP, Menge::MDLRender * _render, Menge::MDLOptions * _options, double _timing )
	{
		if( _render->beginScene() == false )
		{
			//_logger.message( "DrawSparseFrame beginScene == false"
			//	);

			return false;
		}

		_render->setViewport( wP->width, wP->height );

		Menge::Model * model = _options->model;

		if( model == nullptr )
		{
			//_logger.message( "s_DrawMDLFrame headerP emitter == nullptr"
			//	);

			return false;
		}

		model->update( _timing );

		if( _render->renderModel( model ) == false )
		{
			//_logger.message( "s_DrawMDLFrame headerP invalid renderEmitter"
			//	);

			return false;
		}

		_render->endScene();

		if( _render->getRenderData( (unsigned char *)wP->data, wP->rowbytes, wP->width, wP->height ) == false )
		{
			//_logger.message( "s_DrawMDLFrame headerP invalid getRenderData"
			//	);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	A_Err MDLIO::IO_DrawSparseFrame( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, const AEIO_DrawSparseFramePB * sparse_framePPB
		, PF_EffectWorld * wP
		, AEIO_DrawingFlags * draw_flagsP )
	{
		Menge::MDLRender * render = (Menge::MDLRender *)basic_dataP->aegp_refconPV;

		AEIO_Handle optionsH = NULL;
		suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle( specH, reinterpret_cast<void**>(&optionsH) );

		if( optionsH == NULL )
		{
			return A_Err_GENERIC;
		}

		Menge::MDLOptions * headerP;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void**>(&headerP) );

		if( headerP == NULL )
		{
			//logger.message( "DrawSparseFrame headerP == NULL"
			//	);

			return A_Err_GENERIC;
		}

		double timing = (double( sparse_framePPB->tr.value ) / double( sparse_framePPB->tr.scale )) * 1000.0;

		bool successful = s_DrawMDLFrame( wP, render, headerP, timing );

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );

		if( successful == false )
		{
			return A_Err_GENERIC;
		}

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err MDLIO::IO_GetDimensions( AEIO_BasicData * basic_dataP
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

		Menge::MDLOptions * headerP;
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
	A_Err MDLIO::IO_GetDuration( AEIO_BasicData	*basic_dataP
		, AEIO_InSpecH specH
		, A_Time * tr )
	{
		AEIO_Handle optionsH = NULL;
		suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle( specH, reinterpret_cast<void**>(&optionsH) );

		if( optionsH == NULL )
		{
			return A_Err_GENERIC;
		}

		Menge::MDLOptions * headerP;
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
	A_Err MDLIO::IO_GetTime( AEIO_BasicData	* basic_dataP
		, AEIO_InSpecH specH
		, A_Time * tr )
	{
		AEIO_Handle optionsH = NULL;
		suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle( specH, reinterpret_cast<void**>(&optionsH) );

		if( optionsH == NULL )
		{
			return A_Err_GENERIC;
		}

		Menge::MDLOptions * headerP;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void**>(&headerP) );

		if( headerP == NULL )
		{
			return A_Err_GENERIC;
		}

		if( headerP->model != nullptr )
		{
			double time = headerP->model->getTime();

			A_Time time_time;
			time_time.value = (A_long)time;
			time_time.scale = 1000;

			*tr = time_time;
		}

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );

		if( headerP->model == nullptr )
		{
			return A_Err_GENERIC;
		}

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err MDLIO::IO_DisposeOutputOptions(
		AEIO_BasicData	*basic_dataP,
		void			*optionsPV )
	{
		if( optionsPV == NULL )
		{
			return A_Err_NONE;
		}

		AEIO_Handle	optionsH = reinterpret_cast<AEIO_Handle>(optionsPV);

		Menge::MDLOptions * headerP = NULL;
		suites.MemorySuite1()->AEGP_LockMemHandle( optionsH, reinterpret_cast<void**>(&headerP) );

		if( headerP == nullptr )
		{
			return A_Err_GENERIC;
		}

		Menge::Model * model = headerP->model;

		if( model != nullptr )
		{
			Menge::MDLRender * render = (Menge::MDLRender *)basic_dataP->aegp_refconPV;

			render->destroyModel( model );
		}

		suites.MemorySuite1()->AEGP_UnlockMemHandle( optionsH );

		suites.MemorySuite1()->AEGP_FreeMemHandle( optionsH );

		return A_Err_NONE;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err MDLIO::IO_GetDepths(
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
	A_Err MDLIO::IO_VerifyFileImportable(
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
//////////////////////////////////////////////////////////////////////////
A_Err EntryPointFunc( struct SPBasicSuite		*pica_basicP,
	A_long				 	major_versionL,
	A_long					minor_versionL,
	AEGP_PluginID			aegp_plugin_id,
	AEGP_GlobalRefcon		*global_refconP )
{
	AEGP_SuiteHandler	suites( pica_basicP );

	Menge::MDLIO * io = new Menge::MDLIO( pica_basicP, aegp_plugin_id );

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

	if( io->initialize( "MDL IO files (SDK)", 'MDL_', 2048, 2048, "mdl", flags ) == false )
	{
		return A_Err_GENERIC;
	}

	*global_refconP = (AEGP_GlobalRefcon)io;

	return A_Err_NONE;
}
