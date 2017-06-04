#pragma once
#include <stdint.h>

constexpr uint32_t COOKIE(const char a[4])
{
	return (a[3] << 24) | (a[2] << 16) | (a[1] << 8) | a[0];
};

struct Header
{
	uint32_t cookie;
	uint32_t size;
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
	uint32_t unk1;
	uint32_t unk2;
};

// Main header for the entire binary
struct Block_MBS2
{
	// some kind of version
	uint32_t version;
};

struct Block_VEHW
{
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
};

struct Block_VELA
{
	uint32_t unk2;
};

struct Block_SSYM
{
	uint32_t unk2;
};

struct Block_STRI
{
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
	Type type;
};

struct Block_TPGE
{
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
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
};
struct Block_TPST
{
	// TPST
	// Type struct
	// XXX: Haven't seen it to fill it out
};

struct Block_TPSE
{
	// TPSE
	// Type struct/buffer element
};

struct Block_TPAR
{
	// TPAR
	// Type array
	uint32_t unk2;
};

struct Block_UBUF
{
	// UBUF
	uint32_t unk2;
	uint32_t unk3;
};

struct Block_EBIN
{
	// EBIN
	uint32_t unk2;
	uint32_t unk3;
	uint32_t numRelocs;
	uint32_t unk5;
};

struct Block_FSHA
{
	// FSHA
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
	uint32_t unk2;
};

struct Block_SPDv
{
	// SPDv
	uint32_t unk2;
};

struct Block_SPDf
{
	// SPDf
	uint32_t unk2;
	uint32_t unk3;
};

struct Block_SPDc
{
	// SPDc
	uint32_t unk2;
};

struct Block_BATT
{
	// BATT
	uint32_t unk2;
};

struct Block_KWGS
{
	// KWGS
	uint32_t local_x;
	uint32_t local_y;
	uint32_t local_z;
};

struct Block_RLOC
{
	// RLOC
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
	uint32_t unk5;

};
