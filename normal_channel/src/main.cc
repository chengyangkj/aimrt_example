#include "aimrt_module_cpp_interface/co/aimrt_context.h"
#include "aimrt_module_cpp_interface/co/async_scope.h"
#include "aimrt_module_cpp_interface/co/inline_scheduler.h"
#include "aimrt_module_cpp_interface/co/schedule.h"
#include "aimrt_module_cpp_interface/co/task.h"
#include "aimrt_module_cpp_interface/module_base.h"

#include "core/aimrt_core.h"
#include <csignal>
#include <iostream>

#include "aimrt_module_protobuf_interface/channel/protobuf_channel.h"
#include "aimrt_module_protobuf_interface/util/protobuf_tools.h"
#include "event.pb.h"

class NormalChannelModule : public aimrt::ModuleBase {
public:
  NormalChannelModule() = default;
  ~NormalChannelModule() override = default;

  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "NormalChannelModule"};
  }

  bool Initialize(aimrt::CoreRef core) override {
    core_ = core;
    // 初始化执行器
    executor_ = core_.GetExecutorManager().GetExecutor("work_thread_pool");
    // 初始化发布者
    publisher_ = core_.GetChannelHandle().GetPublisher("test_topic");
    // 注册发布者类型
    aimrt::channel::RegisterPublishType<
        aimrt::protocols::example::ExampleEventMsg>(publisher_);
    // 初始化订阅者
    subscriber_ = core_.GetChannelHandle().GetSubscriber("test_topic");
    // 订阅topic 并指明回调函数
    aimrt::channel::Subscribe<aimrt::protocols::example::ExampleEventMsg>(
        subscriber_, std::bind(&NormalChannelModule::DataCallback, this,
                               std::placeholders::_1));
    AIMRT_INFO("Init succeeded.");
    return true;
  }

  bool Start() {
    AIMRT_INFO("Start succeeded.");
    scope_.spawn(aimrt::co::On(aimrt::co::InlineScheduler(), MainLoop()));
    return true;
  }

  void Shutdown() { AIMRT_INFO("Shutdown succeeded."); }
  // Main loop
  aimrt::co::Task<void> MainLoop() {
    AIMRT_INFO("Start MainLoop.");
    aimrt::co::AimRTScheduler work_thread_pool_scheduler(executor_);
    uint32_t count = 0;
    while (run_flag_) {
      // 1s 执行一次
      co_await aimrt::co::ScheduleAfter(work_thread_pool_scheduler,
                                        std::chrono::milliseconds(1000));
      // 发布topic
      aimrt::protocols::example::ExampleEventMsg msg;
      msg.set_msg("count: " + std::to_string(count));
      msg.set_num(count);

      AIMRT_INFO("Publish new pb event, data: {}", aimrt::Pb2CompactJson(msg));
      aimrt::channel::Publish(publisher_, msg);
    }

    AIMRT_INFO("Exit MainLoop.");
    co_return;
  }
  // 订阅者收到topic时的回调函数
  void DataCallback(
      const std::shared_ptr<const aimrt::protocols::example::ExampleEventMsg>
          &data) {
    AIMRT_INFO("Receive new pb event, data: {}", aimrt::Pb2CompactJson(*data));
  }

private:
  // aimrt日志句柄
  aimrt::logger::LoggerRef GetLogger() { return core_.GetLogger(); }
  aimrt::executor::ExecutorRef executor_;
  aimrt::co::AsyncScope scope_;
  std::atomic_bool run_flag_ = true;
  // 发布者
  aimrt::channel::PublisherRef publisher_;
  // 订阅者
  aimrt::channel::SubscriberRef subscriber_;
  aimrt::CoreRef core_;
};

// 信号处理
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
  try {
    // 初始化AimRT Core 对象
    aimrt::runtime::core::AimRTCore core;
    global_core_ptr_ = &core;
    NormalChannelModule normal_publisher_module;
    core.GetModuleManager().RegisterModule(
        normal_publisher_module.NativeHandle());
    aimrt::runtime::core::AimRTCore::Options options;
    if (argc > 1)
      options.cfg_file_path = argv[1];
    core.Initialize(options);
    core.Start();
    core.Shutdown();
  } catch (const std::exception &e) {
    std::cout << "AimRT run with exception and exit. " << e.what() << std::endl;
    return -1;
  }
  std::cout << "AimRT exit." << std::endl;
  return 0;
}
