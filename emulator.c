#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

const int MEMORY_SIZE = 65536;
const int WORD_MAX = 65535;

typedef enum
{
	INS_NOP = 0,
	INS_MOV = 1,
	INS_MOVI = 2,
	INS_ADD = 3,
	INS_ADDI = 4,
	INS_LDR = 5,
	INS_LDRI = 6,
	INS_STR = 7,
	INS_STRI = 8,
	INS_SHL = 9,
	INS_SHLI = 10,
	INS_SHR = 11,
	INS_SHRI = 12,
	INS_AND = 13,
	INS_ANDI = 14,
	INS_JMP = 15,
	INS_JMPI = 16,
	INS_PUSH = 17,
	INS_POP = 18,
	INS_SUB = 19,
	INS_SUBI = 20,
	INS_OR = 21,
	INS_ORI = 22,
	INS_MUL = 23,
	INS_MULI = 24,
	INS_XOR = 25,
	INS_XORI = 26,
	INS_ADC = 27,
	INS_ADCI = 28,
	INS_JZ = 29,
	INS_JZI = 30,
	INS_JNZ = 31,
	INS_JNZI = 32,
	INS_INT = 33,
	INS_IRQE = 34,
	INS_IRQD = 35,
	INS_IRET = 36,
	INS_PR = 37,
	INS_PW = 38,
	INS_CALL = 39,
	INS_CALLI = 40,
	INS_MHI = 41,
	INS_JC = 42,
	INS_JCI = 43,
	INS_JNC = 44,
	INS_JNCI = 45,
	INS_CMP = 46,
	INS_CMPI = 47,
} Instruction;

typedef enum
{
	FLAG_OVERFLOW = 1,
	FLAG_ZERO = 2
} Flags;

typedef struct
{
	uint16_t *memory;
	uint16_t reg_file[16];
	uint16_t pc;
	uint16_t flags;
} EmulatorCtx;

EmulatorCtx ectx_new()
{
	EmulatorCtx ctx = (EmulatorCtx)
	{
		.memory = malloc(sizeof(uint16_t) * MEMORY_SIZE)
	};
	return ctx;
}

void ectx_free(EmulatorCtx *ctx)
{
	free(ctx->memory);
}

