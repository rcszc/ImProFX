# ImProFx - Memory
- [x] Update.20231216 RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src/imgui_profx_src/improfx_memory/

> 2023_11_05 version 1.1.0 Alpha
> 2023_11_21 version 2.0.0 Test-1
> 2023_12_02 version 2.0.0 Test-2
> 2023_12_08 version 2.0.0 Alpha
> 2023_12_16 version 2.1.0 Alpha

框架内动态内存 分配&使用&释放.

## 2.1.0A
> namespace ImDynamicMem

动态内存对象定义.
```cpp
std::unordered_map<std::string, IFC_MEMORY::ImMemoryBase*> CoreModuleIMFX::FRMCORE_PACKAGE::DataDynamicMem;
```

### ImMemory
框架内动态内存对象, 如果不手动销毁则在框架生命周期结束时自动释放, 全部为vector封装, 统一为"Byte"s类型(uint8_t).
> 目前看起来很"鸡肋", 主要给大量内存操作时提供一个监控
```cpp
class ImMemory :public IFC_MEMORY::ImMemoryBase;
```

类成员调用(一般"小"操作直接获取指针操作容器):
```cpp
// 大量内存push操作
// @param "ptr" (数据指针) "bytes" (数据字节大小)
void OperMemoryPush(uint8_t* ptr, size_t bytes);

// 重置内存容器大小
// @param "bytes" (重置大小)
void OperMemoryResize(size_t bytes);

// 擦除内存数据段
// @param "offset" (擦除起始位置) "bytes" (擦除大小)
void OperMemoryErase(size_t offset, size_t bytes);

// 内存状态结构体:
struct MemoryState {
	// 采样周期内: x:Push 操作次数 y:累计操作大小 KBytes
	Vector2T<float> PushOperRate;
	// 采样周期内: x:Erase 操作次数 y:累计操作大小 KBytes
	Vector2T<float> EraseOperRate;
	// 采样周期内: x:Resize 操作次数 y:累计增加大小 KBytes z:累计减少大小 KBytes
	Vector3T<float> ResizeOperRate;
};

// 获取内存状态(见以上结构体注释)
IFC_MEMORY::MemoryState* GetMemoryState();

// 获取vector容器指针.
std::vector<uint8_t>* PointerDynamicMem();
```

__创建&获取&销毁__

创建注册动态内存对象.
```cpp
// @param  "input" (FrmDat.DataDynamicMem), "key" (Key标识)
// @param  "bytes" (初始化分配内存大小) "sample_time" (以上所说内存状态刷新间隔, 单位毫秒)
// @return true:注册成功, false:注册失败
bool RegisterDyMemObject(
	std::unordered_map<std::string, IFC_MEMORY::ImMemoryBase*> input,
	const char*                                                key,
	size_t                                                     bytes       = NULL,
	uint32_t                                                   sample_time = DEFAULT_SMP_TIME
);
```

删除动态内存对象.
```cpp
// @param  "input" (FrmDat.DataDynamicMem), "key" (Key标识)
// @return true:删除成功, false:删除失败
bool DynamicMemObjectDelete(std::unordered_map<std::string, ImMemoryBase*> input, const char* key);
```

动态内存对象调用.
```cpp
// @param  "input" (FrmDat.DataDynamicMem), "key" (Key标识)
// @return 指向内存对象(接口)的指针
IFC_MEMORY::ImMemoryBase* FindGetDyMem(std::unordered_map<std::string, IFC_MEMORY::ImMemoryBase*> input, const char* key);
```

获取框架所有内存对象总大小.
```cpp
// @param  "input" (FrmDat.DataDynamicMem)
// @return bytes_size
size_t TotalDyMemSizeCount(std::unordered_map<std::string, IFC_MEMORY::ImMemoryBase*> input);
```

__Tools小工具:__ ImDynamicMem::Tools.
```cpp
// "vector"1维数组转2维矩阵(x*y)
template <typename T>
std::vector<std::vector<T>> ImDynamicMem::Tools::Vector1DConvertTo2D(const std::vector<T>& input, size_t x, size_t y);

// "vector"2维矩阵转1维数组(warn:数据一定要是矩阵).
template <typename T>
std::vector<T> ImDynamicMem::Tools::Vector2DConvertTo1D(const std::vector<std::vector<T>>& input);

// 浮点类型"vector"转bytes.
template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
std::vector<uint8_t> FloatConvertToBytes(const std::vector<T>& input)

// bytes转浮点类型"vector".
template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
std::vector<T> BytesConvertToFloat(const std::vector<uint8_t>& input)
```
---

__内存对象使用简单Demo:__
> 在框架内注册用户类中

分配131072长度内存, 监控采样时间5秒, 并且获取指针:
```cpp
ImDynamicMem::RegisterDyMemObject(FrmDat.DataDynamicMem, "MyMemory", 131072, 5000);

atuo MyMem = ImDynamicMem::FindGetDyMem(FrmDat.DataDynamicMem, "MyMemory");
auto ptr = MyMem->PointerDynamicMem();
```
---

- 目前全部改为bytes储存加入了监控, 后面还会修改.

```END```