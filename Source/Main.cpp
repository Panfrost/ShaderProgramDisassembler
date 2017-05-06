#include <assert.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

#include "BlockInfo.h"

bool ReadFile(std::vector<uint8_t> *fileData, std::string filename)
{
	FILE* fp = nullptr;
	size_t filesize = 0;

	fp = fopen(filename.c_str(), "rb");
	if (!fp)
		return false;

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	rewind(fp);

	fileData->resize(filesize);

	fread(fileData->data(), 1, filesize, fp);

	fclose(fp);
	return true;

err:
	if (fp) {
		fclose(fp);
	}
	return false;
}

void PrintFile(std::vector<uint8_t> *fileData, size_t offset = 0, size_t realpos = 0)
{
	printf("file size is 0x%08llx\n", fileData->size());
	uint32_t *data = reinterpret_cast<uint32_t*>(fileData->data() + offset);
	size_t size = std::min(fileData->size() / 4, 50ul);
	for (size_t i = 0; i < size; i++)
	{
		uint32_t word = data[i];
		auto getL = [word](int off)
		{
			return (uint8_t)(word >> off);
		};
		printf("%s0x%08x :0x%08x: %c%c%c%c\n",
			((realpos - offset) / 4 == i) ? "->" : "",
			offset + i * 4,
			word,
			getL(0), getL(8), getL(16), getL(24));
	}
}

void DumpInstructions(uint8_t* instBlob, uint32_t size)
{
	// Instructions are 64bits
	// Clause headers and tails are mainly 32bit
	// There can be 64bit clause headers, no idea when they are used
	uint8_t* instEnd = instBlob + size;
	while (instBlob != instEnd)
	{
		printf("0x%08x\n", *(uint32_t*)instBlob);
		instBlob += 4;
	}
}

