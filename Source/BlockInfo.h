#pragma once
#include <stdint.h>

constexpr uint32_t COOKIE(const char a[4])
{
	return (a[3] << 24) | (a[2] << 16) | (a[1] << 8) | a[0];
};

// Block Types seen
//
// MPB1
// VERT
// MBS2
// VEHW
// CVER
// CMMN
// VELA
// SYMB
// TYPE
// TPGE
// KERN
// STR
// KWGS
// BFRE
// OBJC
// CCOM
// STRI
//

struct Block_MPB1
{
	// MPB1
	uint32_t cookie;
	// Size of the file sub 0x8
	uint32_t size;
	uint32_t unk1;
	uint32_t unk2;
};

struct Block_VERT
{
	// VERT
	uint32_t cookie;
	uint32_t unk1;
};

struct Block_FRAG
{
	// FRAG
	uint32_t cookie;
	uint32_t unk1;
};

struct Block_COMP
{
	// COMP
	uint32_t cookie;
	uint32_t unk1;
};

struct Block_MBS2
{
	// MBS2
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
};

struct Block_VEHW
{
	// VEHW
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
};

struct Block_CVER
{
	// CVER
	uint32_t cookie;
	// XXX: Version?
	uint32_t unk1;
};

struct Block_CMMN
{
	// CMMN
	uint32_t cookie;
	uint32_t unk1;
};

struct Block_VELA
{
	// VELA
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
};

struct Block_SSYM
{
	// SSYM
	uint32_t cookie;
	uint32_t size;
	uint32_t unk2;
};

struct Block_SYMB
{
	// SYMB
	uint32_t cookie;
	uint32_t unk1;
};

struct Block_STRI
{
	// STRI
	uint32_t cookie;
	// Size in bytes padded to the nearest word
	uint32_t size;
	// There is a string here
	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
};

struct Block_TYPE
{
	enum Type : uint32_t
	{
	};
	// TYPE
	uint32_t cookie;
	Type type;
};

struct Block_TPGE
{
	// TPGE
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
	uint32_t unk5;
	// XXX: Sometimes doesn't exist?
//	uint32_t unk6;
};

struct Block_TPIB
{
	// TPIB
	// Type buffer
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
};
struct Block_TPST
{
	// TPST
	// Type struct
	uint32_t cookie;
	// XXX: Haven't seen it to fill it out
};

struct Block_TPSE
{
	// TPSE
	// Type struct/buffer element
	uint32_t cookie;
	uint32_t unk1;
};

struct Block_TPAR
{
	// TPAR
	// Type array
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
};

struct Block_UBUF
{
	// UBUF
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
};

struct Block_EBIN
{
	// EBIN
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
	uint32_t unk5;
	uint32_t unk6;
};

struct Block_FSHA
{
	// FSHA
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
	uint32_t unk5;
	uint32_t unk6;
	uint32_t unk7;
};

struct Block_BFRE
{
	// BFRE
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
};

struct Block_SPDv
{
	// SPDv
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
};

struct Block_SPDf
{
	// SPDf
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
};

struct Block_SPDc
{
	// SPDc
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
};

struct Block_OBJC
{
	// OBJC
	uint32_t cookie;
	uint32_t size;
};

struct Block_CFRA
{
	// CFRA
	uint32_t cookie;
	uint32_t unk1;
};

struct Block_BATT
{
	// BATT
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
};

struct Block_CCOM
{
	// CCOM
	uint32_t cookie;
	uint32_t unk1;
};

struct Block_KERN
{
	// KERN
	uint32_t cookie;
	uint32_t unk1;
};

struct Block_KWGS
{
	// KWGS
	uint32_t cookie;
	uint32_t unk1;
	uint32_t local_x;
	uint32_t local_y;
	uint32_t local_z;
};

struct Block_RLOC
{
	// RLOC
	uint32_t cookie;
	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
	uint32_t unk5;
	uint32_t unk6;

};
