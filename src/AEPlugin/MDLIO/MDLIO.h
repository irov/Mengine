#	pragma once

#	include "AEIO.h"

#	include "MDLRender.h"

#	include "entry.h"

namespace Menge
{
	class MDLIO
		: public AEIO
	{
	public:
		MDLIO( struct SPBasicSuite * pica_basicP, AEGP_PluginID aegp_plugin_id );
		~MDLIO();

	public:
		bool initialize( const char * _name
			, AEIO_FileType _sig
			, A_long _width
			, A_long _height
			, const char _ext[3]
			, AEIO_ModuleFlags _flag
			) override;

		void finalize() override;

	protected:
		A_Err IO_InitInSpecFromFile(
			AEIO_BasicData		*basic_dataP,
			const A_UTF16Char	*file_pathZ,
			AEIO_InSpecH		specH ) override;

		A_Err IO_DisposeInSpec(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH ) override;

		A_Err IO_FlattenOptions(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH
			, AEIO_Handle * flat_optionsPH ) override;

		A_Err IO_InflateOptions(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH
			, AEIO_Handle flat_optionsH ) override;

		A_Err IO_GetInSpecInfo(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH
			, AEIO_Verbiage	* verbiageP ) override;

		A_Err IO_DrawSparseFrame(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH
			, const AEIO_DrawSparseFramePB * sparse_framePPB
			, PF_EffectWorld * wP
			, AEIO_DrawingFlags * draw_flagsP ) override;

		A_Err IO_GetDimensions(
			AEIO_BasicData * basic_dataP
			, AEIO_InSpecH specH
			, const AEIO_RationalScale * rs0
			, A_long * width0
			, A_long * height0 ) override;

		A_Err IO_GetDuration(
			AEIO_BasicData	*basic_dataP
			, AEIO_InSpecH specH
			, A_Time * tr ) override;

		A_Err IO_GetTime(
			AEIO_BasicData	* basic_dataP
			, AEIO_InSpecH specH
			, A_Time * tr ) override;

		A_Err IO_DisposeOutputOptions(
			AEIO_BasicData	*basic_dataP,
			void			*optionsPV ) override;

		A_Err IO_GetDepths(
			AEIO_BasicData			*basic_dataP,
			AEIO_OutSpecH			outH,
			AEIO_SupportedDepthFlags		*which ) override;

		A_Err IO_VerifyFileImportable(
			AEIO_BasicData			*basic_dataP,
			AEIO_ModuleSignature	sig,
			const A_UTF16Char		*file_pathZ,
			A_Boolean				*importablePB ) override;

	protected:
		MDLRender * m_render;
	};
}


// This entry point is exported through the PiPL (.r file)
extern "C" DllExport AEGP_PluginInitFuncPrototype EntryPointFunc;
