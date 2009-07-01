#	pragma once

#	define OBJECT_FACTORY( Type )\
	Holder<SceneManager>::hostage()\
	->registerFactory( #Type, Type::genFactory() )

#	define RESOURCE_FACTORY( Type )\
	Holder<ResourceManager>::hostage()\
	->registerFactory( #Type , Type::genFactory() )

#	define DECODER_FACTORY( _typename_, C, _type_ )\
	Holder<DecoderManager>::hostage()\
	->registerFactory( String(_type_) + String(_typename_), C::genFactory() );

#	define ENCODER_FACTORY( _typename_, C, _type_ )\
	Holder<EncoderManager>::hostage()\
	->registerFactory( String(_type_) + String(_typename_), C::genFactory() );
