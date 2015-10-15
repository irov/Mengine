#	pragma once

#	include "AEConfig.h"

#	include "AE_GeneralPlug.h"
#	include "AE_Macros.h"
#	include "AE_IO.h"

#	include "AEGP_SuiteHandler.h"

namespace Menge
{
	class AEIO
	{
	public:
		AEIO( struct SPBasicSuite * pica_basicP, AEGP_PluginID aegp_plugin_id );
		virtual ~AEIO();

	public:
		virtual bool initialize( const char * _name
			, AEIO_FileType _sig
			, A_long _width
			, A_long _height
			, const char _ext[3]
			, AEIO_ModuleFlags _flag
			);

		virtual void finalize();

	public:
		virtual A_Err IO_InitInSpecFromFile(
			AEIO_BasicData		*basic_dataP,
			const A_UTF16Char	*file_pathZ,
			AEIO_InSpecH		specH );

		virtual A_Err IO_InitInSpecInteractive(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH pinH );

		virtual A_Err IO_DisposeInSpec(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH );

		virtual A_Err IO_FlattenOptions(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH
			, AEIO_Handle * flat_optionsPH );

		virtual A_Err IO_InflateOptions(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH
			, AEIO_Handle flat_optionsH );

		virtual A_Err IO_SynchInSpec(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH
			, A_Boolean * changed0 );

		virtual A_Err IO_GetActiveExtent(
			AEIO_BasicData	* basic_dataP
			, AEIO_InSpecH specH
			, const A_Time * tr
			, A_LRect * extent );

		virtual A_Err IO_GetInSpecInfo(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH
			, AEIO_Verbiage	* verbiageP );

		virtual A_Err IO_DrawSparseFrame(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH
			, const AEIO_DrawSparseFramePB * sparse_framePPB
			, PF_EffectWorld * wP
			, AEIO_DrawingFlags * draw_flagsP );

		virtual A_Err IO_GetDimensions(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH
			, const AEIO_RationalScale * rs0
			, A_long * width0
			, A_long * height0 );

		virtual A_Err IO_GetDuration(
			AEIO_BasicData	*basic_dataP
			, AEIO_InSpecH specH
			, A_Time * tr );

		virtual A_Err IO_GetTime(
			AEIO_BasicData	* basic_dataP
			, AEIO_InSpecH specH
			, A_Time * tr );

		virtual A_Err IO_GetSound(
			AEIO_BasicData *basic_dataP
			, AEIO_InSpecH specH,
			AEIO_SndQuality				quality,
			const AEIO_InterruptFuncs	*interrupt_funcsP0,
			const A_Time				*startPT,
			const A_Time				*durPT,
			A_u_long					start_sampLu,
			A_u_long					num_samplesLu,
			void						*dataPV );

		virtual A_Err IO_InqNextFrameTime(
			AEIO_BasicData			*basic_dataP,
			AEIO_InSpecH			specH,
			const A_Time			*base_time_tr,
			AEIO_TimeDir			time_dir,
			A_Boolean				*found0,
			A_Time					*key_time_tr0 );

		virtual A_Err IO_DisposeOutputOptions(
			AEIO_BasicData	*basic_dataP,
			void			*optionsPV );

		virtual A_Err IO_UserOptionsDialog(
			AEIO_BasicData			*basic_dataP,
			AEIO_OutSpecH			outH,
			const PF_EffectWorld	*sample0,
			A_Boolean				*user_interacted0 );

		virtual A_Err IO_GetOutputInfo(
			AEIO_BasicData		*basic_dataP,
			AEIO_OutSpecH		outH,
			AEIO_Verbiage		*verbiageP );

		virtual A_Err IO_SetOutputFile(
			AEIO_BasicData		*basic_dataP,
			AEIO_OutSpecH		outH,
			const A_UTF16Char	*file_pathZ );

		virtual A_Err IO_StartAdding(
			AEIO_BasicData		*basic_dataP,
			AEIO_OutSpecH		outH,
			A_long				flags );

		virtual A_Err IO_AddFrame(
			AEIO_BasicData			*basic_dataP,
			AEIO_OutSpecH			outH,
			A_long					frame_index,
			A_long					frames,
			const PF_EffectWorld	*wP,
			const A_LPoint			*origin0,
			A_Boolean				was_compressedB,
			AEIO_InterruptFuncs		*inter0 );

