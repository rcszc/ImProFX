# ImProFx - ThreadPool
- [x] Update.20231216 RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src/imgui_profx_src/improfx_thread/

> 2023_11_21 version 2.0.0 Test-1
> 2023_12_02 version 2.0.0 Test-2
> 2023_12_08 version 2.0.0 Alpha
> 2023_12_16 version 2.1.0 Alpha

框架内简易线程池使用.

## 2.1.0A
> namespace ImThreadTask

线程池对象直接在框架内调用就可以, 无需创建销毁等.

线程池对象(线程池的初始大小通过"window_config"配置文件设置):
```cpp
class ImThreadTask::ThreadingPool(number);
```

调用线程池: 添加任务, 改变工作线程数量等(以Demo形式讲解说明):
```cpp
// 函数原型:
template<typename InClass, typename... Args_param>
std::future<std::shared_ptr<InClass>> ImThreadTask::ThreadingPool::PushTask(Args_param... Args);

// 向线程池添加任务(类,初始化参数)线程池会自动创建一个对象
FrmDat.TaskThreadPool.PushTask<MyClass>(MyParam...);

// 重置线程池工作线程数量(warn:线程池会等待所有排队任务结束,释放掉当前所有线程,再重新创建指定数量线程)
FrmDat.TaskThreadPool.ResizeWorkers(Number);

// 获取当前线程池正在任务队列中排队的任务数量(不包括正在执行的任务)
uint32_t number = FrmDat.TaskThreadPool.GetQueueTasks();

// 获取当前线程池正在工作的线程数量(正在执行的任务数量)
uint32_t number = FrmDat.TaskThreadPool.GetWorkingThreads();
```

获取当前线程池创建的任务对象信息:
```cpp
// 函数原型:
SYSTEMOBJINFO::RTTI_OBJINFO ImThreadTask::ThreadingPool::GetTaskObjectInfo();

// 返回结构体:
struct RTTI_OBJINFO {
    std::string ObjectName; // 对象名称
    std::size_t ObejctHash; // 对象哈希值
};

// 调用获取信息:
auto MyInfo = FrmDat.TaskThreadPool.GetTaskObjectInfo();
```

在添加任务等地方可以使用"try"来获取错误信息(重写std::exception).
```cpp
try {
    ...
}
catch (ImThreadTask::Error::TPerror err) {
    err.what();
}
```
---

- 老项目直接缝合过来用的 /滑稽.

```END```