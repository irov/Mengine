// ***************************************************************
//  seq.h   version:  1.0   ·  date: 03/23/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************
#	pragma once

#   define IROV_PP_RESULT(res) IROV_PP_RESULT_I(res)
#   define IROV_PP_RESULT_I(x) x

#   define IROV_PP_SEQ_TAIL(Q) IROV_PP_RESULT(IROV_PP_SEQ_TAIL_I Q)
#   define IROV_PP_SEQ_TAIL_I(x)

#   define IROV_PP_SEQ_ENUM(Q) IROV_PP_SEQ_HEAD(Q) ## IROV_PP_SEQ_ENUM_I(IROV_PP_SEQ_TAIL(Q))
#   define IROV_PP_SEQ_ENUM_I(Q) IROV_PP_RESULT(IROV_PP_RESULT(IROV_PP_SEQ_FE_I1 Q) ## _END)

#	define IROV_PP_PARA_FIRST_PARAM(F) IROV_PP_PARA_FIRST_PARAM_I F
#	define IROV_PP_PARA_FIRST_PARAM_I(x,y) x

#	define IROV_PP_SEQ_HEAD(Q) IROV_PP_RESULT(IROV_PP_PARA_FIRST_PARAM( (IROV_PP_SEQ_HEAD_I Q) ) ) 
#   define IROV_PP_SEQ_HEAD_I(Q) Q , (
#   define IROV_PP_SEQ_EMPTY(Q) IROV_PP_SEQ_EMPTY_I
#   define IROV_PP_SEQ_EMPTY_I(Q) IROV_PP_SEQ_EMPTY
#   define IROV_PP_SEQ_EMPTY_END
#   define IROV_PP_SEQ_EMPTY_I_END

#   define IROV_PP_SEQ_FE_I1_END
#   define IROV_PP_SEQ_FE_I2_END
#   define IROV_PP_SEQ_FE_I1(x) , x IROV_PP_SEQ_FE_I2
#   define IROV_PP_SEQ_FE_I2(x) , x IROV_PP_SEQ_FE_I1