		virtual A_Err IO_EndAdding(
			AEIO_BasicData	*basic_dataP,
			AEIO_OutSpecH			outH,
			A_long					flags );

		virtual A_Err IO_OutputFrame(
			AEIO_BasicData			*basic_dataP,
			AEIO_OutSpecH			outH,
			const PF_EffectWorld	*wP );

		virtual A_Err IO_WriteLabels(
			AEIO_BasicData	*basic_dataP,
			AEIO_OutSpecH	outH,
			AEIO_LabelFlags	*written );

		virtual A_Err IO_GetSizes(
			AEIO_BasicData	*basic_dataP,
			AEIO_OutSpecH	outH,
			A_u_longlong	*free_space,
			A_u_longlong	*file_size );

		virtual A_Err IO_Flush(
			AEIO_BasicData	*basic_dataP,
			AEIO_OutSpecH	outH );

		virtual A_Err IO_Idle(
			AEIO_BasicData			*basic_dataP,
			AEIO_ModuleSignature	sig,
			AEIO_IdleFlags			*idle_flags0 );

		virtual A_Err IO_NumAuxFiles(
			AEIO_BasicData			*basic_dataP,
			AEIO_InSpecH			seqH,
			A_long					*files_per_framePL0 );

		virtual A_Err IO_GetDepths(
			AEIO_BasicData			*basic_dataP,
			AEIO_OutSpecH			outH,
			AEIO_SupportedDepthFlags		*which );

		virtual A_Err IO_GetOutputSuffix(
			AEIO_BasicData	*basic_dataP,
			AEIO_OutSpecH	outH,
			A_char			*suffix );

		virtual A_Err IO_SeqOptionsDlg(
			AEIO_BasicData	*basic_dataP,
			AEIO_InSpecH	specH,
			A_Boolean		*user_interactedPB0 );

		virtual A_Err IO_CloseSourceFiles(
			AEIO_BasicData	*basic_dataP,
			AEIO_InSpecH			seqH );

		virtual A_Err IO_CountUserData(
			AEIO_BasicData	*basic_dataP,
			AEIO_InSpecH			inH,
			A_u_long 				typeLu,
			A_u_long				max_sizeLu,
			A_u_long				*num_of_typePLu );

		virtual A_Err IO_SetUserData(
			AEIO_BasicData	*basic_dataP,
			AEIO_OutSpecH			outH,
			A_u_long				typeLu,
			A_u_long				indexLu,
			const AEIO_Handle		dataH );

		virtual A_Err IO_GetUserData(
			AEIO_BasicData	*basic_dataP,
			AEIO_InSpecH			inH,
			A_u_long 				typeLu,
			A_u_long				indexLu,
			A_u_long				max_sizeLu,
			AEIO_Handle				*dataPH );

		virtual A_Err IO_AddMarker(
			AEIO_BasicData	*basic_dataP,
			AEIO_OutSpecH 			outH,
			A_long 					frame_index,
			AEIO_MarkerType 		marker_type,
			void					*marker_dataPV,
			AEIO_InterruptFuncs		*inter0 );

		virtual A_Err IO_VerifyFileImportable(
			AEIO_BasicData			*basic_dataP,
			AEIO_ModuleSignature	sig,
			const A_UTF16Char		*file_pathZ,
			A_Boolean				*importablePB );

		virtual A_Err IO_InitOutputSpec(
			AEIO_BasicData			*basic_dataP,
			AEIO_OutSpecH			outH,
			A_Boolean				*user_interacted );


		virtual A_Err IO_OutputInfoChanged(
			AEIO_BasicData		*basic_dataP,
			AEIO_OutSpecH		outH );

		virtual A_Err IO_GetFlatOutputOptions(
			AEIO_BasicData	*basic_dataP,
			AEIO_OutSpecH	outH,
			AEIO_Handle		*optionsPH );

	public:
		void message( const char * _format, ... );

	public:
		virtual A_Err DeathHook( AEGP_GlobalRefcon unused1 );
		
	protected:		
		struct SPBasicSuite * pica_basicP;
		AEGP_PluginID aegp_plugin_id;

		AEGP_SuiteHandler suites;
	};


}

