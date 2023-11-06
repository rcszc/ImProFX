# ImProFx - Memory
RCSZ

- 所在目录: src\imgui_profx_src\improfx_memory\

> 2023_11_05 version 1.1.0 ahpla

框架内动态内存分配使用释放.

## 1.1.0A
> namespace ImDynamicMem

动态内存对象对应框架内储存定义.
```cpp
std::unordered_map<std::string, ImDynamicMem::ImMemory*> CoreModuleRender::FrameworkRender::DataDynamicMem = {};
```

动态内存基本(操作)类型, 具体见注释.
```cpp
enum ImDynamicMem::MemType {
	NULLMEM   = -2, // 无选择.
	ALLMEM    = -1, // 所有动态内存类型
	Float32   = 0,  // float
	Float64   = 1,  // double
	Integer32 = 2,  // int32_t
	Integer64 = 3,  // int64_t
	Byte      = 4,  // unsigned char
	String    = 5   // std::string
};
```

### ImMemory
框架内动态内存对象, 如果不手动销毁则在框架生命周期结束时自动释放, 全部为vector, 类型参考以上"MemType".
```cpp
class ImDynamicMem::ImMemory();
```

分配(resize)动态内存.
```cpp
// @param "type" (分配类型操作), "length" (分配大小[长度])
void ImDynamicMem::ImMemory::AllocDynamicMem(MemType type, size_t length);
```

获取占用内存大小, bytes并非长度"length".
```cpp
// @param "type" (查询类型操作)
size_t ImDynamicMem::ImMemory::FineSizeDynamicMem(MemType type);
```

清空释放动态内存(vector.clear + vector.shrink_to_fit).
```cpp
// @param "type" (释放类型操作)
void ImDynamicMem::ImMemory::DeleteDynamicMem(MemType type);
```

获取各类型指向动态内存的指针, 如果vector.size == 0将返回nullptr空指针.
```cpp
std::vector<float>*       FindGetDynamicMemFloat32();
std::vector<double>*      FindGetDynamicMemFloat64();
std::vector<int32_t>*     FindGetDynamicMemInt32();
std::vector<int64_t>*     FindGetDynamicMemInt64();
std::vector<uint8_t>*     FindGetDynamicMemByte();
std::vector<std::string>* FindGetDynamicMemString();
```

创建注册动态内存对象.
```cpp
// @param "input" (输入"DataDynamicMem"), "key" (Key标识)
// @param "type" 分配操作类型, "length" 预分配大小(长度)
void ImDynamicMem::DynamicMemCreateReg(
	std::unordered_map<std::string, ImMemory*> input,
	const char*                                key,
	MemType                                    type = NULLMEM,
	size_t                                     length = NULL
);
```

动态内存对象调用.
```cpp
// @param "input" (输入"DataDynamicMem"), "key" (key标识)
// @return 指向内存对象的指针.
ImMemory* ImDynamicMem::CallDynamicMem(std::unordered_map<std::string, ImMemory*> input, const char* key)
```

---

- 动态内存对象有自动回收
- 具体参考"framework_memory.cpp"

Update 20231105