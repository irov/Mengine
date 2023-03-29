#pragma once

#include "Interface/StatisticServiceInterface.h"
#include "Interface/StatisticInterface.h"

#if defined(MENGINE_STATISTIC_ENABLE)
#   define STATISTIC_INC_INTEGER(ID) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->addStatisticInteger(ID, 1) : MENGINE_NOP)
#   define STATISTIC_DEC_INTEGER(ID) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->delStatisticInteger(ID, 1) : MENGINE_NOP)
#   define STATISTIC_ADD_INTEGER(ID, VALUE) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->addStatisticInteger(ID, (VALUE)) : MENGINE_NOP)
#   define STATISTIC_DEL_INTEGER(ID, VALUE) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->delStatisticInteger(ID, (VALUE)) : MENGINE_NOP)
#   define STATISTIC_GET_INTEGER(ID) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->getStatisticInteger(ID) : 0LL)
#   define STATISTIC_RESET_INTEGER(ID) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->resetStatisticInteger(ID) : MENGINE_NOP)

#   define STATISTIC_ADD_DOUBLE(ID, VALUE) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->addStatisticDouble(ID, (VALUE)) : MENGINE_NOP)
#   define STATISTIC_DEL_DOUBLE(ID, VALUE) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->addStatisticDouble(ID, -(VALUE)) : MENGINE_NOP)
#   define STATISTIC_GET_DOUBLE(ID) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->getStatisticDouble(ID) : 0.0)
#   define STATISTIC_RESET_DOUBLE(ID) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->resetStatisticDouble(ID) : MENGINE_NOP)

#   define STATISTIC_SET_CONSTSTRING(ID, VALUE) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->setStatisticConstString(ID, (VALUE)) : MENGINE_NOP)
#   define STATISTIC_GET_CONSTSTRING(ID) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->getStatisticConstString(ID) : ConstString::none())
#   define STATISTIC_RESET_CONSTSTRING(ID) (SERVICE_IS_INITIALIZE(StatisticServiceInterface) == true ? STATISTIC_SERVICE()->resetStatisticConstString(ID) : MENGINE_NOP)
#else
#   define STATISTIC_INC_INTEGER(ID)
#   define STATISTIC_DEC_INTEGER(ID)
#   define STATISTIC_ADD_INTEGER(ID, VALUE)
#   define STATISTIC_DEL_INTEGER(ID, VALUE)
#   define STATISTIC_GET_INTEGER(ID) (0LL)
#   define STATISTIC_RESET_INTEGER(ID)

#   define STATISTIC_ADD_DOUBLE(ID, VALUE)
#   define STATISTIC_DEL_DOUBLE(ID, VALUE)
#   define STATISTIC_GET_DOUBLE(ID) (0.0)
#   define STATISTIC_RESET_DOUBLE(ID)

#   define STATISTIC_SET_CONSTSTRING(ID, VALUE)
#   define STATISTIC_GET_CONSTSTRING(ID) (ConstString::none())
#   define STATISTIC_RESET_CONSTSTRING(ID)
#endif