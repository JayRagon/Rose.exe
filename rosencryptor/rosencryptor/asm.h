#pragma once

#include <Windows.h>
#include <iostream>

#include "zydis.h"


// if relative ptr with jmp or call, we need to insert an add [ptr], x instruction before the use, then a sub [ptr], x instruction after used
// otherwise jmp tables will be messed up
// and oh boy if it's in .rdata, just patch the value
// gl
// also if it's not a jmp or call
// don't do add/sub instructions

// this is horrible
// when injecting bytes
// we might need to alloc more room in .text

typedef struct _relative_instruction
{
	ZydisDecodedInstruction instruction;
	uint32_t offset;
	uint32_t size;
	uint32_t operand_offset;
	uint32_t operand_size;
	PIMAGE_SECTION_HEADER section;
	bool is_ptr, jmp; // jmp accounts for jz, jnz, jmp, call and so on
	uint32_t ptr;
	BYTE text[96]; // only use for debugging
} relative_instruction;

std::vector<relative_instruction> relative_list;

ZydisDecoder decoder;




// replace with zydis adding instructions and changing relative offsets
// make sure to also fix inline_asm.h after doing this

// probably remove this function
bool is_instruction_relative(const BYTE* bytes, uint32_t size = 16)
{
	ZydisDecoderContext context;
	ZydisDecodedInstruction instruction;

	if (ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&decoder, &context, bytes, size, &instruction)))
	{
		if (instruction.attributes & ZYDIS_ATTRIB_IS_RELATIVE)
		{
			std::cout << "RELATIVE\n\n\n";
			return true;
		}
	}
	else {
		std::cout << "is_instruction_relative bad\n";
	}
	return false;
}
/*
bool is_instruction_relative(const BYTE* bytes, uint32_t size = 16)
{
	ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
	ZydisDecoderContext context;
	ZydisDecodedInstruction instruction;

	if (ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&decoder, &context, bytes, size, &instruction))) 
	{
		// check if it's a jump instruction
		if (instruction.mnemonic >= ZYDIS_MNEMONIC_JB && instruction.mnemonic <= ZYDIS_MNEMONIC_JZ)
		{
			// for jump instructions, check the instruction attributes
			if (instruction.attributes & ZYDIS_ATTRIB_IS_RELATIVE)
			{
				std::cout << "RELATIVE (Jump)\n\n\n";
				return true;
			}
		}

		if (instruction.operand_count != 0)
		{
			if (ZYAN_SUCCESS(ZydisDecoderDecodeOperands(&decoder, &context, &instruction, operands, instruction.operand_count)))
			{
				for (uint8_t i = 0; i < instruction.operand_count; i++)
				{
					if (instruction.attributes & ZYDIS_ATTRIB_IS_RELATIVE) {
						std::cout << "this works too\n";
					}

					//printf("Operand %u: Type = %u\n", i, operands[i].type);
					if (operands[i].type == ZYDIS_OPERAND_TYPE_MEMORY && operands[i].mem.base == ZYDIS_REGISTER_RIP)
					{
						std::cout << "RELATIVE\n\n\n";
						return true;
					}

				}
			}
			else {
				std::cout << "is_instruction_relative bad\n";
			}
		}
	}
	else {
		std::cout << "is_instruction_relative bad\n";
	}
	return false;
}
*/

// reverses endian, then converts it to an int
int32_t operand2int32(const BYTE* operand)
{
	int32_t return_buffer = 0;
	for (size_t i = 0; i < 4; i++)
	{
		return_buffer += operand[i] << (i * 8);
	}
	return return_buffer;
}
BYTE* to_operand(int32_t integer)
{
	BYTE ret[4] = {};

	for (size_t i = 0; i < 4; i++)
	{
		ret[i] = (integer & (0xFF << (i * 8))) >> (i * 8);
	}

	return ret;
}

PIMAGE_SECTION_HEADER get_section(file_struct* file, uint32_t offset)
{
	for (size_t i = 0; i < file->section_count; i++)
	{
		if (offset >= (*file->sections)[i].VirtualAddress &&
			offset < (*file->sections)[i].VirtualAddress + (*file->sections)[i].Misc.VirtualSize) {
			return &((*file->sections)[i]);
		}
	}
	return nullptr;
}

