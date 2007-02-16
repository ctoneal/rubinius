#ifndef __CPU_H_
#define __CPU_H_

#include <glib.h>

/* Configuration macros. */

/* Enables use of FastMethodContext objects. */
#define CTX_USE_FAST 1

/* Enables the context cache. */
#define CTX_CACHE_ENABLED 0

struct _method_cache {
  
};

typedef struct _method_cache method_cache;

#define CPU_REGISTERS OBJECT sender; \
  unsigned long int ip; \
  unsigned long int sp; \
  OBJECT block; \
  unsigned long int raiseable; \
  OBJECT method; \
  unsigned char *data; \
  unsigned long int data_size; \
  OBJECT literals; \
  OBJECT self; \
  OBJECT locals; \
  unsigned long int argcount; \
  OBJECT name; \
  OBJECT method_module; \
  unsigned long int num_locals; \
  long is_fast;

struct fast_context {
  CPU_REGISTERS
};

#define FASTCTX_FIELDS 20

struct rubinius_cpu {
  CPU_REGISTERS
  
  char buffer[20];
  
  /* Below here, the cpu registers are not saved and restored
     per call. */
  
  long int args;
  OBJECT stack;
  OBJECT exception;
  OBJECT enclosing_class;
  OBJECT new_class_of;
  OBJECT exceptions;
  OBJECT top_context;

  OBJECT active_context, home_context, main;
  
  GPtrArray *paths;
  unsigned int depth;
  
  OBJECT context_cache;
};

typedef struct rubinius_cpu *cpu;

#define cpu_stack_empty_p(state, cpu) (cpu->sp == -1)
#define cpu_local_get(state, cpu, idx) (NTH_FIELD(cpu->locals, idx))
#define cpu_local_set(state, cpu, idx, obj) (SET_FIELD(cpu->locals, idx, obj))

#define stack_push(obj) cpu_stack_push(state, c, obj, FALSE)
#define stack_pop() cpu_stack_pop(state, c)
#define stack_top() cpu_stack_top(state, c)

cpu cpu_new(STATE);
void cpu_initialize(STATE, cpu c);
void cpu_setup_top_scope(STATE, cpu c);
void cpu_initialize_context(STATE, cpu c);
void cpu_update_roots(STATE, cpu c, GPtrArray *roots, int start);
void cpu_save_registers(STATE, cpu c);
void cpu_restore_context(STATE, cpu c, OBJECT x);
void cpu_restore_context_with_home(STATE, cpu c, OBJECT ctx, OBJECT home, int ret, int is_block);
void cpu_activate_context(STATE, cpu c, OBJECT ctx, OBJECT home);
void cpu_return_to_sender(STATE, cpu c, int consider_block);
OBJECT cpu_const_get(STATE, cpu c, OBJECT sym, OBJECT under);
OBJECT cpu_const_set(STATE, cpu c, OBJECT sym, OBJECT val, OBJECT under);
void cpu_run(STATE, cpu c);
int  cpu_stack_push(STATE, cpu c, OBJECT oop, int check);
OBJECT cpu_stack_pop(STATE, cpu c);
OBJECT cpu_stack_top(STATE, cpu c);
int cpu_dispatch(STATE, cpu c);
void cpu_set_encloser_path(STATE, cpu c, OBJECT cls);
void cpu_push_encloser(STATE, cpu c);
void cpu_add_method(STATE, cpu c, OBJECT target, OBJECT sym, OBJECT method);
void cpu_attach_method(STATE, cpu c, OBJECT target, OBJECT sym, OBJECT method);
int cpu_perform_primitive(STATE, cpu c, int prim, OBJECT mo, int args);
void cpu_raise_exception(STATE, cpu c, OBJECT exc);
void cpu_raise_arg_error(STATE, cpu c, int args, int req);
OBJECT cpu_new_exception(STATE, cpu c, OBJECT klass, char *msg);
inline void cpu_perform_hook(STATE, cpu c, OBJECT recv, OBJECT meth, OBJECT arg);

inline void cpu_goto_method(STATE, cpu c, OBJECT recv, OBJECT meth,
                                     int count, OBJECT name);

void cpu_run_script(STATE, cpu c, OBJECT meth);

OBJECT exported_cpu_find_method(STATE, cpu c, OBJECT klass, OBJECT name, OBJECT *mod);

OBJECT cpu_unmarshal(STATE, char *str);
OBJECT cpu_marshal(STATE, OBJECT obj);
OBJECT cpu_unmarshal_file(STATE, char *path);
GString *cpu_marshal_to_gstring(STATE, OBJECT obj);
OBJECT cpu_marshal_to_file(STATE, OBJECT obj, char *path);

void cpu_bootstrap(STATE);
void cpu_add_roots(STATE, cpu c, GPtrArray *roots);
void cpu_update_roots(STATE, cpu c, GPtrArray *roots, int start);
#endif /* __CPU_H_ */
