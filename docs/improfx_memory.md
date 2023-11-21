# ImProFx - Memory
RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src\imgui_profx_src\improfx_memory\

> 2023_11_05 version 1.1.0 ahpla
> 2023_11_21 version 2.0.0 TEST

框架内动态内存分配&使用&释放.

## 2.0.0T
> namespace ImDynamicMem

动态内存对象定义.
```cpp
std::unordered_map<std::string, ImDynamicMem::ImMemory*> CoreModuleIMFX::FRMCORE_PACKAGE::DataDynamicMem;
```

动态内存基本模式和类型, 具体见注释.
```cpp
enum MemMode {
	SingPatt = 1, // 单例模式
	MultPatt = 2  // 多例模式
};
enum MemType {
	NULMEM    = -1, // 无操作
	Float32   = 0,  // float
	Float64   = 1,  // double
	Integer32 = 2,  // int32_t
	Integer64 = 3,  // int64_t
	Byte      = 4,  // uint8_t (usingned char)
	String    = 5   // std::string
};
```

### ImMemory
框架内动态内存对象, 如果不手动销毁则在框架生命周期结束时自动释放; 全部为vector封装, 类型参考以上"MemType".
```cpp
class ImDynamicMem::ImMemory();
```

分配(resize)动态内存.
```cpp
// @param "type" (分配操作类型), "length" (分配大小[长度])
void ImDynamicMem::ImMemory::AllocDynamicMem(MemType type, size_t length);
```

获取内存对象使用大小, bytes并非长度"length".
```cpp
// @param  "type" (查询操作类型)
// @return bytes_size
size_t ImDynamicMem::ImMemory::FineSizeDynamicMem(MemType type);

// 获取所有类型内存使用大小
// @return bytes_size
size_t FineSizeTotalDynamicMem();
```

清空释放动态内存(vector.clear + vector.shrink_to_fit).
```cpp
// @param "type" (释放操作类型)
void ImDynamicMem::ImMemory::DeleteDynamicMem(MemType type);
```

获取各类型指向动态内存的指针, 如果vector.size = 0将返回nullptr空指针.
```cpp
std::vector<float>*       ImDynamicMem::ImMemory::FindGetDynamicMemFloat32();
std::vector<double>*      ImDynamicMem::ImMemory::FindGetDynamicMemFloat64();
std::vector<int32_t>*     ImDynamicMem::ImMemory::FindGetDynamicMemInt32();
std::vector<int64_t>*     ImDynamicMem::ImMemory::FindGetDynamicMemInt64();
std::vector<uint8_t>*     ImDynamicMem::ImMemory::FindGetDynamicMemByte();
std::vector<std::string>* ImDynamicMem::ImMemory::FindGetDynamicMemString();
```

创建注册动态内存对象.
```cpp
// @param "input" (FrmDat.DataDynamicMem), "key" (Key标识)
// @param "type" (分配操作类型), "length" (预分配大小[长度]), "mode" (单例/多例模式)
void ImDynamicMem::DynamicMemCreateReg(
	std::unordered_map<std::string, ImMemory*> input,
	const char*                                key,
	MemType                                    type   = NULMEM,
	size_t                                     length = NULL,
	MemMode                                    mode   = SingPatt
);
```

动态内存对象调用.
```cpp
// @param  "input" (FrmDat.DataDynamicMem), "key" (Key标识)
// @return 指向内存对象的指针
ImMemory* ImDynamicMem::CallDynamicMem(std::unordered_map<std::string, ImMemory*> input, const char* key);
```

获取框架所有内存对象使用大小.
```cpp
// @param  "input" (FrmDat.DataDynamicMem)
// @return bytes_size
size_t ImDynamicMem::CountTotalObjectDynamicMem(std::unordered_map<std::string, ImMemory*> input);
```

Tools小工具"ImDynamicMem::Tools".
```cpp
// "vector"1维数组转2维矩阵(x*y)
template <typename T>
std::vector<std::vector<T>> ImDynamicMem::Tools::Vector1DConvertTo2D(const std::vector<T>& input, size_t x, size_t y);

// "vector"2维矩阵转1维数组(warn:数据一定要是矩阵).
template <typename T>
std::vector<T> ImDynamicMem::Tools::Vector2DConvertTo1D(const std::vector<std::vector<T>>& input);
```

---

简单Demo(框架内注册用户类中分配1024长度float单例内存):
```cpp
ImDynamicMem::DynamicMemCreateReg(FrmDat.DataDynamicMem, "MyMemory", ImDynamicMem::Float32, 1024);
```

---

- 暂时是非线程安全(已经在改了).

Update 20231121