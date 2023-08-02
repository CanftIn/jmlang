#include "jmlang/JIT/JITModule.h"

#include <cstdint>
#include <string>

#include "jmlang/Base/Debug.h"
#include "jmlang/JIT/LLVMHeaders.h"

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

template <>
RefCount& ref_count<JITModuleHolder>(const JITModuleHolder* f) {
  return f->ref_count;
}

template <>
void destroy<JITModuleHolder>(const JITModuleHolder* f) {
  delete f;
}

void JITModule::compile_module(CodeGen* cg, llvm::Module* m,
                               const std::string& function_name) {}

}  // namespace internal
}  // namespace jmlang