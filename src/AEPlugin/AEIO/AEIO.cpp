#	include "AEIO.h"

#   include "Interface/PluginInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include <stdio.h>
#	include <stdlib.h>
#	include <stdarg.h>

SERVICE_PROVIDER_EXTERN( ServiceProvider );

SERVICE_EXTERN( OptionsSystem );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( RenderSystem );
SERVICE_EXTERN( RenderService );
SERVICE_EXTERN( RenderMaterialService );
SERVICE_EXTERN( RenderTextureService );
SERVICE_EXTERN( ParticleService );
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( UnicodeService );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( PrototypeService );
SERVICE_EXTERN( NodeService );

SERVICE_EXTERN( WindowsLayer );

PLUGIN_EXPORT( MengeWin32FileGroup );
PLUGIN_EXPORT( MengeImageCodec );
PLUGIN_EXPORT( AstralaxParticlePlugin2 );

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AEIO::AEIO( struct SPBasicSuite * _pica_basicP, AEGP_PluginID _aegp_plugin_id )
		: pica_basicP( _pica_basicP )
		, aegp_plugin_id( _aegp_plugin_id )
		, suites( _pica_basicP )		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AEIO::~AEIO()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_InitInSpecFromFile(
		AEIO_BasicData		*basic_dataP,
		const A_UTF16Char	*file_pathZ,
		AEIO_InSpecH		specH )
	{
		/*	Read the file referenced by the path. Use the
		file information to set all fields of the AEIO_InSpecH.
		*/

		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_InitInSpecFromFile( basic_dataP, file_pathZ, specH );

		return err;
	}
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_InitInSpecInteractive( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH pinH )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_InitInSpecInteractive( basic_dataP, pinH );

		return err;
	}
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_DisposeInSpec( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_DisposeInSpec( basic_dataP, specH );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_FlattenOptions( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, AEIO_Handle * flat_optionsPH )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_FlattenOptions( basic_dataP, specH, flat_optionsPH );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_InflateOptions( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, AEIO_Handle flat_optionsH )
	{
		/*	During this function,

		+	Create a non-flat options structure, containing the info from the
		flat_optionsH you're passed.

		+	Set the options for the InSpecH using AEGP_SetInSpecOptionsHandle().
		*/

		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_InflateOptions( basic_dataP, specH, flat_optionsH );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_SynchInSpec( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, A_Boolean * changed0 )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_SynchInSpec( basic_dataP, specH, changed0 );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_GetActiveExtent( AEIO_BasicData	* basic_dataP
		, AEIO_InSpecH specH
		, const A_Time * tr
		, A_LRect * extent )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_GetActiveExtent( basic_dataP, specH, tr, extent );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_GetInSpecInfo( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, AEIO_Verbiage	* verbiageP )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_GetInSpecInfo( basic_dataP, specH, verbiageP );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_DrawSparseFrame( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, const AEIO_DrawSparseFramePB * sparse_framePPB
		, PF_EffectWorld * wP
		, AEIO_DrawingFlags * draw_flagsP )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_DrawSparseFrame( basic_dataP, specH, sparse_framePPB, wP, draw_flagsP );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_GetDimensions( AEIO_BasicData * basic_dataP
		, AEIO_InSpecH specH
		, const AEIO_RationalScale * rs0
		, A_long * width0
		, A_long * height0 )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_GetDimensions( basic_dataP, specH, rs0, width0, height0 );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_GetDuration( AEIO_BasicData	*basic_dataP
		, AEIO_InSpecH specH
		, A_Time * tr )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_GetDuration( basic_dataP, specH, tr );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_GetTime( AEIO_BasicData	* basic_dataP
		, AEIO_InSpecH specH
		, A_Time * tr )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_GetTime( basic_dataP, specH, tr );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_GetSound( AEIO_BasicData *basic_dataP
		, AEIO_InSpecH specH,
		AEIO_SndQuality				quality,
		const AEIO_InterruptFuncs	*interrupt_funcsP0,
		const A_Time				*startPT,
		const A_Time				*durPT,
		A_u_long					start_sampLu,
		A_u_long					num_samplesLu,
		void						*dataPV )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_GetSound( basic_dataP, specH, quality, interrupt_funcsP0, startPT, durPT, start_sampLu, num_samplesLu, dataPV );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_InqNextFrameTime(
		AEIO_BasicData			*basic_dataP,
		AEIO_InSpecH			specH,
		const A_Time			*base_time_tr,
		AEIO_TimeDir			time_dir,
		A_Boolean				*found0,
		A_Time					*key_time_tr0 )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_InqNextFrameTime( basic_dataP, specH, base_time_tr, time_dir, found0, key_time_tr0 );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_DisposeOutputOptions(
		AEIO_BasicData	*basic_dataP,
		void			*optionsPV )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_DisposeOutputOptions( basic_dataP, optionsPV );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_UserOptionsDialog(
		AEIO_BasicData			*basic_dataP,
		AEIO_OutSpecH			outH,
		const PF_EffectWorld	*sample0,
		A_Boolean				*user_interacted0 )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_UserOptionsDialog( basic_dataP, outH, sample0, user_interacted0 );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_GetOutputInfo(
		AEIO_BasicData		*basic_dataP,
		AEIO_OutSpecH		outH,
		AEIO_Verbiage		*verbiageP )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_GetOutputInfo( basic_dataP, outH, verbiageP );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_SetOutputFile(
		AEIO_BasicData		*basic_dataP,
		AEIO_OutSpecH		outH,
		const A_UTF16Char	*file_pathZ )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_SetOutputFile( basic_dataP, outH, file_pathZ );

		return err;
	}
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_StartAdding(
		AEIO_BasicData		*basic_dataP,
		AEIO_OutSpecH		outH,
		A_long				flags )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_StartAdding( basic_dataP, outH, flags );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_AddFrame(
		AEIO_BasicData			*basic_dataP,
		AEIO_OutSpecH			outH,
		A_long					frame_index,
		A_long					frames,
		const PF_EffectWorld	*wP,
		const A_LPoint			*origin0,
		A_Boolean				was_compressedB,
		AEIO_InterruptFuncs		*inter0 )
	{
		/*
		+	reinterpret the provided frame into your format.
		+	append it to the already-opened file.
		*/

		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_AddFrame( basic_dataP, outH, frame_index, frames, wP, origin0, was_compressedB, inter0 );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_EndAdding(
		AEIO_BasicData	*basic_dataP,
		AEIO_OutSpecH			outH,
		A_long					flags )
	{
		/*
		+	Close file handle, clear out optionsH associated with add.
		*/

		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_EndAdding( basic_dataP, outH, flags );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_OutputFrame(
		AEIO_BasicData			*basic_dataP,
		AEIO_OutSpecH			outH,
		const PF_EffectWorld	*wP )
	{
		/*
		+	Re-interpret the supplied PF_EffectWorld in your own
		format, and save it out to the outH's path.

		*/

		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_OutputFrame( basic_dataP, outH, wP );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_WriteLabels(
		AEIO_BasicData	*basic_dataP,
		AEIO_OutSpecH	outH,
		AEIO_LabelFlags	*written )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_WriteLabels( basic_dataP, outH, written );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_GetSizes(
		AEIO_BasicData	*basic_dataP,
		AEIO_OutSpecH	outH,
		A_u_longlong	*free_space,
		A_u_longlong	*file_size )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_GetSizes( basic_dataP, outH, free_space, file_size );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_Flush(
		AEIO_BasicData	*basic_dataP,
		AEIO_OutSpecH	outH )
	{
		/*	free any temp buffers you kept around for
		writing.
		*/

		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_Flush( basic_dataP, outH );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_Idle(
		AEIO_BasicData			*basic_dataP,
		AEIO_ModuleSignature	sig,
		AEIO_IdleFlags			*idle_flags0 )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_Idle( basic_dataP, sig, idle_flags0 );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_NumAuxFiles(
		AEIO_BasicData			*basic_dataP,
		AEIO_InSpecH			seqH,
		A_long					*files_per_framePL0 )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_NumAuxFiles( basic_dataP, seqH, files_per_framePL0 );

		return err;
	}
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_GetDepths(
		AEIO_BasicData			*basic_dataP,
		AEIO_OutSpecH			outH,
		AEIO_SupportedDepthFlags		*which )
	{
		/*	Enumerate possible output depths by ORing
		together different AEIO_SupportedDepthFlags.
		*/

		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_GetDepths( basic_dataP, outH, which );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_GetOutputSuffix(
		AEIO_BasicData	*basic_dataP,
		AEIO_OutSpecH	outH,
		A_char			*suffix )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_GetOutputSuffix( basic_dataP, outH, suffix );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_SeqOptionsDlg(
		AEIO_BasicData	*basic_dataP,
		AEIO_InSpecH	specH,
		A_Boolean		*user_interactedPB0 )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_SeqOptionsDlg( basic_dataP, specH, user_interactedPB0 );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_CloseSourceFiles(
		AEIO_BasicData	*basic_dataP,
		AEIO_InSpecH			seqH )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_CloseSourceFiles( basic_dataP, seqH );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_CountUserData(
		AEIO_BasicData	*basic_dataP,
		AEIO_InSpecH			inH,
		A_u_long 				typeLu,
		A_u_long				max_sizeLu,
		A_u_long				*num_of_typePLu )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_CountUserData( basic_dataP, inH, typeLu, max_sizeLu, num_of_typePLu );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_SetUserData(
		AEIO_BasicData	*basic_dataP,
		AEIO_OutSpecH			outH,
		A_u_long				typeLu,
		A_u_long				indexLu,
		const AEIO_Handle		dataH )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_SetUserData( basic_dataP, outH, typeLu, indexLu, dataH );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_GetUserData(
		AEIO_BasicData	*basic_dataP,
		AEIO_InSpecH			inH,
		A_u_long 				typeLu,
		A_u_long				indexLu,
		A_u_long				max_sizeLu,
		AEIO_Handle				*dataPH )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_GetUserData( basic_dataP, inH, typeLu, indexLu, max_sizeLu, dataPH );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_AddMarker(
		AEIO_BasicData	*basic_dataP,
		AEIO_OutSpecH 			outH,
		A_long 					frame_index,
		AEIO_MarkerType 		marker_type,
		void					*marker_dataPV,
		AEIO_InterruptFuncs		*inter0 )
	{
		/*	The type of the marker is in marker_type,
		and the text they contain is in
		marker_dataPV.
		*/

		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_AddMarker( basic_dataP, outH, frame_index, marker_type, marker_dataPV, inter0 );

		return err;
	};
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_VerifyFileImportable(
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

		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_VerifyFileImportable( basic_dataP, sig, file_pathZ, importablePB );

		return err;
	}
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_InitOutputSpec(
		AEIO_BasicData			*basic_dataP,
		AEIO_OutSpecH			outH,
		A_Boolean				*user_interacted )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_InitOutputSpec( basic_dataP, outH, user_interacted );

		return err;
	}
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_OutputInfoChanged(
		AEIO_BasicData		*basic_dataP,
		AEIO_OutSpecH		outH )
	{
		/*	This function is called when either the user
		or the plug-in has changed the output options info.
		You may want to update your plug-in's internal
		representation of the output at this time.
		We've exercised the likely getters below.
		*/

		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_OutputInfoChanged( basic_dataP, outH );

		return err;
	}
	//////////////////////////////////////////////////////////////////////////
	static A_Err IO_GetFlatOutputOptions(
		AEIO_BasicData	*basic_dataP,
		AEIO_OutSpecH	outH,
		AEIO_Handle		*optionsPH )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)basic_dataP->aegp_refconPV;

		A_Err err = aeio->IO_GetFlatOutputOptions( basic_dataP, outH, optionsPH );

		return err;
	}
	//////////////////////////////////////////////////////////////////////////
	static A_Err ConstructModuleInfo( SPBasicSuite * pica_basicP, AEIO_ModuleInfo * info, const char * _name, AEIO_FileType _sig, A_long _width, A_long _height, const char _ext[3], AEIO_ModuleFlags _flag )
	{
		A_Err err = A_Err_NONE;

		AEGP_SuiteHandler	suites( pica_basicP );

		if( info )
		{
			info->sig = _sig;
			info->max_width = _width;
			info->max_height = _height;
			info->num_filetypes = 1;
			info->num_extensions = 1;
			info->num_clips = 0;

			info->create_kind.type = _sig;
			info->create_kind.creator = AEIO_ANY_CREATOR;

			info->create_ext.pad = '.';
			info->create_ext.extension[0] = _ext[0];
			info->create_ext.extension[1] = _ext[1];
			info->create_ext.extension[2] = _ext[2];
			info->num_aux_extensionsS = 0;

			suites.ANSICallbacksSuite1()->strcpy( info->name, _name );

			info->flags = _flag;

			info->read_kinds[0].mac.type = _sig;
			info->read_kinds[0].mac.creator = AEIO_ANY_CREATOR;
			info->read_kinds[1].ext = info->create_ext;
			info->read_kinds[2].ext.pad = '.';
			info->read_kinds[2].ext.extension[0] = _ext[0];
			info->read_kinds[2].ext.extension[1] = _ext[1];
			info->read_kinds[2].ext.extension[2] = _ext[2];
		}
		else
		{
			err = A_Err_STRUCT;
		}

		return err;
	}
	//////////////////////////////////////////////////////////////////////////
	static A_Err ConstructFunctionBlock( AEIO_FunctionBlock4 * funcs )
	{
		if( funcs )
		{
			funcs->AEIO_AddFrame = IO_AddFrame;
			funcs->AEIO_CloseSourceFiles = IO_CloseSourceFiles;
			funcs->AEIO_CountUserData = IO_CountUserData;
			funcs->AEIO_DisposeInSpec = IO_DisposeInSpec;
			funcs->AEIO_DisposeOutputOptions = IO_DisposeOutputOptions;
			funcs->AEIO_DrawSparseFrame = IO_DrawSparseFrame;
			funcs->AEIO_EndAdding = IO_EndAdding;
			funcs->AEIO_FlattenOptions = IO_FlattenOptions;
			funcs->AEIO_Flush = IO_Flush;
			funcs->AEIO_GetActiveExtent = IO_GetActiveExtent;
			funcs->AEIO_GetDepths = IO_GetDepths;
			funcs->AEIO_GetDimensions = IO_GetDimensions;
			funcs->AEIO_GetDuration = IO_GetDuration;
			funcs->AEIO_GetSound = IO_GetSound;
			funcs->AEIO_GetInSpecInfo = IO_GetInSpecInfo;
			funcs->AEIO_GetOutputInfo = IO_GetOutputInfo;
			funcs->AEIO_GetOutputSuffix = IO_GetOutputSuffix;
			funcs->AEIO_GetSizes = IO_GetSizes;
			funcs->AEIO_GetTime = IO_GetTime;
			funcs->AEIO_GetUserData = IO_GetUserData;
			funcs->AEIO_InflateOptions = IO_InflateOptions;
			funcs->AEIO_InitInSpecFromFile = IO_InitInSpecFromFile;
			funcs->AEIO_InitInSpecInteractive = IO_InitInSpecInteractive;
			funcs->AEIO_InqNextFrameTime = IO_InqNextFrameTime;
			funcs->AEIO_OutputFrame = IO_OutputFrame;
			funcs->AEIO_SeqOptionsDlg = IO_SeqOptionsDlg;
			funcs->AEIO_SetOutputFile = IO_SetOutputFile;
			funcs->AEIO_SetUserData = IO_SetUserData;
			funcs->AEIO_StartAdding = IO_StartAdding;
			funcs->AEIO_SynchInSpec = IO_SynchInSpec;
			funcs->AEIO_UserOptionsDialog = IO_UserOptionsDialog;
			funcs->AEIO_VerifyFileImportable = IO_VerifyFileImportable;
			funcs->AEIO_WriteLabels = IO_WriteLabels;
			funcs->AEIO_InitOutputSpec = IO_InitOutputSpec;
			funcs->AEIO_GetFlatOutputOptions = IO_GetFlatOutputOptions;
			funcs->AEIO_OutputInfoChanged = IO_OutputInfoChanged;
			funcs->AEIO_AddMarker = IO_AddMarker;
			funcs->AEIO_Idle = IO_Idle;
			funcs->AEIO_NumAuxFiles = IO_NumAuxFiles;

			return A_Err_NONE;
		}
		else
		{
			return A_Err_STRUCT;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static A_Err s_deathHook(
		AEGP_GlobalRefcon unused1,
		AEGP_DeathRefcon unused2 )
	{
		Menge::AEIO * aeio = (Menge::AEIO *)unused2;

		aeio->finalize();

		A_Err err = aeio->IO_DeathHook( unused1 );

		return err;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AEIO::initialize( const char * _name
		, AEIO_FileType _sig
		, A_long _width
		, A_long _height
		, const char _ext[3]
		, AEIO_ModuleFlags _flag
		)
	{
		suites.RegisterSuite5()->AEGP_RegisterDeathHook( aegp_plugin_id, &s_deathHook, (AEGP_DeathRefcon)this );

		AEIO_ModuleInfo info;
		AEFX_CLR_STRUCT( info );
		ConstructModuleInfo( pica_basicP, &info, _name, _sig, _width, _height, _ext, _flag );

		AEIO_FunctionBlock4	funcs;
		AEFX_CLR_STRUCT( funcs );
		ConstructFunctionBlock( &funcs );

		suites.RegisterSuite5()->AEGP_RegisterIO( aegp_plugin_id,
			(AEGP_IORefcon)this,
			&info,
			&funcs );

		ServiceProviderInterface * serviceProvider;
		SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

		m_serviceProvider = serviceProvider;

		SERVICE_CREATE( m_serviceProvider, OptionsSystem );
		SERVICE_CREATE( m_serviceProvider, OptionsService );

		if( this->initializeLogEngine_() == false )
		{
			return false;
		}

		SERVICE_CREATE( m_serviceProvider, StringizeService );
		SERVICE_CREATE( m_serviceProvider, LoggerService );

		SERVICE_CREATE( m_serviceProvider, WindowsLayer );

		SERVICE_CREATE( m_serviceProvider, NotificationService );

		SERVICE_CREATE( m_serviceProvider, UnicodeSystem );
		SERVICE_CREATE( m_serviceProvider, UnicodeService );

		SERVICE_CREATE( m_serviceProvider, FileService );

		if( this->initializeFileEngine_() == false )
		{
			return false;
		}

		SERVICE_CREATE( m_serviceProvider, ConfigService );


		SERVICE_CREATE( m_serviceProvider, RenderSystem );

		SERVICE_CREATE( m_serviceProvider, RenderService );
		SERVICE_CREATE( m_serviceProvider, RenderMaterialService );
		SERVICE_CREATE( m_serviceProvider, RenderTextureService );

		SERVICE_CREATE( m_serviceProvider, CodecService );
		SERVICE_CREATE( m_serviceProvider, MemoryService );

		SERVICE_CREATE( m_serviceProvider, ParticleService );

#	define MENGINE_ADD_PLUGIN( Name, Info )\
		do{LOGGER_INFO(m_serviceProvider)( Info );\
		if(	PLUGIN_CREATE(m_serviceProvider, Name) == false ){\
		LOGGER_ERROR(m_serviceProvider)( "Invalid %s", Info );}else{\
		LOGGER_WARNING(m_serviceProvider)( "Successful %s", Info );}}while(false, false)

		MENGINE_ADD_PLUGIN( MengeImageCodec, "initialize Plugin Image Codec..." );
		MENGINE_ADD_PLUGIN( AstralaxParticlePlugin2, "initialize Plugin Astralax Particle..." );

		SERVICE_CREATE( m_serviceProvider, PrototypeService );
		SERVICE_CREATE( m_serviceProvider, NodeService );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AEIO::finalize()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_InitInSpecFromFile(
		AEIO_BasicData		*,
		const A_UTF16Char	*,
		AEIO_InSpecH )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	}
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_InitInSpecInteractive( AEIO_BasicData *
		, AEIO_InSpecH )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	}
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_DisposeInSpec( AEIO_BasicData *
		, AEIO_InSpecH )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_FlattenOptions( AEIO_BasicData *
		, AEIO_InSpecH
		, AEIO_Handle * )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_InflateOptions( AEIO_BasicData *
		, AEIO_InSpecH
		, AEIO_Handle )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_SynchInSpec( AEIO_BasicData *
		, AEIO_InSpecH
		, A_Boolean * )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_GetActiveExtent( AEIO_BasicData	*
		, AEIO_InSpecH
		, const A_Time *
		, A_LRect * )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_GetInSpecInfo( AEIO_BasicData *
		, AEIO_InSpecH
		, AEIO_Verbiage	* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_DrawSparseFrame( AEIO_BasicData *
		, AEIO_InSpecH
		, const AEIO_DrawSparseFramePB *
		, PF_EffectWorld *
		, AEIO_DrawingFlags * )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_GetDimensions( AEIO_BasicData *
		, AEIO_InSpecH
		, const AEIO_RationalScale *
		, A_long *
		, A_long * )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_GetDuration( AEIO_BasicData	*
		, AEIO_InSpecH
		, A_Time * )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_GetTime( AEIO_BasicData	*
		, AEIO_InSpecH
		, A_Time * )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_GetSound(
		AEIO_BasicData *,
		AEIO_InSpecH,
		AEIO_SndQuality,
		const AEIO_InterruptFuncs	*,
		const A_Time				*,
		const A_Time				*,
		A_u_long,
		A_u_long,
		void						* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_InqNextFrameTime(
		AEIO_BasicData			*,
		AEIO_InSpecH,
		const A_Time			*,
		AEIO_TimeDir,
		A_Boolean				*,
		A_Time					* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_DisposeOutputOptions(
		AEIO_BasicData	*,
		void			* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_UserOptionsDialog(
		AEIO_BasicData			*,
		AEIO_OutSpecH,
		const PF_EffectWorld	*,
		A_Boolean				* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_GetOutputInfo(
		AEIO_BasicData		*,
		AEIO_OutSpecH,
		AEIO_Verbiage		* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_SetOutputFile(
		AEIO_BasicData		*,
		AEIO_OutSpecH,
		const A_UTF16Char	* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	}
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_StartAdding(
		AEIO_BasicData		*,
		AEIO_OutSpecH,
		A_long )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_AddFrame(
		AEIO_BasicData			*,
		AEIO_OutSpecH,
		A_long,
		A_long,
		const PF_EffectWorld	*,
		const A_LPoint			*,
		A_Boolean,
		AEIO_InterruptFuncs		* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_EndAdding(
		AEIO_BasicData	*,
		AEIO_OutSpecH,
		A_long )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_OutputFrame(
		AEIO_BasicData			*,
		AEIO_OutSpecH,
		const PF_EffectWorld	* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_WriteLabels(
		AEIO_BasicData	*,
		AEIO_OutSpecH,
		AEIO_LabelFlags	* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_GetSizes(
		AEIO_BasicData	*,
		AEIO_OutSpecH,
		A_u_longlong	*,
		A_u_longlong	* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_Flush(
		AEIO_BasicData	*,
		AEIO_OutSpecH )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_Idle(
		AEIO_BasicData			*,
		AEIO_ModuleSignature,
		AEIO_IdleFlags			* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_NumAuxFiles(
		AEIO_BasicData			*,
		AEIO_InSpecH,
		A_long					* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	}
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_GetDepths(
		AEIO_BasicData			*,
		AEIO_OutSpecH,
		AEIO_SupportedDepthFlags		* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_GetOutputSuffix(
		AEIO_BasicData	*,
		AEIO_OutSpecH,
		A_char			* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_SeqOptionsDlg(
		AEIO_BasicData	*,
		AEIO_InSpecH,
		A_Boolean		* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_CloseSourceFiles(
		AEIO_BasicData	*,
		AEIO_InSpecH )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_CountUserData(
		AEIO_BasicData	*,
		AEIO_InSpecH,
		A_u_long,
		A_u_long,
		A_u_long				* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_SetUserData(
		AEIO_BasicData	*,
		AEIO_OutSpecH,
		A_u_long,
		A_u_long,
		const AEIO_Handle )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_GetUserData(
		AEIO_BasicData	*,
		AEIO_InSpecH,
		A_u_long,
		A_u_long,
		A_u_long,
		AEIO_Handle				* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_AddMarker(
		AEIO_BasicData	*,
		AEIO_OutSpecH,
		A_long,
		AEIO_MarkerType,
		void					*,
		AEIO_InterruptFuncs		* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	};
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_VerifyFileImportable(
		AEIO_BasicData			*,
		AEIO_ModuleSignature,
		const A_UTF16Char		*,
		A_Boolean				* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	}
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_InitOutputSpec(
		AEIO_BasicData			*,
		AEIO_OutSpecH,
		A_Boolean				* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	}
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_OutputInfoChanged(
		AEIO_BasicData		*,
		AEIO_OutSpecH )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	}
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_GetFlatOutputOptions(
		AEIO_BasicData	*,
		AEIO_OutSpecH,
		AEIO_Handle		* )
	{
		return AEIO_Err_USE_DFLT_CALLBACK;
	}
	//////////////////////////////////////////////////////////////////////////
	A_Err AEIO::IO_DeathHook( AEGP_GlobalRefcon )
	{
		delete this;

		return A_Err_NONE;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AEIO::initializeLogEngine_()
	{
#	ifdef _DEBUG
		{
			m_stdioLogger = new STDIOLogger();

			LOGGER_SERVICE( m_serviceProvider )
				->registerLogger( m_stdioLogger );
		}
#	endif

		m_aeioLogger = new AEIOLogger( suites, aegp_plugin_id );

		m_aeioLogger->setVerboseLevel( LM_ERROR );

		LOGGER_SERVICE( m_serviceProvider )
			->registerLogger( m_aeioLogger );

		LOGGER_SERVICE( m_serviceProvider )
			->setVerboseLevel( LM_MAX );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AEIO::initializeFileEngine_()
	{
		LOGGER_INFO( m_serviceProvider )("Inititalizing File Service...");

		{
			LOGGER_INFO( m_serviceProvider )("Initialize Win32 file group...");
			PLUGIN_CREATE( m_serviceProvider, MengeWin32FileGroup );
		}

		// mount root		
		if( FILE_SERVICE( m_serviceProvider )->mountFileGroup( ConstString::none(), ConstString::none(), STRINGIZE_STRING_LOCAL( m_serviceProvider, "dir" ) ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("WinApplication::setupFileService: failed to mount application directory"
				);

			return false;
		}

		return true;
	}
}
