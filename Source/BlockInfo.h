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
