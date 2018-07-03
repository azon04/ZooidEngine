#ifndef __ZE_PRIMITIVE_TYPES__
#define  __ZE_PRIMITIVE_TYPES__

#include <cstdint>

namespace ZE 
{
	
	// 32 bit Float
	typedef float Float32;
	
	// 1 byte int
	typedef std::int8_t Int8;
	
	// 1 byte unsigned int
	typedef std::uint8_t UInt8;

	// 2 bytes int
	typedef std::int16_t Int16;

	// 2 bytes short
	typedef std::int16_t Short;

	// 2 bytes unsigned int
	typedef std::uint16_t UInt16;

	// 2 bytes unsigned short
	typedef std::uint16_t UShort;

	// 4 bytes int
	typedef std::int32_t Int32;

	// 4 bytes unsigned int
	typedef std::uint32_t UInt32;

	// 8 bytes int
	typedef std::int64_t Int64;

	// 8 bytes unsigned int
	typedef std::uint64_t UInt64;

	// unsigned char
	typedef unsigned char UChar;

	// byte
	typedef unsigned char Byte;
}
#endif // !__ZE_PRIMITIVE_TYPES__
