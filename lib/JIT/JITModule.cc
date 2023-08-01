#include "jmlang/JIT/JITModule.h"
#include "jmlang/JIT/LLVMHeaders.h"
#include "jmlang/Base/Debug.h"

#include <cstdint>
#include <string>

namespace jmlang {
namespace internal {

// Wraps an execution engine. Takes ownership of the given module and
// the memory for jit compiled code.
class JITModuleHolder {
 public:
  mutable RefCount ref_count;

  JITModuleHolder(llvm::ExecutionEngine* ee, llvm::Module* m,
                  void (*stop_threads)(), void (*stop_trace)())
      : execution_engine(ee),
        module(m),
        context(&m->getContext()),
        shutdown_thread_pool(stop_threads),
        shutdown_trace(stop_trace) {}

  ~JITModuleHolder() {
    for (size_t i = 0; i < cleanup_routines.size(); i++) {
      void* ptr = reinterpret_bits<void*>(cleanup_routines[i]);
      debug(1) << "Calling target specific cleanup routine at " << ptr << "\n";
      (*cleanup_routines[i])();
    }

    shutdown_thread_pool();
    shutdown_trace();
    delete execution_engine;
    delete context;
  }

  llvm::ExecutionEngine* execution_engine;
  llvm::Module* module;
  llvm::LLVMContext* context;
  void (*shutdown_thread_pool)();
  void (*shutdown_trace)();

  /** Do any target-specific module cleanup. */
  std::vector<void (*)()> cleanup_routines;
};

}  // namespace internal
}  // namespace jmlang