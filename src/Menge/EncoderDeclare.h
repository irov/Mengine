#	pragma once

#	include "Config/Typedef.h"

#	define ENCODER_DECLARE( C )\
	String m_type;\
		public:\
		static void * C::genObject( void * );\
		private:
