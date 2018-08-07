#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "lib/xalloc.h"
#include "lib/stack.h"
#include "lib/contracts.h"
#include "lib/c0v_stack.h"
#include "lib/c0vm.h"
#include "lib/c0vm_c0ffi.h"
#include "lib/c0vm_abort.h"

/* call stack frames */
typedef struct frame_info frame;
struct frame_info {
	c0v_stack_t S;	/* Operand stack of C0 values */
	ubyte *P;		/* Function body */
	size_t pc;		/* Program counter */
	c0_value *V;	/* The local variables */
};

int pop_int(c0v_stack_t S) {
	return val2int(c0v_pop(S));
}

void push_int(c0v_stack_t S, int i) {
	c0v_push(S, int2val(i));
}

void *pop_ptr(c0v_stack_t S) {
	return val2ptr(c0v_pop(S));
}

void push_ptr(c0v_stack_t S, void *ptr) {
	c0v_push(S, ptr2val(ptr));
}

uint16_t get_index(uint8_t arg0, uint8_t arg1) {
	uint16_t arg0_32 = arg0;
	uint16_t arg1_32 = arg1;
	arg0_32 = arg0_32 << 8;
	return arg0_32 | arg1_32;
}

size_t pc_goto(size_t pc, int8_t arg0, int8_t arg1) {
	int32_t arg0_32 = arg0;
	int32_t arg1_32 = arg1;
	arg0_32 = arg0_32 << 8;
	int32_t pc_inc = arg0_32 | arg1_32;
	return pc + pc_inc;
}

