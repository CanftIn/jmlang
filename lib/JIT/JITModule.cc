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

namespace {
// Retrieve a function pointer from an llvm module, possibly by
// compiling it. Returns it by assigning to the last argument.
template <typename FP>
void hook_up_function_pointer(llvm::ExecutionEngine* ee, llvm::Module* mod,
                              const std::string& name, bool must_succeed,
                              FP* result) {
  assert(mod && ee);

  debug(2) << "Retrieving " << name << " from module\n";
  llvm::Function* fn = mod->getFunction(name);
  if (!fn) {
    if (must_succeed) {
      std::cerr << "Could not find function " << name << " in module\n";
      assert(false);
    } else {
      *result = NULL;
      return;
    }
  }

  debug(2) << "JIT Compiling " << name << "\n";
  void* f = ee->getPointerToFunction(fn);
  if (!f && must_succeed) {
    std::cerr << "Compiling " << name << " returned NULL\n";
    assert(false);
  }

  debug(2) << "Function " << name << " is at " << f << "\n";

  *result = reinterpret_bits<FP>(f);
}

}

void JITModule::compile_module(CodeGen* cg, llvm::Module* m,
                               const std::string& function_name) {
  // Make the execution engine
  debug(2) << "Creating new execution engine\n";
  std::string error_string;

  llvm::TargetOptions options;
  options.LessPreciseFPMADOption = true;
  options.NoFramePointerElim = false;
  options.AllowFPOpFusion = llvm::FPOpFusion::Fast;
  options.UnsafeFPMath = true;
  options.NoInfsFPMath = true;
  options.NoNaNsFPMath = true;
  options.HonorSignDependentRoundingFPMathOption = false;
  options.UseSoftFloat = false;
  options.FloatABIType =
      cg->use_soft_float_abi() ? llvm::FloatABI::Soft : llvm::FloatABI::Hard;
  options.NoZerosInBSS = false;
  options.GuaranteedTailCallOpt = false;
  options.DisableTailCalls = false;
  options.StackAlignmentOverride = 0;
  options.TrapFuncName = "";
  options.PositionIndependentExecutable = true;
  options.EnableSegmentedStacks = false;
  options.UseInitArray = false;

  llvm::EngineBuilder engine_builder(m);
  engine_builder.setTargetOptions(options);
  engine_builder.setErrorStr(&error_string);
  engine_builder.setEngineKind(llvm::EngineKind::JIT);
  engine_builder.setUseMCJIT(false);
  engine_builder.setOptLevel(llvm::CodeGenOpt::Aggressive);
  engine_builder.setMCPU(cg->mcpu());
  engine_builder.setMAttrs(vec<std::string>(cg->mattrs()));
  llvm::ExecutionEngine* ee = engine_builder.create();
  if (!ee)
    std::cerr << error_string << "\n";
  assert(ee && "Couldn't create execution engine");

  // Do any target-specific initialization
  cg->jit_init(ee, m);

  // Retrieve function pointers from the compiled module (which also
  // triggers compilation)
  debug(1) << "JIT compiling...\n";

  hook_up_function_pointer(ee, m, function_name, true, &function);

  void* function_address = reinterpret_bits<void*>(function);
  debug(1) << "JIT compiled function pointer " << function_address << "\n";

  hook_up_function_pointer(ee, m, function_name + "_jit_wrapper", true,
                           &wrapped_function);
  hook_up_function_pointer(ee, m, "jmlang_copy_to_host", false, &copy_to_host);
  hook_up_function_pointer(ee, m, "jmlang_copy_to_dev", false, &copy_to_dev);
  hook_up_function_pointer(ee, m, "jmlang_dev_free", false, &free_dev_buffer);
  hook_up_function_pointer(ee, m, "jmlang_set_error_handler", true,
                           &set_error_handler);
  hook_up_function_pointer(ee, m, "jmlang_set_custom_allocator", true,
                           &set_custom_allocator);
  hook_up_function_pointer(ee, m, "jmlang_set_custom_do_par_for", true,
                           &set_custom_do_par_for);
  hook_up_function_pointer(ee, m, "jmlang_set_custom_do_task", true,
                           &set_custom_do_task);
  hook_up_function_pointer(ee, m, "jmlang_set_custom_trace", true,
                           &set_custom_trace);
  hook_up_function_pointer(ee, m, "jmlang_shutdown_thread_pool", true,
                           &shutdown_thread_pool);
  hook_up_function_pointer(ee, m, "jmlang_shutdown_trace", true,
                           &shutdown_trace);

  debug(2) << "Finalizing object\n";
  ee->finalizeObject();

  // Stash the various objects that need to stay alive behind a
  // reference-counted pointer.
  module = new JITModuleHolder(ee, m, shutdown_thread_pool, shutdown_trace);

  // Do any target-specific post-compilation module meddling
  cg->jit_finalize(ee, m, &module.ptr->cleanup_routines);
}

}
}  // namespace internal
}  // namespace jmlang