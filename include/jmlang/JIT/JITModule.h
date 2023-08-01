#ifndef JMLANG_JIT_JIT_MODULE_H
#define JMLANG_JIT_JIT_MODULE_H

#include "jmlang/Base/IntrusivePtr.h"
#include "jmlang/Base/buffer_t.h"

namespace llvm {
class Module;
}

namespace jmlang {
namespace internal {

class JITModuleHolder;
class CodeGen;

/// Function pointers into a compiled module. These function
/// pointers are meaningless once the last copy of a JITModule
/// is deleted, so don't cache them.
struct JITModule {
  /// A pointer to the raw function. It's true type depends
  /// on the Argument vector passed to CodeGen::compile. Image
  /// parameters become (buffer_t *), and scalar parameters become
  /// pointers to the appropriate values. The final argument is a
  /// pointer to the buffer_t defining the output.
  void* function;

  /// A slightly more type-safe wrapper around the raw
  /// module. Takes it arguments as an array of pointers that
  /// correspond to the arguments to function.
  int (*wrapped_function)(const void**);

  /// JIT helpers to interact with device-mapped buffer_t
  /// objects. These pointers may be NULL if not compiling for a
  /// gpu-like target.
  void (*copy_to_host)(struct buffer_t*);
  void (*copy_to_dev)(struct buffer_t*);
  void (*free_dev_buffer)(struct buffer_t*);

  /// The type of a halide runtime error handler function.
  typedef void (*ErrorHandler)(const char*);

  /// Set the runtime error handler for this module.
  void (*set_error_handler)(ErrorHandler);

  /// Set a custom malloc and free for this module to use. See
  /// Func::set_custom_allocator.
  void (*set_custom_allocator)(void* (*malloc)(size_t), void (*free)(void*));

  /// Set a custom parallel for loop launcher. See
  /// Func::set_custom_do_par_for.
  typedef int (*HalideTask)(int, uint8_t*);
  void (*set_custom_do_par_for)(int (*custom_do_par_for)(HalideTask, int, int,
                                                         uint8_t*));

  /// Set a custom do parallel task. See
  /// Func::set_custom_do_task.
  void (*set_custom_do_task)(int (*custom_do_task)(HalideTask, int, uint8_t*));

  /// Set a custom trace function. See Func::set_custom_trace.
  typedef void (*TraceFn)(const char*, int, int, int, int, int, const void*,
                          int, const int*);
  void (*set_custom_trace)(TraceFn);

  /// Shutdown the thread pool maintained by this JIT module. This
  /// is also done automatically when the last reference to this
  /// module is destroyed.
  void (*shutdown_thread_pool)();

  /// Close the tracing file this module may be writing to. Also
  /// done automatically when the last reference to this module is
  /// destroyed.
  void (*shutdown_trace)();

  // The JIT Module Allocator holds onto the memory storing the functions above.
  IntrusivePtr<JITModuleHolder> module;

  JITModule()
      : function(nullptr),
        wrapped_function(nullptr),
        copy_to_host(nullptr),
        copy_to_dev(nullptr),
        free_dev_buffer(nullptr),
        set_error_handler(nullptr),
        set_custom_allocator(nullptr),
        set_custom_do_par_for(nullptr),
        set_custom_do_task(nullptr),
        set_custom_trace(nullptr),
        shutdown_thread_pool(nullptr),
        shutdown_trace(nullptr) {}

  /// Take an llvm module and compile it. Populates the function
  /// pointer members above with the result.
  void compile_module(CodeGen* cg, llvm::Module* mod,
                      const std::string& function_name);
};

}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_JIT_JIT_MODULE_H