// find all relative instructions
void find_relatives(file_struct* file)
{
	std::cout << std::hex;

	ZyanU64 runtime_address = file->text->PointerToRawData;

	BYTE* rawbytes = new BYTE[file->bytes.size()];
	
	for (size_t i = 0; i < file->bytes.size(); i++)
	{
		rawbytes[i] = file->bytes[i];
	}

	ZyanUSize offset = 0;
	ZydisDisassembledInstruction instruction;
	for (;;)
	{
		if (offset > file->text->SizeOfRawData) { break; }

		ZyanStatus status = ZydisDisassembleIntel(
			/* machine_mode:    */ ZYDIS_MACHINE_MODE_LONG_64,
			/* runtime_address: */ runtime_address,
			/* buffer:          */ rawbytes + offset + file->text->PointerToRawData,
			/* length:          */ file->text->SizeOfRawData - offset,
			/* instruction:     */ &instruction
		);

		if (ZYAN_SUCCESS(status)) 
		{
			//std::cout << instruction.text << '\n';

			ZydisDecoderContext context;
			ZydisDecodedInstruction decoded;

			if (ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&decoder, &context, 
				rawbytes + offset + file->text->PointerToRawData, file->text->SizeOfRawData - offset, &decoded)))
			{
				if (decoded.attributes & ZYDIS_ATTRIB_IS_RELATIVE)
				{
					ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
					bool is_ptr = false;
					bool is_relative = false;
					uint32_t relative_operand;

					if (ZYAN_SUCCESS(ZydisDecoderDecodeOperands(&decoder, &context, &decoded, operands, decoded.operand_count_visible)))
					{
						for (uint32_t i = 0; i < decoded.operand_count_visible; i++)
						{
							// jmp x instructions are imm, jmp [x] instructions are ptr
							switch (operands[i].type) {
							case ZYDIS_OPERAND_TYPE_IMMEDIATE:
								is_relative = operands[i].imm.is_relative;
								is_ptr = false;
								break;
							case ZYDIS_OPERAND_TYPE_MEMORY:
								is_relative = (operands[i].mem.base == ZYDIS_REGISTER_RIP);
								is_ptr = is_relative;
								break;
							}

							if (is_relative) {
								//std::cout << "operand " << i << "\n";
								relative_operand = i;
								break;
							}
						}
					}
					else {
						std::cout << "is_instruction_relative bad\n";
					}

					
					if (is_relative)
					{
						//std::cout << instruction.text << '\n';

						relative_instruction rel_buffer;
						rel_buffer.is_ptr = is_ptr;
						rel_buffer.jmp = decoded.meta.branch_type != ZYDIS_BRANCH_TYPE_NONE;
						rel_buffer.offset = offset + file->text->PointerToRawData;

						if (!is_ptr) {
							// only works for immediate values, because with ptrs it gives the access size
							rel_buffer.operand_size = (operands[relative_operand].size / 8);
						}
						else  {
							rel_buffer.operand_size = 4; // relative ptrs are always 4 bytes long
						}

						rel_buffer.operand_offset = decoded.length - rel_buffer.operand_size;
						rel_buffer.size = decoded.length;
						
						if (rel_buffer.is_ptr)
						{
							BYTE rel_operand[4] = {};
							for (size_t i = 0; i < rel_buffer.operand_size; i++)
							{
								rel_operand[i] = file->bytes[i + rel_buffer.offset + rel_buffer.operand_offset];
							}

							int32_t ptr = operand2int32(rel_operand);
							ptr += rel_buffer.offset + rel_buffer.size;

							// mem offset of int
							uint32_t mem_ptr = (ptr - file->text->PointerToRawData) + file->text->VirtualAddress;

							rel_buffer.section = get_section(file, mem_ptr);

							if (!rel_buffer.section)
							{
								//std::cout << "no section found\n";

								// go to next instruction, this instruction is probably not going to be executed
								offset += instruction.info.length;
								runtime_address += instruction.info.length;
								continue;
							}

							// convert to file offset again
							ptr = mem_ptr + rel_buffer.section->PointerToRawData - rel_buffer.section->VirtualAddress;
							//std::cout << "ptr 0x" << ptr << '\n';

							rel_buffer.ptr = ptr;
						}

						rel_buffer.instruction = decoded;

						memcpy(rel_buffer.text, instruction.text, 96);

						relative_list.push_back(rel_buffer);
					}
				}
			}

			offset += instruction.info.length;
			runtime_address += instruction.info.length;
		}
		else
		{
			//std::cout << "???" << '\n';
			offset += 1;
			runtime_address += 1;
		}
	}

	for (size_t i = 0; i < relative_list.size(); i++)
	{
		/*
		std::cout << relative_list[i].text << "\n"
			"size: " << relative_list[i].size << "\n" <<
			"offset: " << relative_list[i].offset << "\n" <<
			"operand offset: " << relative_list[i].operand_offset << "\n" <<
			"is ptr: " << relative_list[i].is_ptr << "\n\n";
		*/
	}

	delete[] rawbytes;
	std::cout << std::dec;
}

