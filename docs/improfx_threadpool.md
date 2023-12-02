# ImProFx - ThreadPool
RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src\imgui_profx_src\improfx_thread\

>- 2023_11_21 version 2.0.0 TEST

框架内简易线程池使用.

## 2.0.0T
> namespace ImDynamicMem (用不到)

线程池直接在框架内调用就可以, 无需创建销毁等.

线程池对象:
```cpp
class ImThreadTask::ThreadPool(32);
```

调用线程池: 添加任务, 改变工作线程数量, 获取正在排队任务数量:
```cpp
// 向线程池添加任务(类,初始化参数)线程池会自动创建一个对象
FrmDat.TaskThreadPool.PushTask<MyClass>(MyParam...);
// 函数原型:
template<typename InClass, typename... Args_param>
std::future<std::shared_ptr<InClass>> ImThreadTask::ThreadPool::PushTask(Args_param... Args);

// 重置线程池工作线程数量(warn:线程池会等待所有排队任务结束,释放掉当前所有线程,再重新创建指定数量线程)
FrmDat.TaskThreadPool.ResizeWorkers(Number);

// 获取当前线程池正在任务队列中排队的任务数量(不包括正在执行的任务)
uint32_t number = FrmDat.TaskThreadPool.QueueTasksCount();
```

在添加任务等地方可以使用"try"来获取错误信息(重写std::exception).
```cpp
try {
    ...
}
catch (ImThreadTask::error::ewp_error err) {
    err.what();
}
```

---

- 老项目直接缝合过来用的(后面应该会完善).

Update 20231121
