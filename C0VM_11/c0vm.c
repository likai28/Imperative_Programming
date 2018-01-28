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
  c0v_stack_t S; /* Operand stack of C0 values */
  ubyte *P;      /* Function body */
  size_t pc;     /* Program counter */
  c0_value *V;   /* The local variables */
};

void stack_elem_free_function(stack_elem x)
{
    free(x);
}

int execute(struct bc0_file *bc0) {
  REQUIRES(bc0 != NULL);

  /* Variables */
  c0v_stack_t S; /* Operand stack of C0 values */
  ubyte *P;      /* The array of bytes that make up the current function */
  size_t pc;     /* Your current location within the current byte array P */
  c0_value *V;   /* The local variables (you won't need this till Task 2) */
  (void) V;

  S = c0v_stack_new();
  P = (bc0->function_pool[0]).code;
  pc = 0;
  V = xmalloc(sizeof(c0_value)*((size_t)(bc0->function_pool->num_vars)));

  /* The call stack, a generic stack that should contain pointers to frames */
  /* You won't need this until you implement functions. */
  gstack_t callStack;
  //(void) callStack;
  callStack = stack_new();
  gstack_t arraystack = stack_new();

  while (true) {
#ifdef DEBUG
    /* You can add extra debugging information here */
    fprintf(stderr, "Opcode %x -- Stack size: %zu -- PC: %zu\n",
            P[pc], c0v_stack_size(S), pc);
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
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     c0v_push(S,v1);
     c0v_push(S,v2);
     break;
    }


    /* Returning from a function.
     * This currently has a memory leak! It will need to be revised
     * when you write INVOKESTATIC. */

    case RETURN: {
      c0_value retval = c0v_pop(S);
      assert(c0v_stack_empty(S));
#ifdef DEBUG

#endif
      // Free everything before returning from the execute function!
      c0v_stack_free(S);
      free(V);
      //New implementation for task4
      if (stack_empty(callStack))
      {
        int32_t res2 = val2int(retval);
        stack_free(callStack,NULL);
        stack_free(arraystack,&stack_elem_free_function);
        fprintf(stderr, "Returning %d from execute()\n", res2);
        return res2;
      }
      else
      {
        frame *F = pop(callStack);
        V = F->V;
        S = F->S;
        P = F->P;
        pc = F->pc;
        free(F);
        c0_value res = retval;
        c0v_push(S,res);
      }
      break;
    }


    /* Arithmetic and Logical operations */

    case IADD: {
      pc++;
      c0_value v1 = c0v_pop(S);
      c0_value v2 = c0v_pop(S);
      int32_t res1 = val2int(v1);
      int32_t res2 = val2int(v2);
      c0_value v3 = int2val((res1+res2));
      c0v_push(S,v3);
      break;
    }


    case ISUB: {
      pc++;
      c0_value v1 = c0v_pop(S);
      c0_value v2 = c0v_pop(S);
      int32_t res1 = val2int(v1);
      int32_t res2 = val2int(v2);
      c0_value v3 = int2val((res2-res1));
      c0v_push(S,v3);
      break;
    }

    case IMUL: {
     pc++;
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     int32_t res1 = val2int(v1);
     int32_t res2 = val2int(v2);
     c0_value v3 = int2val((res2*res1));
     c0v_push(S,v3);
     break;
    }

    case IDIV: {
     pc++;
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     int32_t res1 = val2int(v1);
     int32_t res2 = val2int(v2);
     if(res1==0)
         c0_arith_error("Divided by zero");
     if(res2==INT_MIN&&res1==-1)
         c0_arith_error("Int_min divided by -1");
     c0_value v3 = int2val((res2/res1));
     c0v_push(S,v3);
     break;
    }

    case IREM: {
     pc++;
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     int32_t res1 = val2int(v1);
     int32_t res2 = val2int(v2);
     if(res1 ==0)
         c0_arith_error("Moded by zero");
     if(res2==INT_MIN&&res1==-1)
         c0_arith_error("Int_min moded by -1");
     c0_value v3 = int2val((res2%res1));
     c0v_push(S,v3);
     break;
    }


    case IAND: {
     pc++;
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     int32_t res1 = val2int(v1);
     int32_t res2 = val2int(v2);
     c0_value v3 = int2val((res2&res1));
     c0v_push(S,v3);
     break;
    }

    case IOR: {
     pc++;
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     int32_t res1 = val2int(v1);
     int32_t res2 = val2int(v2);
     c0_value v3 = int2val((res2|res1));
     c0v_push(S,v3);
     break;
    }

    case IXOR: {
     pc++;
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     int32_t res1 = val2int(v1);
     int32_t res2 = val2int(v2);
     c0_value v3 = int2val((res2^res1));
     c0v_push(S,v3);
     break;
    }

    case ISHL: {
     pc++;
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     int32_t res1 = val2int(v1);
     int32_t res2 = val2int(v2);
     if(res1<0||res1>31)
        c0_arith_error("Left shift bits number invalid");
     c0_value v3 = int2val((res2<<res1));
     c0v_push(S,v3);
     break;
    }

    case ISHR: {
     pc++;
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     int32_t res1 = val2int(v1);
     int32_t res2 = val2int(v2);
     if(res1<0||res1>31)
         c0_arith_error("Right shift bits number invalid");
     c0_value v3 = int2val((res2>>res1));
     c0v_push(S,v3);
     break;
    }


    /* Pushing constants */

    case BIPUSH: {
     pc++;
     int32_t i1 = (int32_t)(int8_t)P[pc];
     //int32_t i2 = (int32_t)i1;
     c0_value v1 = int2val(i1);
     c0v_push(S,v1);
     pc++;
     break;
    }

    case ILDC: {
     pc++;
     uint16_t c1 = (uint16_t)P[pc];
     pc++;
     uint16_t c2 = (uint16_t)P[pc];
     int32_t res = bc0->int_pool[(c1<<8)|c2];
     c0_value v1 = int2val(res);
     c0v_push(S,v1);
     pc++;
     break;
    }

    case ALDC: {
     pc++;
     uint16_t c1 = (uint16_t)P[pc];
     pc++;
     uint16_t c2 = (uint16_t)P[pc];
     char* res = &(bc0->string_pool[(c1<<8)|c2]);
     c0_value v1 = ptr2val(res);
     c0v_push(S,v1);
     pc++;
     break;
    }

    case ACONST_NULL: {
     void* p = NULL;
     c0_value v1 = ptr2val(p);
     c0v_push(S,v1);
     pc++;
     break;
    }


    /* Operations on local variables */

    case VLOAD: {
     pc++;
     c0_value res = V[P[pc]];
     c0v_push(S,res);
     pc++;
     break;
    }

    case VSTORE: {
     pc++;
     c0_value v = c0v_pop(S);
     V[P[pc]] = v;
     pc++;
     break;
    }


    /* Control flow operations */

    case NOP: {
     pc++;
     break;
    }

    case IF_CMPEQ: {
     size_t pc0 = pc;
     pc++;
     uint16_t o1 =(uint16_t)P[pc];
     pc++;
     uint16_t o2 = (uint16_t)P[pc];
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     if(val_equal(v1,v2))
        pc = pc0+(int16_t)(o1<<8|o2);
     else
        pc++;
     break;
    }

    case IF_CMPNE: {
     size_t pc0 = pc;
     pc++;
     uint16_t o1 =(uint16_t)P[pc];
     pc++;
     uint16_t o2 = (uint16_t)P[pc];
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     if(!val_equal(v1,v2))
        pc = pc0+(int16_t)(o1<<8|o2);
     else
        pc++;
     break;
    }

    case IF_ICMPLT: {
     size_t pc0 = pc;
     pc++;
     uint16_t o1 =(uint16_t)P[pc];
     pc++;
     uint16_t o2 = (uint16_t)P[pc];
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     int32_t res1 = val2int(v1);
     int32_t res2 = val2int(v2);
     if(res2<res1)
        pc = pc0+(int16_t)(o1<<8|o2);
     else
        pc++;
     break;
    }

    case IF_ICMPGE: {
     size_t pc0 = pc;
     pc++;
     uint16_t o1 =(uint16_t)P[pc];
     pc++;
     uint16_t o2 = (uint16_t)P[pc];
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     int32_t res1 = val2int(v1);
     int32_t res2 = val2int(v2);
     if(res2>=res1)
        pc = pc0+(int16_t)(o1<<8|o2);
     else
        pc++;
     break;
    }

    case IF_ICMPGT: {
     size_t pc0 =pc;
     pc++;
     uint16_t o1 =(uint16_t)P[pc];
     pc++;
     uint16_t o2 = (uint16_t)P[pc];
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     int32_t res1 = val2int(v1);
     int32_t res2 = val2int(v2);
     if(res2>res1)
        pc = pc0+(int16_t)(o1<<8|o2);
     else
        pc++;
     break;
    }

    case IF_ICMPLE: {
     size_t pc0 =pc;
     pc++;
     uint16_t o1 =(uint16_t)P[pc];
     pc++;
     uint16_t o2 = (uint16_t)P[pc];
     c0_value v1 = c0v_pop(S);
     c0_value v2 = c0v_pop(S);
     int32_t res1 = val2int(v1);
     int32_t res2 = val2int(v2);
     if(res2<=res1)
        pc = pc0+(int16_t)(o1<<8|o2);
     else
        pc++;
     break;
    }


    case GOTO: {
     size_t pc0 = pc;
     pc++;
     uint16_t o1 = (uint16_t)P[pc];
     pc++;
     int16_t o2 = (uint16_t)P[pc];
     pc = pc0 +(int16_t)(o1<<8|o2);
     break;
    }

    case ATHROW: {
     pc++;
     c0_value res = c0v_pop(S);
     char *c = val2ptr(res);
     c0_user_error(c);
     break;
    }

    case ASSERT: {
     pc++;
     c0_value s = c0v_pop(S);
     c0_value x = c0v_pop(S);
     int32_t xres = val2int(x);
     char *c = val2ptr(s);
     if(xres==0)
         c0_assertion_failure(c);
     break;
    }


    /* Function call operations: */

    case INVOKESTATIC: {
     pc++;
     uint16_t c1 = (uint16_t)P[pc];
     pc++;
     uint16_t c2 = (uint16_t)P[pc];
     frame * F = xmalloc(sizeof(frame));
     F->P = P;
     F->pc = pc+1;
     F->V =V;
     F->S = S;
     push(callStack,F);
     size_t V_new_size = (size_t)((bc0->function_pool[c1<<8|c2]).num_vars);
     c0_value *V_new = xmalloc(sizeof(c0_value)*V_new_size);
     size_t Args_size = (size_t)((bc0->function_pool[c1<<8|c2]).num_args);
     for(int i=Args_size-1;i>=0;i--)
     {
        V_new[i] = c0v_pop(S);
     }
     V = V_new;
     S = c0v_stack_new();
     P = bc0->function_pool[c1<<8|c2].code;
     pc =0;
     break;
    }

    case INVOKENATIVE: {
     pc++;
     uint16_t c1 = (uint16_t)P[pc];
     pc++;
     uint16_t c2 = (uint16_t)P[pc];
     size_t Args_size =(size_t)((bc0->native_pool[c1<<8|c2]).num_args);
     c0_value *V_new = xmalloc(sizeof(c0_value)*Args_size);
     for (int i =Args_size-1;i>=0;i--)
     {
        V_new[i] = c0v_pop(S);
     }
     size_t fn_index = (size_t)((bc0->native_pool[c1<<8|c2]).
                       function_table_index);
     native_fn* g = native_function_table[fn_index];
     c0_value res = (*g)(V_new);
     c0v_push(S,res);
     pc++;
     free(V_new);
     break;
    }


    /* Memory allocation operations: */

    case NEW: {
     pc++;
     size_t s = (size_t)P[pc];
     char *a = calloc(1,s);
     c0_value res = ptr2val(a);
     c0v_push(S,res);
     push(arraystack,a);
     pc++;
     break;
    }

    case NEWARRAY: {
     pc++;
     size_t s = (size_t)P[pc];
     c0_value n1 = c0v_pop(S);
     int32_t n = val2int(n1);
     //void *a = xmalloc(n*s);
     c0_array* arr = xmalloc(sizeof(c0_array));
     arr->count = n;
     arr->elt_size = s;
     arr->elems = calloc(n,s);
     c0_value res = ptr2val(arr);
     c0v_push(S,res);
     push(arraystack,arr);
     push(arraystack,arr->elems);
     pc++;
     break;
    }

    case ARRAYLENGTH: {
     c0_value a1 = c0v_pop(S);
     c0_array* a = (c0_array*)val2ptr(a1);
     int32_t n =0;
     if(a==NULL)
        n =0;
     else
        n = a->count;
     c0_value res = int2val(n);
     c0v_push(S,res);
     pc++;
     break;
    }


    /* Memory access operations: */

    case AADDF: {
     pc++;
     uint8_t f = P[pc];
     c0_value a1 = c0v_pop(S);
     char *a = (char*)val2ptr(a1);
     if(a==NULL)
         c0_memory_error("Pointer is NULL in AADDF");
     else
     {
         char *a_new = a+f;
         c0_value res = ptr2val(a_new);
         c0v_push(S,res);
     }
     pc++;
     break;
    }

    case AADDS: {
     c0_value i1 = c0v_pop(S);
     c0_value a1 = c0v_pop(S);
     int32_t i = val2int(i1);
     c0_array* a = (c0_array*)val2ptr(a1);
     if(a==NULL)
         c0_memory_error("Pointer is NULL in AADDS");
     if(i<0||i>=a->count)
         c0_memory_error("Invalid index access of array");
     char* tmp = (char*)a->elems;
     tmp = tmp +a->elt_size*i;
     c0_value res = ptr2val(tmp);
     c0v_push(S,res);
     pc++;
     break;
    }

    case IMLOAD: {
     c0_value a = c0v_pop(S);
     int32_t *p = val2ptr(a);
     if(p==NULL)
         c0_memory_error("Pointer is NULL in IMLOAD");
     else
     {
         int32_t x = *p;
         c0_value res = int2val(x);
         c0v_push(S,res);
     }
     pc++;
     break;
    }

    case IMSTORE: {
     c0_value x1 = c0v_pop(S);
     int32_t x = val2int(x1);
     c0_value a1 = c0v_pop(S);
     int32_t* a = val2ptr(a1);
     if(a==NULL)
         c0_memory_error("Pointer is NULL in IMSTORE");
     else
     {
         *a = x;
     }
     pc++;
     break;
    }

    case AMLOAD: {
     c0_value a1 = c0v_pop(S);
     void **a = val2ptr(a1);
     if(a==NULL)
         c0_memory_error("Pointer is NULL in AMLOAD");
     else
     {
         c0_value res = ptr2val(*a);
         c0v_push(S,res);
     }
     pc++;
     break;
    }

    case AMSTORE: {
     c0_value b1 = c0v_pop(S);
     c0_value a1 = c0v_pop(S);
     void **a = val2ptr(a1);
     void *b = val2ptr(b1);
     if(a==NULL)
         c0_memory_error("Pointer is NULL in AMSTORE");
     else
     {
         *a = b;
     }
     pc++;
     break;
    }

    case CMLOAD: {
     c0_value a1 = c0v_pop(S);
     char *a = val2ptr(a1);
     if(a==NULL)
         c0_memory_error("Pointer is NULL in CMLOAD");
     else
     {
         int32_t x = (int32_t)(*a);
         c0_value res = int2val(x);
         c0v_push(S,res);
     }
     pc++;
     break;
    }

    case CMSTORE: {
     c0_value x1 = c0v_pop(S);
     c0_value a1 = c0v_pop(S);
     int32_t x = val2int(x1);
     char *a = val2ptr(a1);
     if(a==NULL)
         c0_memory_error("Pointer is NULL in CMSTORE");
     else
     {
         *a = x&0x7F;
     }
     pc++;
     break;
    }

    default:
      fprintf(stderr, "invalid opcode: 0x%02x\n", P[pc]);
      abort();
    }
  }

  /* cannot get here from infinite loop */
  assert(false);
}