void zydis_init()
{
	ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
}

void assembly_init(file_struct* file)
{
	zydis_init();
	find_relatives(file);
}

std::vector<relative_instruction> find_affected(file_struct* file, uint32_t offset)
{
	std::vector<relative_instruction> ret;

	for (size_t i = 0; i < relative_list.size(); i++)
	{
		if ((relative_list[i].offset > offset && relative_list[i].ptr < offset) || // backward
			(relative_list[i].offset < offset && relative_list[i].ptr > offset))   // forward
		{
			ret.push_back(relative_list[i]);
		}
	}

	return ret;
}

// add check to see if we have enough .text space and if not alloc more
void text_inject(file_struct* file, uint32_t offset, const BYTE* text, uint32_t size)
{
	// shift over bytes
	for (int i = file->text->PointerToRawData + file->text->SizeOfRawData; i >= offset; i--)
	{
		file->bytes[i] = file->bytes[i - size];
	}

	// patch bytes
	for (size_t i = 0; i < size; i++)
	{
		file->bytes[offset + i] = text[i];
	}
}

// add/sub
void __inject_asm(file_struct* file, uint32_t offset, const char* assembly, uint32_t size)
{
	std::cout << std::hex;

	// directly patch .rdata addresses (UNTESTED)
	for (size_t i = 0; i < relative_list.size(); i++)
	{
		if (relative_list[i].jmp && relative_list[i].is_ptr && 
			_stricmp((const char*)relative_list[i].section->Name, ".rdata") == 0)
		{
			// read absolute ptr
			uint64_t abs_ptr = 0;
			for (size_t u = 0; u < 8; u++)
			{
				abs_ptr |= (uint64_t)file->bytes[relative_list[i].ptr + u] << (u * 8);
			}

			uint64_t mem_ptr = abs_ptr;
			abs_ptr -= file->base_addr;
			abs_ptr = abs_ptr + relative_list[i].section->PointerToRawData - relative_list[i].section->VirtualAddress;

			// if affected, patch ptr
			if (abs_ptr > offset) 
			{
				mem_ptr += size;
				for (size_t u = 0; u < 8; u++)
				{
					file->bytes[relative_list[i].ptr + u] = (mem_ptr & (0xFFULL << (u * 8))) >> (u * 8);
				}
			}
		}
	}

	auto affected = find_affected(file, offset);

	for (size_t i = 0; i < affected.size(); i++)
	{
		// only adjust if in .text
		if (_stricmp((const char*)relative_list[i].section->Name, ".text") == 0)
		{
			std::cout << affected[i].text << "\nOFFSET: 0x" << affected[i].offset << '\n';

			// only works for 4 bytes operands (NO SHORT JMPS)
			BYTE operand[4];
			for (size_t u = 0; u < 4; u++)
			{
				operand[u] = file->bytes[affected[i].offset + affected[i].operand_offset + u];
			}
			int32_t raw_ptr = operand2int32(operand);

			if (raw_ptr < 0) // backwards
			{
				raw_ptr -= size;
			}
			else // forwards
			{
				raw_ptr += size;
			}

			BYTE* new_operand = to_operand(raw_ptr);

			// patch operand
			for (size_t u = 0; u < 4; u++)
			{
				file->bytes[affected[i].offset + affected[i].operand_offset + u] = new_operand[u];
			}

			// change relative jmp and call addresses
			if (affected[i].jmp)
			{
				if (affected[i].is_ptr)
				{
					// .rdata is already patched
					if (_stricmp((const char*)(affected[i].section->Name), ".rdata") != 0)
					{
						// inject add/sub instructions

					}
				}
			}

			text_inject(file, offset, (const BYTE*)assembly, size);
		}
	}


	std::cout << std::dec;
}






