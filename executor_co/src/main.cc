#include "aimrt_module_cpp_interface/module_base.h"
#include "core/aimrt_core.h"
#include <csignal>
#include <iostream>

#include "aimrt_module_cpp_interface/co/aimrt_context.h"
#include "aimrt_module_cpp_interface/co/async_scope.h"
#include "aimrt_module_cpp_interface/co/inline_scheduler.h"
#include "aimrt_module_cpp_interface/co/on.h"
#include "aimrt_module_cpp_interface/co/schedule.h"
#include "aimrt_module_cpp_interface/co/sync_wait.h"
#include "aimrt_module_cpp_interface/co/task.h"
// 定义一个ExecutorCoModule类,继承自aimrt::ModuleBase
class ExecutorCoModule : public aimrt::ModuleBase {
public:
  ExecutorCoModule() = default;
  ~ExecutorCoModule() override = default;

  // 需要实现该方法，返回模块信息
  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "ExecutorCoModule"};
  }

  // 需要实现该方法，初始化模块资源
  bool Initialize(aimrt::CoreRef core) override {
    // Save aimrt framework handle
    core_ = core;
    // 初始化执行器 需要传入执行器的名称
    thread_safe_executor_ =
        core_.GetExecutorManager().GetExecutor("thread_safe_executor");

    // 检查执行器是否为线程安全的
    AIMRT_CHECK_ERROR_THROW(thread_safe_executor_.ThreadSafe(),
                            "the executor must be thread safe!");

    AIMRT_INFO("Init succeeded.");

    return true;
  }

  // 需要实现该方法，开启模块
  bool Start() {
    AIMRT_INFO("Start succeeded.");
    // 原地开启协程任务
    scope_.spawn(
        aimrt::co::On(aimrt::co::InlineScheduler(), SimpleExecuteDemo()));

    return true;
  }

  void Shutdown() { AIMRT_INFO("Shutdown succeeded."); }

  aimrt::co::Task<void> SimpleExecuteDemo() {
    // Get work_executor scheduler
    aimrt::co::AimRTScheduler thread_safe_scheduler(thread_safe_executor_);
    // 调度协程至当前执行器
    co_await aimrt::co::Schedule(thread_safe_scheduler);
    uint32_t n = 0;
    auto task = [&n]() -> aimrt::co::Task<void> {
      n++;
      co_return;
    };

    // Schedule to thread_safe_scheduler
    aimrt::co::AsyncScope scope;
    for (int i = 0; i < 100; i++) {
      scope.spawn(aimrt::co::On(thread_safe_scheduler, task()));
    }

    // Wait for all task done
    co_await aimrt::co::On(aimrt::co::InlineScheduler(), scope.complete());

    AIMRT_INFO("Value of n is {}", n);

    co_return;
  }

private:
  // aimrt日志句柄
  aimrt::logger::LoggerRef GetLogger() { return core_.GetLogger(); }
  aimrt::executor::ExecutorRef thread_safe_executor_;

  aimrt::co::AsyncScope scope_;

private:
  aimrt::CoreRef core_;
};

// 信号处理函数,当有信号触发时,调用AimRT的shutdown 关闭aimrt
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
    // 初始化AimRT Core 对象
    aimrt::runtime::core::AimRTCore core;
    global_core_ptr_ = &core;

    // 向AimRT注册模块
    ExecutorCoModule executor_module;
    core.GetModuleManager().RegisterModule(executor_module.NativeHandle());

    // 读取命令行参数
    aimrt::runtime::core::AimRTCore::Options options;
    if (argc > 1)
      options.cfg_file_path = argv[1];
    // 初始化AimRT
    core.Initialize(options);
    // 开始执行AimRT 事件循环,该行代码会阻塞当前线程
    // 直到AimRT被关闭
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