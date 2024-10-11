#include "aimrt_module_cpp_interface/module_base.h"
#include "core/aimrt_core.h"
#include <csignal>
#include <iostream>

class ExecutorModule : public aimrt::ModuleBase {
public:
  ExecutorModule() = default;
  ~ExecutorModule() override = default;

  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "ExecutorModule"};
  }

  bool Initialize(aimrt::CoreRef core) override {

    core_ = core;
    // 初始化执行器 需要传入执行器的名称
    work_executor_ = core_.GetExecutorManager().GetExecutor("work_thread_pool");
    time_schedule_executor_ =
        core_.GetExecutorManager().GetExecutor("work_thread_pool");
    AIMRT_INFO("Init succeeded.");

    return true;
  }

  bool Start() {
    AIMRT_INFO("Start succeeded.");
    run_flag_ = true;
    // 使用执行器执行10个任务
    for (int i = 0; i < 10; i++) {
      work_executor_.Execute([this]() {
        std::stringstream thread_id;
        thread_id << std::this_thread::get_id();
        AIMRT_INFO("This is a multi thread execute, current thread id:{}",
                   thread_id.str());
      });
    }
    TimeScheduleDemo();
    return true;
  }

  void TimeScheduleDemo() {
    if (!run_flag_)
      return;
    static int loop_count = 0;
    AIMRT_INFO("Loop count : {}", loop_count++);

    time_schedule_executor_.ExecuteAfter(
        std::chrono::seconds(1),
        std::bind(&ExecutorModule::TimeScheduleDemo, this));
  }

  void Shutdown() {
    run_flag_ = false;
    AIMRT_INFO("Shutdown succeeded.");
  }

private:
  aimrt::logger::LoggerRef GetLogger() { return core_.GetLogger(); }
  // 声明执行器对象
  aimrt::executor::ExecutorRef work_executor_;
  aimrt::executor::ExecutorRef time_schedule_executor_;
  std::atomic<bool> run_flag_{0};

private:
  aimrt::CoreRef core_;
};

aimrt::runtime::core::AimRTCore *global_core_ptr_ = nullptr;
void SignalHandler(int sig) {
  if (global_core_ptr_ && (sig == SIGINT || sig == SIGTERM)) {
    global_core_ptr_->Shutdown();
    return;
  }

  raise(sig);
};

int main(int32_t argc, char **argv) {
  signal(SIGINT, SignalHandler);
  signal(SIGTERM, SignalHandler);
  std::cout << "AimRT start." << std::endl;

  try {
    aimrt::runtime::core::AimRTCore core;
    global_core_ptr_ = &core;

    ExecutorModule executor_module;
    core.GetModuleManager().RegisterModule(executor_module.NativeHandle());

    aimrt::runtime::core::AimRTCore::Options options;
    if (argc > 1)
      options.cfg_file_path = argv[1];
    core.Initialize(options);
    core.Start();

    core.Shutdown();

    global_core_ptr_ = nullptr;
  } catch (const std::exception &e) {
    std::cout << "AimRT run with exception and exit. " << e.what() << std::endl;
    return -1;
  }

  std::cout << "AimRT exit." << std::endl;
  return 0;
}
