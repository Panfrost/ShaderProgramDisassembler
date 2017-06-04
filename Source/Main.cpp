#include <assert.h>
#include <stdarg.h>
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

void iprintf(unsigned indent, const char *format, ...)
{
	for (unsigned i = 0; i < indent; i++)
		printf("\t");
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

void DumpInstructions(unsigned indent, uint8_t* instBlob, uint32_t size)
{
	// Instructions are 64bits
	// Clause headers and tails are mainly 32bit
	// There can be 64bit clause headers, no idea when they are used
	uint8_t* instEnd = instBlob + size;
	while (instBlob != instEnd)
	{
		iprintf(indent, "0x%08x\n", *(uint32_t*)instBlob);
		instBlob += 4;
	}
}

bool PrintBlocks(unsigned indent, uint8_t *data, size_t size);

// Attempt to parse a single block with maxSize words
bool ParseSingleBlock(unsigned indent, uint8_t* blockBlob, size_t *blockSize)
{
	uint32_t cookie = *reinterpret_cast<uint32_t*>(blockBlob);
	switch (cookie)
	{
	case COOKIE("MPB1"):
	{
		Block_MPB1* block = reinterpret_cast<Block_MPB1*>(blockBlob);
		iprintf(indent, "Block_MPB1\n");
		indent++;
		iprintf(indent, "Size: 0x%08x\n", block->size);
		assert(block->unk1 == 2);
		assert(block->unk2 == 0);
		*blockSize = sizeof(Block_MPB1);
	}
	break;
	case COOKIE("VERT"):
	{
		Block_VERT* block = reinterpret_cast<Block_VERT*>(blockBlob);
		iprintf(indent, "Block_VERT\n");
		iprintf(indent, "unk1 = 0x%08x\n", block->unk1);

		assert(block->unk1 == 0x45c);
		*blockSize = sizeof(Block_VERT);
	}
	break;
	case COOKIE("FRAG"):
	{
		Block_FRAG* block = reinterpret_cast<Block_FRAG*>(blockBlob);
		iprintf(indent, "Block_FRAG\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);

		assert(block->unk1 == 0x37c);
		*blockSize = sizeof(Block_FRAG);
	}
	break;
	case COOKIE("COMP"):
	{
		Block_COMP* block = reinterpret_cast<Block_COMP*>(blockBlob);
		iprintf(indent, "Block_COMP\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);

		// XXX: Changes, probably a size
		//assert(block->unk1 == 0x1a8);
		*blockSize = sizeof(Block_COMP);
	}
	break;
	case COOKIE("MBS2"):
	{
		Block_MBS2* block = reinterpret_cast<Block_MBS2*>(blockBlob);
		iprintf(indent, "Block_MBS2\n");
		indent++;
		iprintf(indent, "size = 0x%08x\n", block->size);
		iprintf(indent, "version = 0x%08x\n", block->version);
		PrintBlocks(indent, blockBlob + sizeof(Block_MBS2),
			    block->size - 4); // 4 bytes for version field (?)

		*blockSize = block->size + 8; // 8 bytes for cookie + size
	}
	break;
	case COOKIE("VEHW"):
	{
		Block_VEHW* block = reinterpret_cast<Block_VEHW*>(blockBlob);
		iprintf(indent, "Block_VEHW\n");
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
		iprintf(indent, "Block_CVER\n");
		assert(block->unk1 == 0x434);
		*blockSize = sizeof(Block_CVER);
	}
	break;
	case COOKIE("CMMN"):
	{
		Block_CMMN* block = reinterpret_cast<Block_CMMN*>(blockBlob);
		iprintf(indent, "Block_CMMN\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);

		// XXX: Sometimes different
		//assert(block->unk1 == 0x42c);
		*blockSize = sizeof(Block_CMMN);
	}
	break;
	case COOKIE("VELA"):
	{
		Block_VELA* block = reinterpret_cast<Block_VELA*>(blockBlob);
		iprintf(indent, "Block_VELA\n");
		assert(block->unk1 == 0x4);
		assert(block->unk2 == 0x8);
		*blockSize = sizeof(Block_VELA);
	}
	break;
	case COOKIE("SSYM"):
	{
		Block_SSYM* block = reinterpret_cast<Block_SSYM*>(blockBlob);
		iprintf(indent, "Block_SSYM\n");
		iprintf(indent, "\tsize = 0x%08x\n", block->size);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);

		// XXX: Sometimes different
		//assert(block->unk2 == 0x2);
		// XXX: Skipping the entire SSYM because parsing of sub blocks(STRI) aren't complete
		*blockSize = sizeof(Block_SSYM) + block->size - 4;
	}
	break;
	case COOKIE("SYMB"):
	{
		Block_SYMB* block = reinterpret_cast<Block_SYMB*>(blockBlob);
		iprintf(indent, "Block_SYMB\n");
		iprintf(indent, "\tunk1: 0x%08x\n", block->unk1);
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

		iprintf(indent, "Block_STRI\n");
		iprintf(indent, "\tSize: 0x%08x\n", block.size);
		iprintf(indent, "\tName: %s\n", name);
		iprintf(indent, "\tunk1 = 0x%08x\n", block.unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block.unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block.unk3);
		iprintf(indent, "\tunk4 = 0x%08x\n", block.unk4);

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
		iprintf(indent, "\tFull block size: 0x%08llx\n", *blockSize);
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
		iprintf(indent, "Block_TYPE\n");
		iprintf(indent, "\ttype: 0x%08x - %s\n", block->type, getType(block->type));
		*blockSize = sizeof(Block_TYPE);
	}
	break;
	case COOKIE("TPGE"):
	{
		Block_TPGE* block = reinterpret_cast<Block_TPGE*>(blockBlob);
		iprintf(indent, "Block_TPGE\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);
		iprintf(indent, "\tunk4 = 0x%08x\n", block->unk4);
		iprintf(indent, "\tunk5 = 0x%08x\n", block->unk5);
		//iprintf(indent, "\tunk6 = 0x%08x\n", block->unk6);

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
		iprintf(indent, "Block_TPIB\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);
		iprintf(indent, "\tunk4 = 0x%08x\n", block->unk4);

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
		iprintf(indent, "Block_TPSE\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);

		// XXX: Sometimes different
		//assert(block->unk1 == 0x40);
		*blockSize = sizeof(Block_TPSE);
	}
	break;
	case COOKIE("TPAR"):
	{
		Block_TPAR* block = reinterpret_cast<Block_TPAR*>(blockBlob);
		iprintf(indent, "Block_TPAR\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);

		assert(block->unk1 == 0x20);
		// XXX: Probably an array size
		//assert(block->unk2 == 0x18);

		*blockSize = sizeof(Block_TPAR);
	}
	break;
	case COOKIE("UBUF"):
	{
		Block_UBUF* block = reinterpret_cast<Block_UBUF*>(blockBlob);
		iprintf(indent, "Block_UBUF\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);


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
		iprintf(indent, "Block_EBIN\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);
		iprintf(indent, "\tunk4 = 0x%08x\n", block->unk4);
		iprintf(indent, "\tunk5 = 0x%08x\n", block->unk5);
		iprintf(indent, "\tunk6 = 0x%08x\n", block->unk6);

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
		iprintf(indent, "Block_FSHA\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);
		iprintf(indent, "\tunk4 = 0x%08x\n", block->unk4);
		iprintf(indent, "\tunk5 = 0x%08x\n", block->unk5);
		iprintf(indent, "\tunk6 = 0x%08x\n", block->unk6);
		iprintf(indent, "\tunk7 = 0x%08x\n", block->unk7);

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
		iprintf(indent, "Block_BFRE\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);

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
		iprintf(indent, "Block_SPDv\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);

		assert(block->unk1 == 0x4);
		assert(block->unk2 == 0x0);
		*blockSize = sizeof(Block_SPDv);
	}
	break;
	case COOKIE("SPDf"):
	{
		Block_SPDf* block = reinterpret_cast<Block_SPDf*>(blockBlob);
		iprintf(indent, "Block_SPDf\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);

		assert(block->unk1 == 0x8);
		assert(block->unk2 == 0x0080003e);
		assert(block->unk3 == 0x0);
		*blockSize = sizeof(Block_SPDf);
	}
	break;
	case COOKIE("SPDc"):
	{
		Block_SPDc* block = reinterpret_cast<Block_SPDc*>(blockBlob);
		iprintf(indent, "Block_SPDc\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);

		assert(block->unk1 == 0x4);
		assert(block->unk2 == 0x0);
		*blockSize = sizeof(Block_SPDc);
	}
	break;

	case COOKIE("OBJC"):
	{
		Block_OBJC* block = reinterpret_cast<Block_OBJC*>(blockBlob);
		iprintf(indent, "Block_OBJC\n");
		iprintf(indent, "\tsize = 0x%08x\n", block->size);
		// XXX: Bunch of instructions here?

		DumpInstructions(indent + 1, blockBlob + 4, block->size);
		*blockSize = sizeof(Block_OBJC) + block->size;
	}
	break;
	case COOKIE("CFRA"):
	{
		Block_CFRA* block = reinterpret_cast<Block_CFRA*>(blockBlob);
		iprintf(indent, "Block_CFRA\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);

		assert(block->unk1 == 0x354);
		*blockSize = sizeof(Block_CFRA);
	}
	break;
	case COOKIE("BATT"):
	{
		Block_BATT* block = reinterpret_cast<Block_BATT*>(blockBlob);
		iprintf(indent, "Block_BATT\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);

		assert(block->unk1 == 0x38);
		assert(block->unk2 == 0x2);
		*blockSize = sizeof(Block_BATT);
	}
	break;
	case COOKIE("CCOM"):
	{
		Block_CCOM* block = reinterpret_cast<Block_CCOM*>(blockBlob);
		iprintf(indent, "Block_CCOM\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);

		// XXX: Probably a size
		// assert(block->unk1 == 0x180);
		*blockSize = sizeof(Block_CCOM);
	}
	break;
	case COOKIE("KERN"):
	{
		Block_KERN* block = reinterpret_cast<Block_KERN*>(blockBlob);
		iprintf(indent, "Block_KERN\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);

		assert(block->unk1 == 0x30);
		*blockSize = sizeof(Block_KERN);
	}
	break;
	case COOKIE("KWGS"):
	{
		Block_KWGS* block = reinterpret_cast<Block_KWGS*>(blockBlob);
		iprintf(indent, "Block_KWGS\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);

		assert(block->unk1 == 0xc);
		*blockSize = sizeof(Block_KWGS);
	}
	break;
	case COOKIE("RLOC"):
	{
		Block_RLOC* block = reinterpret_cast<Block_RLOC*>(blockBlob);
		iprintf(indent, "Block_RLOC\n");
		iprintf(indent, "\tunk1 = 0x%08x\n", block->unk1);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);
		iprintf(indent, "\tunk4 = 0x%08x\n", block->unk4);
		iprintf(indent, "\tunk5 = 0x%08x\n", block->unk5);
		iprintf(indent, "\tunk6 = 0x%08x\n", block->unk6);

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

bool PrintBlocks(unsigned indent, uint8_t *data, size_t size)
{
	for (size_t i = 0; i < size;)
	{
		size_t blockSize = 0;
		if (!ParseSingleBlock(indent, &data[i], &blockSize))
		{
			printf("Couldn't parse block! Leaving!\n");
			return false;
		}

		i += blockSize;
	}

	return true;
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
		PrintBlocks(0, file.data(), file.size());
	}

	return 0;
}