// Attempt to parse a single block with maxSize words
bool ParseSingleBlock(uint8_t* blockBlob, size_t *blockSize)
{
	uint32_t cookie = *reinterpret_cast<uint32_t*>(blockBlob);
	switch (cookie)
	{
	case COOKIE("MPB1"):
	{
		Block_MPB1* block = reinterpret_cast<Block_MPB1*>(blockBlob);
		printf("Block_MPB1\n");
		printf("\tSize: 0x%08x\n", block->size);
		assert(block->unk1 == 2);
		assert(block->unk2 == 0);
		*blockSize = sizeof(Block_MPB1);
	}
	break;
	case COOKIE("VERT"):
	{
		Block_VERT* block = reinterpret_cast<Block_VERT*>(blockBlob);
		printf("Block_VERT\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);

		assert(block->unk1 == 0x45c);
		*blockSize = sizeof(Block_VERT);
	}
	break;
	case COOKIE("FRAG"):
	{
		Block_FRAG* block = reinterpret_cast<Block_FRAG*>(blockBlob);
		printf("Block_FRAG\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);

		assert(block->unk1 == 0x37c);
		*blockSize = sizeof(Block_FRAG);
	}
	break;
	case COOKIE("COMP"):
	{
		Block_COMP* block = reinterpret_cast<Block_COMP*>(blockBlob);
		printf("Block_COMP\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);

		// XXX: Changes, probably a size
		//assert(block->unk1 == 0x1a8);
		*blockSize = sizeof(Block_COMP);
	}
	break;
	case COOKIE("MBS2"):
	{
		Block_MBS2* block = reinterpret_cast<Block_MBS2*>(blockBlob);
		printf("Block_MBS2\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);

		// XXX: Sometimes different
		//assert(block->unk1 == 0x454);
		assert(block->unk2 == 0x12);
		*blockSize = sizeof(Block_MBS2);
	}
	break;
	case COOKIE("VEHW"):
	{
		Block_VEHW* block = reinterpret_cast<Block_VEHW*>(blockBlob);
		printf("Block_VEHW\n");
		assert(block->unk1 == 0xc);
		assert(block->unk2 == 0xb);
		assert(block->unk3 == 0x0);
		assert(block->unk4 == 0x0);
		*blockSize = sizeof(Block_VEHW);
	}
	break;
	case COOKIE("CVER"):
	{
		Block_CVER* block = reinterpret_cast<Block_CVER*>(blockBlob);
		printf("Block_CVER\n");
		assert(block->unk1 == 0x434);
		*blockSize = sizeof(Block_CVER);
	}
	break;
	case COOKIE("CMMN"):
	{
		Block_CMMN* block = reinterpret_cast<Block_CMMN*>(blockBlob);
		printf("Block_CMMN\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);

		// XXX: Sometimes different
		//assert(block->unk1 == 0x42c);
		*blockSize = sizeof(Block_CMMN);
	}
	break;
	case COOKIE("VELA"):
	{
		Block_VELA* block = reinterpret_cast<Block_VELA*>(blockBlob);
		printf("Block_VELA\n");
		assert(block->unk1 == 0x4);
		assert(block->unk2 == 0x8);
		*blockSize = sizeof(Block_VELA);
	}
	break;
	case COOKIE("SSYM"):
	{
		Block_SSYM* block = reinterpret_cast<Block_SSYM*>(blockBlob);
		printf("Block_SSYM\n");
		printf("\tsize = 0x%08x\n", block->size);
		printf("\tunk2 = 0x%08x\n", block->unk2);

		// XXX: Sometimes different
		//assert(block->unk2 == 0x2);
		// XXX: Skipping the entire SSYM because parsing of sub blocks(STRI) aren't complete
		*blockSize = sizeof(Block_SSYM) + block->size - 4;
	}
	break;
	case COOKIE("SYMB"):
	{
		Block_SYMB* block = reinterpret_cast<Block_SYMB*>(blockBlob);
		printf("Block_SYMB\n");
		printf("\tunk1: 0x%08x\n", block->unk1);
		// XXX: Sometimes different
		//assert(block->unk1 == 0x4c);
		*blockSize = sizeof(Block_SYMB);
	}
	break;
	case COOKIE("STRI"):
	{
		Block_STRI block;
		block.cookie = *reinterpret_cast<uint32_t*>(blockBlob);
		block.size = *reinterpret_cast<uint32_t*>(blockBlob + 4);
		block.unk1 = *reinterpret_cast<uint32_t*>(blockBlob + block.size + 8);
		block.unk2 = *reinterpret_cast<uint32_t*>(blockBlob + block.size + 12);
		block.unk3 = *reinterpret_cast<uint32_t*>(blockBlob + block.size + 16);
		block.unk4 = *reinterpret_cast<uint32_t*>(blockBlob + block.size + 20);

		// block.size includes the word aligned zero padding for the string
		char* name = static_cast<char*>(malloc(block.size));
		memcpy(name, blockBlob + 8, block.size);

		printf("Block_STRI\n");
		printf("\tSize: 0x%08x\n", block.size);
		printf("\tName: %s\n", name);
		printf("\tunk1 = 0x%08x\n", block.unk1);
		printf("\tunk2 = 0x%08x\n", block.unk2);
		printf("\tunk3 = 0x%08x\n", block.unk3);
		printf("\tunk4 = 0x%08x\n", block.unk4);

		// XXX: Sometimes different
		//assert(block.unk1 == 0x0);
		//assert(block.unk2 == 0x22);
		//assert(block.unk3 == ~0U);
		//assert(block.unk4 == ~0U);
		free(name);

		// XXX: Why do some STRI blocks miss unk4?
		if (block.unk1 == 0x10)
			*blockSize = sizeof(Block_STRI) + block.size - 12;
		else if ((block.unk2 & 0xffff0000) == 0xffff0000)
			*blockSize = sizeof(Block_STRI) + block.size - 4;
		else if (block.unk2 == 0)
			*blockSize = sizeof(Block_STRI) + block.size - 4;
		else
			*blockSize = sizeof(Block_STRI) + block.size;
		printf("\tFull block size: 0x%08llx\n", *blockSize);
	}
	break;
	case COOKIE("TYPE"):
	{
		auto getType = [](Block_TYPE::Type type)
		{
			switch (type)
			{
			default:
				return "<UNKNOWN>";
			}
		};
		Block_TYPE* block = reinterpret_cast<Block_TYPE*>(blockBlob);
		printf("Block_TYPE\n");
		printf("\ttype: 0x%08x - %s\n", block->type, getType(block->type));
		*blockSize = sizeof(Block_TYPE);
	}
	break;
	case COOKIE("TPGE"):
	{
		Block_TPGE* block = reinterpret_cast<Block_TPGE*>(blockBlob);
		printf("Block_TPGE\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);
		printf("\tunk3 = 0x%08x\n", block->unk3);
		printf("\tunk4 = 0x%08x\n", block->unk4);
		printf("\tunk5 = 0x%08x\n", block->unk5);
		//printf("\tunk6 = 0x%08x\n", block->unk6);

		assert(block->unk1 == 0xc);
		// XXX: Sometimes different
		//assert(block->unk2 == 0x01020102);
		//assert(block->unk3 == 0x1);
		// Probably a bitfield
		//assert(block->unk4 == 0x8);
		//assert(block->unk5 == 0x0 || block->unk5 == 0x45535054);
		//assert(block->unk6 == 0x0 || block->unk6 == 0x3c);
		*blockSize = sizeof(Block_TPGE);
	}
	break;
	case COOKIE("TPIB"):
	{
		Block_TPIB* block = reinterpret_cast<Block_TPIB*>(blockBlob);
		printf("Block_TPIB\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);
		printf("\tunk3 = 0x%08x\n", block->unk3);
		printf("\tunk4 = 0x%08x\n", block->unk4);

		// XXX: Sometimes different
		//assert(block->unk1 == 0x98);
		//assert(block->unk2 == 0x304);
		//assert(block->unk3 == 0x2);
		//assert(block->unk4 == 0x2);
		*blockSize = sizeof(Block_TPIB);
	}
	break;
	case COOKIE("TPSE"):
	{
		Block_TPSE* block = reinterpret_cast<Block_TPSE*>(blockBlob);
		printf("Block_TPSE\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);

		// XXX: Sometimes different
		//assert(block->unk1 == 0x40);
		*blockSize = sizeof(Block_TPSE);
	}
	break;
	case COOKIE("TPAR"):
	{
		Block_TPAR* block = reinterpret_cast<Block_TPAR*>(blockBlob);
		printf("Block_TPAR\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);

		assert(block->unk1 == 0x20);
		// XXX: Probably an array size
		//assert(block->unk2 == 0x18);

		*blockSize = sizeof(Block_TPAR);
	}
	break;
	case COOKIE("UBUF"):
	{
		Block_UBUF* block = reinterpret_cast<Block_UBUF*>(blockBlob);
		printf("Block_UBUF\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);
		printf("\tunk3 = 0x%08x\n", block->unk3);


		// XXX: sometimes different
		//assert(block->unk1 == 0x4);
		//assert(block->unk2 == 0x0);
		//assert(block->unk3 == 0x1);

		// XXX: unk2...?
		*blockSize = sizeof(Block_UBUF) + (block->unk2 ? 4 : 0);
	}
	break;
	case COOKIE("EBIN"):
	{
		Block_EBIN* block = reinterpret_cast<Block_EBIN*>(blockBlob);
		printf("Block_EBIN\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);
		printf("\tunk3 = 0x%08x\n", block->unk3);
		printf("\tunk4 = 0x%08x\n", block->unk4);
		printf("\tunk5 = 0x%08x\n", block->unk5);
		printf("\tunk6 = 0x%08x\n", block->unk6);

		// XXX: Sometimes different
		//assert(block->unk1 == 0xd4);
		assert(block->unk2 == 0x0);
		assert(block->unk3 == ~0U);
		//assert(block->unk4 == 0x0);
		assert(block->unk5 == 0x0);
		//assert(block->unk6 == ~0U);

		// XXX: unk4...?
		*blockSize = sizeof(Block_EBIN) + (block->unk4 ? -4 : 0);
	}
	break;
	case COOKIE("FSHA"):
	{
		Block_FSHA* block = reinterpret_cast<Block_FSHA*>(blockBlob);
		printf("Block_FSHA\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);
		printf("\tunk3 = 0x%08x\n", block->unk3);
		printf("\tunk4 = 0x%08x\n", block->unk4);
		printf("\tunk5 = 0x%08x\n", block->unk5);
		printf("\tunk6 = 0x%08x\n", block->unk6);
		printf("\tunk7 = 0x%08x\n", block->unk7);

		assert(block->unk1 == 0x18);
		assert(block->unk2 == 0x0);
		assert(block->unk3 == 0x0);
		assert(block->unk4 == 0x20);
		//assert(block->unk5 == 0x0);
		// XXX:Sometimes different
		// Probably a bitfield
		//assert(block->unk6 == 0x0);
		assert(block->unk7 == 0x0);
		*blockSize = sizeof(Block_FSHA);
	}
	break;
	case COOKIE("BFRE"):
	{
		Block_BFRE* block = reinterpret_cast<Block_BFRE*>(blockBlob);
		printf("Block_BFRE\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);

		// XXX: Sometimes different
		// Probably a bitfield
		//assert(block->unk1 == 0x10);
		assert(block->unk2 == 0x0);
		*blockSize = sizeof(Block_BFRE);
	}
	break;
	case COOKIE("SPDv"):
	{
		Block_SPDv* block = reinterpret_cast<Block_SPDv*>(blockBlob);
		printf("Block_SPDv\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);

		assert(block->unk1 == 0x4);
		assert(block->unk2 == 0x0);
		*blockSize = sizeof(Block_SPDv);
	}
	break;
	case COOKIE("SPDf"):
	{
		Block_SPDf* block = reinterpret_cast<Block_SPDf*>(blockBlob);
		printf("Block_SPDf\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);
		printf("\tunk3 = 0x%08x\n", block->unk3);

		assert(block->unk1 == 0x8);
		assert(block->unk2 == 0x0080003e);
		assert(block->unk3 == 0x0);
		*blockSize = sizeof(Block_SPDf);
	}
	break;
	case COOKIE("SPDc"):
	{
		Block_SPDc* block = reinterpret_cast<Block_SPDc*>(blockBlob);
		printf("Block_SPDc\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);

		assert(block->unk1 == 0x4);
		assert(block->unk2 == 0x0);
		*blockSize = sizeof(Block_SPDc);
	}
	break;

	case COOKIE("OBJC"):
	{
		Block_OBJC* block = reinterpret_cast<Block_OBJC*>(blockBlob);
		printf("Block_OBJC\n");
		printf("\tsize = 0x%08x\n", block->size);
		// XXX: Bunch of instructions here?

		DumpInstructions(blockBlob + 4, block->size);
		*blockSize = sizeof(Block_OBJC) + block->size;
	}
	break;
	case COOKIE("CFRA"):
	{
		Block_CFRA* block = reinterpret_cast<Block_CFRA*>(blockBlob);
		printf("Block_CFRA\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);

		assert(block->unk1 == 0x354);
		*blockSize = sizeof(Block_CFRA);
	}
	break;
	case COOKIE("BATT"):
	{
		Block_BATT* block = reinterpret_cast<Block_BATT*>(blockBlob);
		printf("Block_BATT\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);

		assert(block->unk1 == 0x38);
		assert(block->unk2 == 0x2);
		*blockSize = sizeof(Block_BATT);
	}
	break;
	case COOKIE("CCOM"):
	{
		Block_CCOM* block = reinterpret_cast<Block_CCOM*>(blockBlob);
		printf("Block_CCOM\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);

		// XXX: Probably a size
		// assert(block->unk1 == 0x180);
		*blockSize = sizeof(Block_CCOM);
	}
	break;
	case COOKIE("KERN"):
	{
		Block_KERN* block = reinterpret_cast<Block_KERN*>(blockBlob);
		printf("Block_KERN\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);

		assert(block->unk1 == 0x30);
		*blockSize = sizeof(Block_KERN);
	}
	break;
	case COOKIE("KWGS"):
	{
		Block_KWGS* block = reinterpret_cast<Block_KWGS*>(blockBlob);
		printf("Block_KWGS\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);

		assert(block->unk1 == 0xc);
		*blockSize = sizeof(Block_KWGS);
	}
	break;
	case COOKIE("RLOC"):
	{
		Block_RLOC* block = reinterpret_cast<Block_RLOC*>(blockBlob);
		printf("Block_RLOC\n");
		printf("\tunk1 = 0x%08x\n", block->unk1);
		printf("\tunk2 = 0x%08x\n", block->unk2);
		printf("\tunk3 = 0x%08x\n", block->unk3);
		printf("\tunk4 = 0x%08x\n", block->unk4);
		printf("\tunk5 = 0x%08x\n", block->unk5);
		printf("\tunk6 = 0x%08x\n", block->unk6);

		assert(block->unk1 == 0x10);
		assert(block->unk2 == 0x0);
		assert(block->unk3 == 0x0);
		assert(block->unk4 == 0x0);
		assert(block->unk5 == 0x8);
		// XXX: Sometimes different
		//assert(block->unk6 == 0x0);
		*blockSize = sizeof(Block_RLOC);
	}
	break;

	default:
	{
		auto getL = [cookie](int off)
		{
			return (uint8_t)(cookie >> off);
		};
		printf("Attempting 0x%08x: %c%c%c%c\n",
			cookie,
			getL(0), getL(8), getL(16), getL(24));
		return false;
	}
	break;
	};

	return true;
}

void ParseBlocks(std::vector<uint8_t> *fileData)
{
	uint8_t *data = fileData->data();
	size_t size = fileData->size();
	for (size_t i = 0; i < size;)
	{
		size_t blockSize = 0;
		if (!ParseSingleBlock(&data[i], &blockSize))
		{
			printf("Couldn't parse block! Leaving!\n");
			PrintFile(fileData, i - 128, i);
			return;
		}

		i += blockSize;
	}
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Usage: %s <file.bin>\n", argv[0]);
		return 0;
	}

	std::vector<uint8_t> file;
	if (ReadFile(&file, argv[1]))
	{
		PrintFile(&file);
		ParseBlocks(&file);
	}

	return 0;
}