int execute(struct bc0_file *bc0) {
	REQUIRES(bc0 != NULL);

	/* Variables */
	c0v_stack_t S = c0v_stack_new();	/* Operand stack of C0 values */
	ubyte *P = bc0->function_pool[0].code;	/* Array of bytes that make up the current function */
	size_t pc = 0;	/* Current location within the current byte array P */
	c0_value *V = xmalloc(sizeof(c0_value) * (bc0->function_pool[0].num_vars));	/* Local variables (you won't need this till Task 2) */
	(void) V;

	/* The call stack, a generic stack that should contain pointers to frames */
	/* You won't need this until you implement functions. */
	gstack_t callStack = stack_new();
	(void) callStack;

	while (true) {

		#ifdef DEBUG
		/* You can add extra debugging information here */
		fprintf(stderr, "Opcode 0x%02X -- Stack size: %zu -- PC: %zu\t", P[pc], c0v_stack_size(S), pc);
		#endif

		switch (P[pc]) {

		/* Additional stack operation: */

		case POP: {
			pc++;
			c0v_pop(S);
			break;
		}

		case DUP: {
			pc++;
			c0_value v = c0v_pop(S);
			c0v_push(S,v);
			c0v_push(S,v);
			break;
		}

		case SWAP: {
			pc++;
			c0_value x = c0v_pop(S);
			c0_value y = c0v_pop(S);
			c0v_push(S, x);
			c0v_push(S, y);
			break;
		}


		/* Returning from a function.
		 * This currently has a memory leak! You will need to make a slight
		 * change for the initial tasks to avoid leaking memory.	You will
		 * need to be revise it further when you write INVOKESTATIC. */

		case RETURN: {
			c0_value retval = c0v_pop(S);
			assert(c0v_stack_empty(S));
			c0v_stack_free(S);
			free(V);
			if (!stack_empty(callStack)) {
				frame *return_function = (frame*)pop(callStack);
				S = return_function->S;
				P = return_function->P;
				pc = return_function->pc;
				V = return_function->V;
				free(return_function);
				c0v_push(S, retval);
				#ifdef DEBUG
				fprintf(stderr, "Returning 0x%X\n", val2int(retval));
				#endif
				break;
			}
			else {
				#ifdef DEBUG
				fprintf(stderr, "Returning 0x%X from main\n", val2int(retval));
				#endif
				stack_free(callStack, NULL);
				return val2int(retval);
			}
		}


		/* Arithmetic and Logical operations */

		case IADD: {
			pc++;
			int32_t b = pop_int(S);
			int32_t a = pop_int(S);
			int32_t result = a + b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X + 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, result);
			break;
		}

		case ISUB: {
			pc++;
			int32_t b = pop_int(S);
			int32_t a = pop_int(S);
			int32_t result = a - b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X - 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, result);
			break;
		}

		case IMUL: {
			pc++;
			int32_t b = pop_int(S);
			int32_t a = pop_int(S);
			int32_t result = a * b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X * 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, result);
			break;
		}

		case IDIV: {
			pc++;
			int32_t b = pop_int(S);
			int32_t a = pop_int(S);
			if (a == INT_MIN && b == -1)
				c0_arith_error("INT_MIN divided by -1");
			else if (b == 0)
				c0_arith_error("Division by zero");
			int32_t result = a / b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X / 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, result);
			break;
		}

		case IREM: {
			pc++;
			int32_t b = pop_int(S);
			int32_t a = pop_int(S);
			if (a == INT_MIN && b == -1)
				c0_arith_error("INT_MIN divided by -1");
			else if (b == 0)
				c0_arith_error("Division by zero");
			int32_t result = a % b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X mod 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, result);
			break;
		}

		case IAND: {
			pc++;
			uint32_t b = pop_int(S);
			uint32_t a = pop_int(S);
			uint32_t result = a & b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X & 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, (int32_t)result);
			break;
		}

		case IOR: {
			pc++;
			uint32_t b = pop_int(S);
			uint32_t a = pop_int(S);
			uint32_t result = a | b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X | 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, (int32_t)result);
			break;
		}

		case IXOR: {
			pc++;
			uint32_t b = pop_int(S);
			uint32_t a = pop_int(S);
			uint32_t result = a ^ b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X ^ 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, (int32_t)result);
			break;
		}

		case ISHL: {
			pc++;
			int32_t b = pop_int(S);
			int32_t a = pop_int(S);
			if (b < 0 || b > 31)
				c0_arith_error("Invalid number of bits to shift left");
			int32_t result = a << b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X << 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, result);
			break;
		}

		case ISHR: {
			pc++;
			int32_t b = pop_int(S);
			int32_t a = pop_int(S);
			if (b < 0 || b > 31)
				c0_arith_error("Invalid number of bits to shift right");
			int32_t result = a >> b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X >> 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, result);
			break;
		}


		/* Pushing constants */

		case BIPUSH: {
			#ifdef DEBUG
			fprintf(stderr, "Bipushing 0x%X onto stack\n", P[pc+1]);
			#endif
			int8_t signed_byte = P[pc+1];
			push_int(S, (int32_t)signed_byte);
			pc += 2;
			break;
		}

		case ILDC: {
			#ifdef DEBUG
			fprintf(stderr, "Pushing 0x%X onto stack from int_pool[0x%X]\n", bc0->int_pool[ get_index(P[pc+1], P[pc+2]) ], get_index(P[pc+1], P[pc+2]));
			#endif
			push_int(S, bc0->int_pool[ get_index(P[pc+1], P[pc+2]) ]);
			pc += 3;
			break;
		}

		case ALDC: {
			#ifdef DEBUG
			fprintf(stderr, "Pushing a string onto stack from string_pool[0x%X]\n", get_index(P[pc+1], P[pc+2]));
			#endif
			push_ptr(S, &(bc0->string_pool[ get_index(P[pc+1], P[pc+2]) ]));
			pc += 3;
			break;
		}

		case ACONST_NULL: {
			pc++;
			#ifdef DEBUG
			fprintf(stderr, "\n");
			#endif
			push_ptr(S, NULL);
			break;
		}


		/* Operations on local variables */

		case VLOAD: {
			c0v_push(S, V[P[pc+1]]);
			#ifdef DEBUG
//			fprintf(stderr, "Got 0x%X from V[0x%X], pushing onto stack\n", val2int(V[P[pc+1]]), P[pc+1]);
			fprintf(stderr, "Got a c0_value from V[0x%X], pushing onto stack\n", P[pc+1]);
			#endif
			pc += 2;
			break;
		}

		case VSTORE: {
			V[P[pc+1]] = c0v_pop(S);
			#ifdef DEBUG
//			fprintf(stderr, "Popped 0x%X from stack, stored into V[0x%X]\n", val2int(V[P[pc+1]]), P[pc+1]);
			fprintf(stderr, "Popped a c0_value from stack, stored into V[0x%X]\n", P[pc+1]);
			#endif
			pc += 2;
			break;
		}


		/* Control flow operations */

		case NOP: {
			#ifdef DEBUG
			fprintf(stderr, "\n");
			#endif
			pc++;
			break;
		}

		case IF_CMPEQ: {
			#ifdef DEBUG
			fprintf(stderr, "\n");
			#endif
			c0_value a = c0v_pop(S);
			c0_value b = c0v_pop(S);
			if (val_equal(a, b))
				pc = pc_goto(pc, P[pc+1], P[pc+2]);
			else
				pc += 3;
			break;
		}

		case IF_CMPNE: {
			#ifdef DEBUG
			fprintf(stderr, "\n");
			#endif
			c0_value a = c0v_pop(S);
			c0_value b = c0v_pop(S);
			if (!val_equal(a, b))
				pc = pc_goto(pc, P[pc+1], P[pc+2]);
			else
				pc += 3;
			break;
		}

		case IF_ICMPLT: {
			#ifdef DEBUG
			fprintf(stderr, "\n");
			#endif
			int y = pop_int(S);
			int x = pop_int(S);
			if (x < y)
				pc = pc_goto(pc, P[pc+1], P[pc+2]);
			else
				pc += 3;
			break;
		}

		case IF_ICMPGE: {
			#ifdef DEBUG
			fprintf(stderr, "\n");
			#endif
			int y = pop_int(S);
			int x = pop_int(S);
			if (x >= y)
				pc = pc_goto(pc, P[pc+1], P[pc+2]);
			else
				pc += 3;
			break;
		}

		case IF_ICMPGT: {
			#ifdef DEBUG
			fprintf(stderr, "\n");
			#endif
			int y = pop_int(S);
			int x = pop_int(S);
			if (x > y)
				pc = pc_goto(pc, P[pc+1], P[pc+2]);
			else
				pc += 3;
			break;
		}

		case IF_ICMPLE: {
			#ifdef DEBUG
			fprintf(stderr, "\n");
			#endif
			int y = pop_int(S);
			int x = pop_int(S);
			if (x <= y)
				pc = pc_goto(pc, P[pc+1], P[pc+2]);
			else
				pc += 3;
			break;
		}

		case GOTO: {
			#ifdef DEBUG
			fprintf(stderr, "Going to PC + 0x%X\n", (unsigned int)(pc_goto(pc, P[pc+1], P[pc+2]) - pc) );
			#endif
			pc = pc_goto(pc, P[pc+1], P[pc+2]);
			break;
		}

		case ATHROW: {
			c0_user_error((char*)pop_ptr(S));
			pc++;
			break;
		}

		case ASSERT: {
			void *error_msg = pop_ptr(S);
			int x = pop_int(S);
			if (x == 0) {
				c0_assertion_failure((char*)error_msg);
				pc++;
			}
			else {
				pc++;
			}
			break;
		}


		/* Function call operations: */

		case INVOKESTATIC: {
			
			uint16_t function_index = get_index(P[pc+1], P[pc+2]);
			
			#ifdef DEBUG
			fprintf(stderr, "Invoking static function 0x%X\n", function_index);
			#endif
			
			uint16_t argc = bc0->function_pool[function_index].num_args;
			int i;
			frame *current_function = xmalloc(sizeof(frame));
			current_function->S = S;
			current_function->P = P;
			current_function->pc = pc + 3;
			current_function->V = V;
			push(callStack, current_function);
			
			S = c0v_stack_new();
			P = bc0->function_pool[function_index].code;
			pc = 0;
			V = xmalloc(sizeof(c0_value) * (bc0->function_pool[function_index].num_vars));
			for (i = argc - 1; i >= 0; i--) {
				V[i] = c0v_pop(current_function->S);
//				#ifdef DEBUG
//				fprintf(stderr, "Loading arguments from calling function to V[0x%X]\n", i-1);
//				#endif
			}
			
			break;
		}

		case INVOKENATIVE: {
			uint16_t function_index = get_index(P[pc+1], P[pc+2]);
			
			#ifdef DEBUG
			fprintf(stderr, "Invoking native function 0x%X\n", function_index);
			#endif
			
			uint16_t argc = bc0->native_pool[function_index].num_args;
			c0_value args[argc];
			int i;
			for (i = argc - 1; i >= 0; i--) {
				args[i] = c0v_pop(S);
			}
			
			c0_value native_retval = native_function_table[bc0->native_pool[function_index].function_table_index](args);
			c0v_push(S, native_retval);
			
			pc += 3;
			break;
		}


		/* Memory allocation operations: */

		case NEW: {
			
			#ifdef DEBUG
			fprintf(stderr, "Allocating 0x%X bytes of memory\n", P[pc+1]);
			#endif
			
			void *new_ptr = xcalloc(sizeof(char), P[pc+1]);
			push_ptr(S, new_ptr);
			pc += 2;
			break;
		}

		case NEWARRAY: {
			
			int array_length = pop_int(S);
			
			#ifdef DEBUG
			fprintf(stderr, "Allocating an array of length 0x%X, element size 0x%X bytes\n", array_length, P[pc+1]);
			#endif
			
			c0_array *array = xmalloc(sizeof(c0_array));
			array->count = array_length;
			array->elt_size = P[pc+1];
			array->elems = xcalloc(P[pc+1], array_length);
			
			push_ptr(S, array);
			
			pc += 2;
			break;
		}

		case ARRAYLENGTH: {
			
			#ifdef DEBUG
			fprintf(stderr, "Getting the array length: ");
			#endif
			
			c0_array *array = (c0_array*)pop_ptr(S);
			if (array == NULL)
				c0_memory_error("Segmentation fault (attempt to dereference a NULL pointer)");
			
			#ifdef DEBUG
			fprintf(stderr, "0x%X\n", array->count);
			#endif
			
			push_int(S, array->count);
			
			pc++;
			break;
		}


		/* Memory access operations: */

		case AADDF: {
			
			#ifdef DEBUG
			fprintf(stderr, "Offset by 0x%X\n", P[pc+1]);
			#endif
			
			char *a = (char*)pop_ptr(S);
			if (a == NULL)
				c0_memory_error("Segmentation fault (attempt to dereference a NULL pointer)");
			
			a += P[pc+1];
			push_ptr(S, a);
			
			pc += 2;
			break;
		}

		case AADDS: {
			
			int i = pop_int(S);
			
			#ifdef DEBUG
			fprintf(stderr, "Array offset by 0x%X\n", i);
			#endif
			
			c0_array *array = (c0_array*)pop_ptr(S);
			if (array == NULL)
				c0_memory_error("Segmentation fault (attempt to dereference a NULL pointer)");
			
			char *addr = (char*)array->elems + array->elt_size * i;
			
			push_ptr(S, addr);
			
			pc++;
			break;
		}

		case IMLOAD: {
		
			#ifdef DEBUG
			fprintf(stderr, "Loading an int from a pointer, ");
			#endif
			
			int *int_ptr = (int*)pop_ptr(S);
			if (int_ptr == NULL)
				c0_memory_error("Segmentation fault (attempt to dereference a NULL pointer)");
			
			#ifdef DEBUG
			fprintf(stderr, "pushing 0x%X onto the stack\n", *int_ptr);
			#endif
			
			push_int(S, *int_ptr);
			pc++;
			break;
		}

		case IMSTORE: {
			
			int x = pop_int(S);
			
			#ifdef DEBUG
			fprintf(stderr, "Storing 0x%X to an address\n", x);
			#endif
			
			int *int_ptr = (int*)pop_ptr(S);
			if (int_ptr == NULL)
				c0_memory_error("Segmentation fault (attempt to dereference a NULL pointer)");
			
			*int_ptr = x;
			
			pc++;
			break;
		}

		case AMLOAD: {
			
			#ifdef DEBUG
			fprintf(stderr, "Loading a pointer from a pointer\n");
			#endif
			
			void *a = pop_ptr(S);
			if (a == NULL)
				c0_memory_error("Segmentation fault (attempt to dereference a NULL pointer)");
			
			push_ptr(S, *(void**)a);
			
			pc++;
			break;
		}

		case AMSTORE: {
			
			#ifdef DEBUG
			fprintf(stderr, "Storing a pointer to an address\n");
			#endif
			
			void *b = pop_ptr(S);
			void *a = pop_ptr(S);
			
			if (a == NULL)
				c0_memory_error("Segmentation fault (attempt to dereference a NULL pointer)");
			
			*(void**)a = b;
			
			pc++;
			break;
		}

		case CMLOAD: {
			
			#ifdef DEBUG
			fprintf(stderr, "Loading a char from a pointer, ");
			#endif
			
			char *char_ptr = (char*)pop_ptr(S);
			if (char_ptr == NULL)
				c0_memory_error("Segmentation fault (attempt to dereference a NULL pointer)");
			char c = *char_ptr;
			c &= 0x7F;
			
			#ifdef DEBUG
			fprintf(stderr, "pushing %c (0x%X) onto the stack\n", c, c);
			#endif
			
			push_int(S, c);
			pc++;
			break;
		}

		case CMSTORE: {
			
			char c = (char)pop_int(S);
			c &= 0x7F;
			
			#ifdef DEBUG
			fprintf(stderr, "Storing %c (0x%X) to an address\n", c, c);
			#endif
			
			char *char_ptr = (char*)pop_ptr(S);
			if (char_ptr == NULL)
				c0_memory_error("Segmentation fault (attempt to dereference a NULL pointer)");
			
			*char_ptr = c;
			
			pc++;
			break;
		}

		default:
			fprintf(stderr, "invalid opcode: 0x%02X\n", P[pc]);
			abort();
		}
	}

	/* cannot get here from infinite loop */
	assert(false);
}
