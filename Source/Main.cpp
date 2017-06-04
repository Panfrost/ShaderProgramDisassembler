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
	size_t size = fileData->size() / 4;
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

void iprintf(unsigned indent, const char *format, ...) __attribute__((format(printf, 2, 3)));

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
		printf("0x%08x\n", *(uint32_t*)instBlob);
		instBlob += 4;
	}
}

bool PrintBlocks(unsigned indent, uint8_t *data, size_t size);
bool PrintBlock(unsigned indent, uint8_t **data);

// Attempt to parse a single block with maxSize words
bool ParseSingleBlock(unsigned indent, uint8_t* blockBlob, uint32_t cookie, uint32_t size)
{
	switch (cookie)
	{
	case COOKIE("MPB1"):
	{
		Block_MPB1* block = reinterpret_cast<Block_MPB1*>(blockBlob);
		assert(block->unk2 == 0);
	}
	break;
	case COOKIE("VERT"):
		break;
	case COOKIE("FRAG"):
		break;
	case COOKIE("COMP"):
		break;
	case COOKIE("MBS2"):
	{
		Block_MBS2* block = reinterpret_cast<Block_MBS2*>(blockBlob);
		iprintf(indent, "\tversion = 0x%08x\n", block->version);
		PrintBlocks(indent + 1, blockBlob + sizeof(Block_MBS2),
			    size - sizeof(Block_MBS2));
	}
	break;
	case COOKIE("VEHW"):
	{
		Block_VEHW* block = reinterpret_cast<Block_VEHW*>(blockBlob);
		assert(block->unk2 == 0xb);
		assert(block->unk3 == 0x0);
		assert(block->unk4 == 0x0);
	}
	break;
	case COOKIE("CVER"):
		PrintBlocks(indent + 1, blockBlob, size);
		break;
	case COOKIE("CMMN"):
	{
		assert(*reinterpret_cast<uint32_t*>(blockBlob) == COOKIE("VELA"));
		PrintBlock(indent + 1, &blockBlob);

		// symbol tables?
		for (int i = 0; i < 6; i++) {
			assert(*reinterpret_cast<uint32_t*>(blockBlob) == COOKIE("SSYM"));
			PrintBlock(indent + 1, &blockBlob);
		}
		assert(*reinterpret_cast<uint32_t*>(blockBlob) == COOKIE("UBUF"));
		PrintBlock(indent + 1, &blockBlob);

		uint32_t numBinaries = *reinterpret_cast<uint32_t*>(blockBlob);
		iprintf(indent, "\tbinaries = %u\n", numBinaries);
		blockBlob += sizeof(uint32_t);

		for (unsigned i = 0; i < numBinaries; i++)
		{
			assert(*reinterpret_cast<uint32_t*>(blockBlob) == COOKIE("EBIN"));
			PrintBlock(indent + 1, &blockBlob);
		}
	}
	break;
	case COOKIE("VELA"):
	{
		Block_VELA* block = reinterpret_cast<Block_VELA*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		//assert(block->unk2 == 0x8);
	}
	break;
	case COOKIE("SSYM"):
	{
		Block_SSYM* block = reinterpret_cast<Block_SSYM*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);

		// XXX: Sometimes different
		//assert(block->unk2 == 0x2);
		// XXX: Skipping the entire SSYM because parsing of sub blocks(STRI) aren't complete
	}
	break;
	case COOKIE("SYMB"):
		break;
	case COOKIE("STRI"):
	{
		// block.size includes the word aligned zero padding for the string
		char* name = reinterpret_cast<char*>(blockBlob);
		iprintf(indent + 1, "%s", name);
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
		iprintf(indent, "\ttype: 0x%08x - %s\n", block->type, getType(block->type));
	}
	break;
	case COOKIE("TPGE"):
	{
		Block_TPGE* block = reinterpret_cast<Block_TPGE*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);
		iprintf(indent, "\tunk4 = 0x%08x\n", block->unk4);
		iprintf(indent, "\tunk5 = 0x%08x\n", block->unk5);
		//iprintf(indent, "\tunk6 = 0x%08x\n", block->unk6);

		// XXX: Sometimes different
		//assert(block->unk2 == 0x01020102);
		//assert(block->unk3 == 0x1);
		// Probably a bitfield
		//assert(block->unk4 == 0x8);
		//assert(block->unk5 == 0x0 || block->unk5 == 0x45535054);
		//assert(block->unk6 == 0x0 || block->unk6 == 0x3c);
	}
	break;
	case COOKIE("TPIB"):
	{
		Block_TPIB* block = reinterpret_cast<Block_TPIB*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);
		iprintf(indent, "\tunk4 = 0x%08x\n", block->unk4);

		// XXX: Sometimes different
		//assert(block->unk2 == 0x304);
		//assert(block->unk3 == 0x2);
		//assert(block->unk4 == 0x2);
	}
	break;
	case COOKIE("TPSE"):
		break;
	case COOKIE("TPAR"):
	{
		Block_TPAR* block = reinterpret_cast<Block_TPAR*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);

		// XXX: Probably an array size
		//assert(block->unk2 == 0x18);
	}
	break;
	case COOKIE("UBUF"):
	{
		Block_UBUF* block = reinterpret_cast<Block_UBUF*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);


		// XXX: sometimes different
		//assert(block->unk2 == 0x0);

		// XXX: unk2...?
	}
	break;
	case COOKIE("EBIN"):
	{
		Block_EBIN* block = reinterpret_cast<Block_EBIN*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);
		iprintf(indent, "\tunk4 = 0x%08x\n", block->unk4);
		iprintf(indent, "\tunk5 = 0x%08x\n", block->unk5);
		iprintf(indent, "\tunk6 = 0x%08x\n", block->unk6);

		// XXX: Sometimes different
		//assert(block->unk1 == 0xd4);
		//assert(block->unk2 == 0x0);
		assert(block->unk3 == ~0U);
		//assert(block->unk4 == 0x0);
		assert(block->unk5 == 0x0);
		//assert(block->unk6 == ~0U);

		// XXX: unk4...?
		blockBlob += sizeof(Block_EBIN);
		PrintBlocks(indent + 1, blockBlob, size - sizeof(Block_EBIN));

	}
	break;
	case COOKIE("FSHA"):
	{
		Block_FSHA* block = reinterpret_cast<Block_FSHA*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);
		iprintf(indent, "\tunk4 = 0x%08x\n", block->unk4);
		iprintf(indent, "\tunk5 = 0x%08x\n", block->unk5);
		iprintf(indent, "\tunk6 = 0x%08x\n", block->unk6);
		iprintf(indent, "\tunk7 = 0x%08x\n", block->unk7);

		assert(block->unk2 == 0x0);
		assert(block->unk3 == 0x0);
		//assert(block->unk4 == 0x20);
		//assert(block->unk5 == 0x0);
		// XXX:Sometimes different
		// Probably a bitfield
		//assert(block->unk6 == 0x0);
		assert(block->unk7 == 0x0);
	}
	break;
	case COOKIE("BFRE"):
	{
		Block_BFRE* block = reinterpret_cast<Block_BFRE*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);

		// XXX: Sometimes different
		// Probably a bitfield
		assert(block->unk2 == 0x0);
	}
	break;
	case COOKIE("SPDv"):
	{
		Block_SPDv* block = reinterpret_cast<Block_SPDv*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);

		assert(block->unk2 == 0x0);
	}
	break;
	case COOKIE("SPDf"):
	{
		Block_SPDf* block = reinterpret_cast<Block_SPDf*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);

		assert(block->unk2 == 0x0080003e);
		assert(block->unk3 == 0x0);
	}
	break;
	case COOKIE("SPDc"):
	{
		Block_SPDc* block = reinterpret_cast<Block_SPDc*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);

		assert(block->unk2 == 0x0);
	}
	break;

	case COOKIE("OBJC"):
	{
		DumpInstructions(indent + 1, blockBlob, size);
	}
	break;
	case COOKIE("CFRA"):
	{
		PrintBlocks(indent + 1, blockBlob, size);
	}
	break;
	case COOKIE("BATT"):
	{
		Block_BATT* block = reinterpret_cast<Block_BATT*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);

		assert(block->unk2 == 0x2);
	}
	break;
	case COOKIE("CCOM"):
		break;
	case COOKIE("KERN"):
		break;
	case COOKIE("KWGS"):
		break;
	case COOKIE("RLOC"):
	{
		Block_RLOC* block = reinterpret_cast<Block_RLOC*>(blockBlob);
		iprintf(indent, "\tunk2 = 0x%08x\n", block->unk2);
		iprintf(indent, "\tunk3 = 0x%08x\n", block->unk3);
		iprintf(indent, "\tunk4 = 0x%08x\n", block->unk4);
		iprintf(indent, "\tunk5 = 0x%08x\n", block->unk5);
		iprintf(indent, "\tunk6 = 0x%08x\n", block->unk6);

		assert(block->unk2 == 0x0);
		assert(block->unk3 == 0x0);
		assert(block->unk4 == 0x0);
		assert(block->unk5 == 0x8);
		// XXX: Sometimes different
		//assert(block->unk6 == 0x0);
	}
	break;
	case COOKIE("FOTV"):
	{
		// output variables
		uint32_t numVariables = *reinterpret_cast<uint32_t*>(blockBlob);
		blockBlob += sizeof(uint32_t);
		iprintf(indent, "\tvariables = %u\n", numVariables);

		for (unsigned i = 0; i < numVariables; i++)
		{
			assert(*reinterpret_cast<uint32_t*>(blockBlob) == COOKIE("OUTV"));
			PrintBlock(indent + 1, &blockBlob);
		}
	}
	case COOKIE("OUTV"):
		//TODO
		break;
	case COOKIE("AINF"):
		//TODO
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

bool PrintBlock(unsigned indent, uint8_t **data)
{
	Header *hdr = reinterpret_cast<Header*>(*data);
	auto getL = [hdr](int off)
	{
		return (uint8_t)(hdr->cookie >> off);
	};
	iprintf(indent, "%c%c%c%c\n", getL(0), getL(8), getL(16), getL(24));
	iprintf(indent, "\tsize = 0x%08x\n", hdr->size);
	if (!ParseSingleBlock(indent, *data + sizeof(Header), hdr->cookie, hdr->size))
	{
		printf("Couldn't parse block! Leaving!\n");
		return false;
	}

	*data += hdr->size + sizeof(Header);
	return true;
}

bool PrintBlocks(unsigned indent, uint8_t *data, size_t size)
{
	for (uint8_t *tmp = data; tmp < data + size;)
	{
		if (!PrintBlock(indent, &tmp))
			return false;
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
