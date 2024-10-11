#include "aimrt_module_cpp_interface/module_base.h"
#include "core/aimrt_core.h"
#include <csignal>
#include <iostream>
// 定义一个HelloWorldModule类,继承自aimrt::ModuleBase
class HelloWorldModule : public aimrt::ModuleBase {
public:
  HelloWorldModule() = default;
  ~HelloWorldModule() override = default;

  // 需要实现该方法，返回模块信息
  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "HelloWorldModule"};
  }

  // 需要实现该方法，初始化模块资源
  bool Initialize(aimrt::CoreRef core) override {
    // Save aimrt framework handle
    core_ = core;

    // 读取配置
    auto file_path = core_.GetConfigurator().GetConfigFilePath();
    if (!file_path.empty()) {
      // AimRT会将配置写入到临时文件中，我们使用YAML-CPP加载配置
      YAML::Node cfg_node = YAML::LoadFile(file_path.data());
      // 遍历配置并打印
      for (const auto &itr : cfg_node) {
        std::string k = itr.first.as<std::string>();
        std::string v = itr.second.as<std::string>();
        AIMRT_INFO("cfg [{} : {}]", k, v);
      }
    }

    AIMRT_INFO("Init succeeded.");
    return true;
  }

  // 需要实现该方法，开启模块
  bool Start() {
    AIMRT_INFO("Start succeeded.");

    return true;
  }

  // 在AimRT被关闭时，会调用该方法
  void Shutdown() { AIMRT_INFO("Shutdown succeeded."); }

private:
  // aimrt日志句柄
  aimrt::logger::LoggerRef GetLogger() { return core_.GetLogger(); }

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
  // 注册ctrl+c信号处理
  signal(SIGINT, SignalHandler);
  // 注册系统kill信号处理
  signal(SIGTERM, SignalHandler);
  std::cout << "AimRT start." << std::endl;

  try {
    // 初始化AimRT Core 对象
    aimrt::runtime::core::AimRTCore core;
    global_core_ptr_ = &core;

    // 向AimRT注册模块
    HelloWorldModule helloworld_module;
    core.GetModuleManager().RegisterModule(helloworld_module.NativeHandle());

    // 读取命令行参数
    aimrt::runtime::core::AimRTCore::Options options;
    if (argc > 1)
      options.cfg_file_path = argv[1];
    // 初始化AimRT
    core.Initialize(options);
    // 开始执行AimRT 事件循环,该行代码会阻塞当前线程
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
