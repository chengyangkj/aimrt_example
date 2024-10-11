
#include "aimrt_module_cpp_interface/co/aimrt_context.h"
#include "aimrt_module_cpp_interface/co/async_scope.h"
#include "aimrt_module_cpp_interface/co/inline_scheduler.h"
#include "aimrt_module_cpp_interface/co/on.h"
#include "aimrt_module_cpp_interface/co/schedule.h"
#include "aimrt_module_cpp_interface/co/sync_wait.h"
#include "aimrt_module_cpp_interface/co/task.h"
#include "aimrt_module_cpp_interface/module_base.h"

#include "aimrt_module_protobuf_interface/util/protobuf_tools.h"
#include "core/aimrt_core.h"
#include "my_rpc.aimrt_rpc.pb.h" //生成的rpc头文件
#include <csignal>
#include <iostream>

aimrt::logger::LoggerRef global_logger;
void SetLogger(aimrt::logger::LoggerRef logger) { global_logger = logger; }
aimrt::logger::LoggerRef GetLogger() { return global_logger; }

// RPC服务端实例类
class MyRpcServiceImpl : public my_rpc::proto::MyServiceCoService {
public:
  MyRpcServiceImpl() = default;
  ~MyRpcServiceImpl() override = default;
  aimrt::co::Task<aimrt::rpc::Status>
  GetFooData(aimrt::rpc::ContextRef ctx_ref,
             const ::my_rpc::proto::GetFooDataReq &req,
             ::my_rpc::proto::GetFooDataRsp &rsp) override {
    rsp.set_msg("echo " + req.msg());

    AIMRT_INFO("Server handle new rpc call. req: {}, return rsp: {}",
               aimrt::Pb2CompactJson(req), aimrt::Pb2CompactJson(rsp));
    co_return aimrt::rpc::Status();
  }
};

// 服务端AimRT module
class NormalRpcSrvModule : public aimrt::ModuleBase {
public:
  NormalRpcSrvModule() = default;
  ~NormalRpcSrvModule() override = default;

  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "NormalRpcSrvModule"};
  }

  bool Initialize(aimrt::CoreRef core) override {
    core_ = core;
    SetLogger(core.GetLogger());
    // 注册rpc服务
    service_ptr_ = std::make_shared<MyRpcServiceImpl>();
    core_.GetRpcHandle().RegisterService(service_ptr_.get());
    return true;
  }

  bool Start() { return true; }

  void Shutdown() {}

private:
  aimrt::CoreRef core_;
  std::shared_ptr<MyRpcServiceImpl> service_ptr_;
};

// 客户端AimRT module
class NormalRpcCliModule : public aimrt::ModuleBase {
public:
  NormalRpcCliModule() = default;
  ~NormalRpcCliModule() override = default;

  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "NormalRpcCliModule"};
  }

  bool Initialize(aimrt::CoreRef core) override {
    core_ = core;
    SetLogger(core.GetLogger());
    // 注册rpc服务
    auto rpc_handle = core_.GetRpcHandle();
    my_rpc::proto::RegisterMyServiceClientFunc(rpc_handle);
    proxy_ = std::make_shared<my_rpc::proto::MyServiceCoProxy>(rpc_handle);
    executor_ = core_.GetExecutorManager().GetExecutor("work_thread_pool");
    return true;
  }

  bool Start() {

    // 开启协程
    scope_.spawn(aimrt::co::On(aimrt::co::InlineScheduler(), MainLoop()));
    return true;
  }

  aimrt::co::Task<void> MainLoop() {
    aimrt::co::AimRTScheduler work_thread_pool_scheduler(executor_);
    // 调度协程至当前执行器
    co_await aimrt::co::Schedule(work_thread_pool_scheduler);
    static int count = 0;
    while (run_flag_) {
      co_await aimrt::co::ScheduleAfter(work_thread_pool_scheduler,
                                        std::chrono::milliseconds(200));
      {
        // rpc请求体
        my_rpc::proto::GetFooDataReq req;
        // rpc响应体
        my_rpc::proto::GetFooDataRsp rsp;
        req.set_msg("hello world " + std::to_string(count++));
        // 设置ctx 3s 无响应就超时
        auto ctx_ptr = proxy_->NewContextSharedPtr();
        ctx_ptr->SetTimeout(std::chrono::seconds(3));
        // 发起RPC请求
        auto status = co_await proxy_->GetFooData(ctx_ptr, req, rsp);

        // 判断rpc请求是否成功
        if (status.OK()) {
          AIMRT_INFO("GetFooData success rsp: {}", rsp.msg());
        } else {
          AIMRT_WARN("Call GetFooData failed, status: {}", status.ToString());
        }
      }
    }
  }

  void Shutdown() {
    run_flag_ = false;
    aimrt::co::SyncWait(scope_.complete());
    AIMRT_INFO("Shutdown succeeded.");
  }

private:
  aimrt::CoreRef core_;
  aimrt::co::AsyncScope scope_;
  aimrt::executor::ExecutorRef executor_;
  std::atomic_bool run_flag_ = true;
  // 客户端代理句柄
  std::shared_ptr<my_rpc::proto::MyServiceCoProxy> proxy_;
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
    NormalRpcSrvModule normal_rpc_srv_module;
    NormalRpcCliModule normal_rpc_cli_module;
    core.GetModuleManager().RegisterModule(
        normal_rpc_srv_module.NativeHandle());
    core.GetModuleManager().RegisterModule(
        normal_rpc_cli_module.NativeHandle());
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
