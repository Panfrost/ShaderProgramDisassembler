#include <assert.h>
#include <stdio.h>
#include <string>
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

void PrintFile(std::vector<uint8_t> *fileData, size_t offset = 0)
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
		printf("0x%08x: %c%c%c%c\n",
			word,
			getL(0), getL(8), getL(16), getL(24));
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
		assert(block->unk1 == 0x45c);
		*blockSize = sizeof(Block_VERT);
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
