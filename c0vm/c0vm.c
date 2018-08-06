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

int execute(struct bc0_file *bc0) {
	REQUIRES(bc0 != NULL);

	/* Variables */
	c0v_stack_t S = c0v_stack_new();	/* Operand stack of C0 values */
	ubyte *P = bc0->function_pool[0].code;	/* Array of bytes that make up the current function */
	size_t pc = 0;	/* Current location within the current byte array P */
	c0_value *V = xmalloc(sizeof(c0_value) * (bc0->function_pool[0].num_args + bc0->function_pool[0].num_vars));	/* Local variables (you won't need this till Task 2) */
	(void) V;

	/* The call stack, a generic stack that should contain pointers to frames */
	/* You won't need this until you implement functions. */
	gstack_t callStack;
	(void) callStack;

	while (true) {

#ifdef DEBUG
		/* You can add extra debugging information here */
		fprintf(stderr, "Opcode %x -- Stack size: %zu -- PC: %zu\t", P[pc], c0v_stack_size(S), pc);
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
			int retval = val2int(c0v_pop(S));
			assert(c0v_stack_empty(S));
#ifdef DEBUG
			fprintf(stderr, "Returning %d from execute()\n", retval);
#endif
			c0v_stack_free(S);
			free(V);
			return retval;
		}


		/* Arithmetic and Logical operations */

		case IADD: {
			pc++;
			uint32_t b = pop_int(S);
			uint32_t a = pop_int(S);
			uint32_t result = a + b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X + 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, (int32_t)result);
			break;
		}

		case ISUB: {
			pc++;
			uint32_t b = pop_int(S);
			uint32_t a = pop_int(S);
			uint32_t result = a - b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X - 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, (int32_t)result);
			break;
		}

		case IMUL: {
			pc++;
			uint32_t b = pop_int(S);
			uint32_t a = pop_int(S);
			uint32_t result = a * b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X * 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, (int32_t)result);
			break;
		}

		case IDIV: {
			pc++;
			uint32_t b = pop_int(S);
			uint32_t a = pop_int(S);
			if (a == 0x80 && b == 0xFF)
				c0_arith_error("INT_MIN divided by -1");
			else if (b == 0)
				c0_arith_error("Division by zero");
			uint32_t result = a / b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X / 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, (int32_t)result);
			break;
		}

		case IREM: {
			pc++;
			uint32_t b = pop_int(S);
			uint32_t a = pop_int(S);
			if (a == 0x80 && b == 0xFF)
				c0_arith_error("INT_MIN divided by -1");
			else if (b == 0)
				c0_arith_error("Division by zero");
			uint32_t result = a % b;
			#ifdef DEBUG
			fprintf(stderr, "0x%X mod 0x%X = 0x%X\n", a, b, result);
			#endif
			push_int(S, (int32_t)result);
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
			uint32_t b = pop_int(S);
			uint32_t a = pop_int(S);
			uint32_t result = a << b;
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
			push_int(S, P[pc+1]);
			pc += 2;
			break;
		}

		case ILDC: {
			#ifdef DEBUG
			fprintf(stderr, "Pushing 0x%X onto stack\n", bc0->int_pool[ (P[pc+1] << 8) | P[pc+2] ]);
			#endif
			push_int(S, bc0->int_pool[ (P[pc+1] << 8) | P[pc+2] ]);
			pc += 3;
			break;
		}

		case ALDC: {
			push_ptr(S, &(bc0->string_pool[ (P[pc+1] << 8) | P[pc+2] ]));
			pc += 3;
			break;
		}

		case ACONST_NULL: {
			pc++;
			push_ptr(S, NULL);
			break;
		}


		/* Operations on local variables */

		case VLOAD: {
			c0v_push(S, V[P[pc+1]]);
			#ifdef DEBUG
			fprintf(stderr, "Got 0x%X from V[0x%X], pushing onto stack\n", val2int(V[P[pc+1]]), P[pc+1]);
			#endif
			pc += 2;
			break;
		}

		case VSTORE: {
			V[P[pc+1]] = c0v_pop(S);
			#ifdef DEBUG
			fprintf(stderr, "Popped 0x%X from stack, stored into V[0x%X]\n", val2int(V[P[pc+1]]), P[pc+1]);
			#endif
			pc += 2;
			break;
		}


		/* Control flow operations */

		case NOP: {
			pc++;
			break;
		}

		case IF_CMPEQ: {
			c0_value a = c0v_pop(S);
			c0_value b = c0v_pop(S);
			if (val_equal(a, b))
				pc += (P[pc+1] << 8) | P[pc+2];
			else
				pc += 3;
			break;
		}

		case IF_CMPNE: {
			c0_value a = c0v_pop(S);
			c0_value b = c0v_pop(S);
			if (!val_equal(a, b))
				pc += (P[pc+1] << 8) | P[pc+2];
			else
				pc += 3;
			break;
		}

		case IF_ICMPLT: {
			int y = pop_int(S);
			int x = pop_int(S);
			if (x < y)
				pc += (P[pc+1] << 8) | P[pc+2];
			else
				pc += 3;
			break;
		}

		case IF_ICMPGE: {
			int y = pop_int(S);
			int x = pop_int(S);
			if (x >= y)
				pc += (P[pc+1] << 8) | P[pc+2];
			else
				pc += 3;
			break;
		}

		case IF_ICMPGT: {
			int y = pop_int(S);
			int x = pop_int(S);
			if (x > y)
				pc += (P[pc+1] << 8) | P[pc+2];
			else
				pc += 3;
			break;
		}

		case IF_ICMPLE: {
			int y = pop_int(S);
			int x = pop_int(S);
			if (x <= y)
				pc += (P[pc+1] << 8) | P[pc+2];
			else
				pc += 3;
			break;
		}

		case GOTO: {
			pc += (P[pc+1] << 8) | P[pc+2];
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

		case INVOKESTATIC:

		case INVOKENATIVE:


		/* Memory allocation operations: */

		case NEW:

		case NEWARRAY:

		case ARRAYLENGTH:


		/* Memory access operations: */

		case AADDF:

		case AADDS:

		case IMLOAD:

		case IMSTORE:

		case AMLOAD:

		case AMSTORE:

		case CMLOAD:

		case CMSTORE:

		default:
			fprintf(stderr, "invalid opcode: 0x%02x\n", P[pc]);
			abort();
		}
	}

	/* cannot get here from infinite loop */
	assert(false);
}