void emulate(EmulatorCtx *ctx)
{
	while(1)
	{
		uint16_t instruction = ctx->memory[ctx->pc];
		Instruction ins = (Instruction)((instruction & 0xFF00) >> 8);
		char hi_value = (char)((instruction & 0xF0) >> 4);
		char lo_value = (char)(instruction & 0xF);
		uint8_t byte_value = (char)(instruction & 0xFF);
		uint8_t hi_reg = hi_value;
		uint8_t lo_reg = lo_value;
		if(hi_reg > 15)
			hi_reg = 0;
		if(lo_reg > 15)
			lo_reg = 0;

		switch(ins)
		{
			case INS_MOV:
				ctx->reg_file[hi_reg] = ctx->reg_file[lo_reg];
				ctx->pc++;
				continue;
			case INS_MOVI:
				ctx->reg_file[0] = byte_value;
				ctx->pc++;
				continue;
			case INS_ADD:
				if(ctx->reg_file[hi_reg] + ctx->reg_file[lo_reg] < ctx->reg_file[hi_reg])
					ctx->flags |= FLAG_OVERFLOW;
				else
					ctx->flags &= ~FLAG_OVERFLOW;
				ctx->reg_file[hi_reg] += ctx->reg_file[lo_reg];
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_ADDI:
				if(ctx->reg_file[0] + byte_value < ctx->reg_file[0])
					ctx->flags |= FLAG_OVERFLOW;
				else
					ctx->flags &= ~FLAG_OVERFLOW;
				ctx->reg_file[0] += byte_value;
				if(ctx->reg_file[0] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_LDR:
				ctx->reg_file[hi_reg] = ctx->memory[ctx->reg_file[lo_reg]];
				ctx->pc++;
				continue;
			case INS_LDRI:
				ctx->reg_file[0] = ctx->memory[byte_value];
				ctx->pc++;
				continue;
			case INS_STR:
				ctx->memory[ctx->reg_file[hi_reg]] = ctx->reg_file[lo_reg];
				ctx->pc++;
				continue;
			case INS_STRI:
				ctx->memory[byte_value] = ctx->reg_file[0];
				ctx->pc++;
				continue;
			case INS_SHL:
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] << ctx->reg_file[lo_reg];
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_SHLI:
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] << lo_value;
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_SHR:
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] >> ctx->reg_file[lo_reg];
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_SHRI:
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] >> lo_value;
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_AND:
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] & ctx->reg_file[lo_reg];
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_ANDI:
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] & byte_value;
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_JMP:
				ctx->pc = ctx->reg_file[hi_reg];
				continue;
			case INS_JMPI:
				ctx->pc = byte_value;
				continue;
			case INS_PUSH:
				ctx->memory[ctx->reg_file[8]] = ctx->reg_file[hi_reg];
				ctx->reg_file[8]++;
				ctx->pc++;
				continue;
			case INS_POP:
				ctx->reg_file[8]--;
				ctx->reg_file[hi_reg] = ctx->memory[ctx->reg_file[8]];
				ctx->pc++;
				continue;
			case INS_SUB:
				if(ctx->reg_file[hi_reg] - ctx->reg_file[lo_reg] > ctx->reg_file[hi_reg])
					ctx->flags |= FLAG_OVERFLOW;
				else
					ctx->flags &= ~FLAG_OVERFLOW;
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] - ctx->reg_file[lo_reg];
				if(ctx->reg_file[hi_reg])
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_SUBI:
				if(ctx->reg_file[hi_reg] - byte_value > ctx->reg_file[hi_reg])
					ctx->flags |= FLAG_OVERFLOW;
				else
					ctx->flags &= ~FLAG_OVERFLOW;
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] - byte_value;
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_OR:
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] | ctx->reg_file[lo_reg];
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_ORI:
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] | byte_value;
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_MUL:
				if(ctx->reg_file[hi_reg] * ctx->reg_file[lo_reg] < ctx->reg_file[hi_reg])
					ctx->flags |= FLAG_OVERFLOW;
				else
					ctx->flags &= ~FLAG_OVERFLOW;
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] * ctx->reg_file[lo_reg];
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_MULI:
				if(ctx->reg_file[hi_reg] * byte_value < ctx->reg_file[hi_reg])
					ctx->flags |= FLAG_OVERFLOW;
				else
					ctx->flags &= ~FLAG_OVERFLOW;
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] * byte_value;
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_XOR:
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] ^ ctx->reg_file[lo_reg];
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;
			case INS_XORI:
				ctx->reg_file[hi_reg] = ctx->reg_file[hi_reg] ^ byte_value;
				if(ctx->reg_file[hi_reg] == 0)
					ctx->flags |= FLAG_ZERO;
				else
					ctx->flags &= ~FLAG_ZERO;
				ctx->pc++;
				continue;

			default:
				ctx->pc++;
				continue;
		}
	}
}

int main(int argc, const char **argv)
{
	if(argc < 2)
	{
		puts("Missing filepath");
		return 1;
	}
	FILE *file = fopen(argv[1], "r");
	if(!file)
	{
		printf("Failed to open file %s\n", argv[1]);
		return 1;
	}

	fseek(file, 0, SEEK_END);
	long buffer_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	if(buffer_size == 0)
	{
		puts("Input file empty");
		return 1;
	}

	char *file_buffer = malloc(sizeof(char) * (buffer_size + 1));
	size_t read_bytes = fread(file_buffer, sizeof(char), buffer_size, file);
	file_buffer[read_bytes] = 0;
	fclose(file);

	EmulatorCtx ctx = ectx_new();

	char *next = file_buffer;
	for(int i = 0; i < MEMORY_SIZE; i++)
	{
		if(next >= &file_buffer[read_bytes - 1])
			break;
		long value = strtol(next, &next, 16);
		ctx.memory[i] = (uint16_t)value;
	}
	free(file_buffer);

	ectx_free(&ctx);
}
