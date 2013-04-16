#	pragma once

#	define STRINGIZE_I(A) #A
#	define STRINGIZE(A) STRINGIZE_I(A)

#	include STRINGIZE(MENGINE_CONFIG)