# ImProFx - ShaderFX
RCSZ

- 所在目录: src\imgui_profx_src\improfx_core\

>- 2023_11_02 version 1.0.0 alpha
>- 2023_11_05 version 1.1.0 alpha

ShaderFX为框架核心部分.

```cpp
class CoreModuleImGui::RenderImGui;   // 框架ImGui调用
class CoreModuleWindow::RenderWindow; // 框架窗口
class FrameworkRender;                // 框架渲染
class ProfxSystemFramework;           // 框架启动
```

GLEW调用封装定义: src\imgui_profx_src\improfx_shader\

## 1.1.0A
> Shader的调用以及配置文本

### 配置文件结构
配置文件使用.json文件, RapidJSON库加载, 通常此框架需要两个必要核心配置文件"window_config.json"和"shader_config.json", 前者为配置GLFW窗口和ImGui的初始化, 后者为配置加载OpenGL着色器(特效部分).

首先是: "window_config.json"
```cpp
{
  "window_name": "ImProFX - V0.1",                      // GLFW窗口标题
  "window_size": [ 1536, 864 ],                         // GLFW窗口大小[ width,height ]
  "window_MSAA": 4,                                     // GLFW抗锯齿(MSAA)等级
  "window_fw_version": [ 3, 3 ],                        // GLFW版本配置 x.x
  "window_sh_version": "#version 330 core",             // ImGui配置着色器版本
  "imgui_fonts_file": "imgui_profx_system/corbelz.ttf", // ImGui加载字体(全局)
  "imgui_fonts_size": 18.5,                             // ImGui配置字体大小(全局)
  "imgui_fonts_color": [ 0.0, 1.0, 0.85, 1.0 ],         // ImGui配置字体颜色(全局)
  "imgui_window_rounding": 3.2,                         // ImGui窗口圆角程度
  "imgui_frame_rounding": 5.5,                          // ImGui帧控件圆角程度
  "shader_reg_file": "imgui_profx_system/config/shader_config.json" // 着色器配置文件路径
}
```

然后是: "shader_config.json" (对应以上"shader_reg_file"配置项).
```cpp
{
  "core_shaderfx_config": [ // 着色器包(package)数组
    {
      "shaderfx_package": "aaaxxx",                               // 包名称Key(Unique)
      "shader_file_vertex": "imgui_profx_system/shader/xxx.vs",   // 顶点着色器代码文件路径
      "shader_file_fragment": "imgui_profx_system/shader/xxx.fs", // 片元着色器代码文件路径
      "shader_fbo_texture_name": "xxx",                           // fbo附着的纹理名称(Unique)
      "matrix_ortho_cube": 10.0,                                  // 2D正交视图矩阵缩放(当前着色器程序[包])
      "render_max_fps": 200,                                      // 当前着色器渲染最大帧率(微秒级计时器)
      "render_max_resolution": [ 1,1 ],                           // 当前着色器渲染分辨率(如果为[ -1,-1 ]那么默认为窗口分辨率相同) 
      "shader_uniform_texture": [ // 着色器采样纹理(通过Uniform传入着色器的纹理)
        {
          "image_file_path": "xxx/xxx.png", // 纹理图片文件路径
          "image_ufh_name": "xxx"           // 着色器内"Uniform"名称(1i)
        }
      ],
      "shader_uniform_param": [ "aaa", "bbb" ] // 着色器额外"Uniform"参数名称(暂时全部类型为float)
    }
  ],
  "core_raw_texture_config": [ // 源(纯)图片纹理(不通过采样器传入着色器, ImGui直接通过句柄调用)
    {
      "image_file_path": "xxx/xxx.png", // 纹理图片文件路径
      "image_unique_name": "xxx"        // 纹理名称(调用时的Key)
    }
  ]
}
```
"core_shaderfx_config"对应框架内数据储存定义.
```cpp
// framework_render.h
std::unordered_map<std::string, FxRenderItem> CoreModuleRender::FrameworkRender::DataFxRender = {};
```
"core_raw_texture_config"对应框架内数据存储定义.
```cpp
// framework_render.h
std::unordered_map<std::string, FStextureHandle> CoreModuleRender::FrameworkRender::DataRawTexture = {};
```
简略流程: 加载着色器项(包)(每个包内一个独立着色器程序) -> 每个包通过FBO附着渲染到对应纹理"shader_fbo_texture_name"(非屏幕) -> 查询搜索对应包 -> 通过ImGui调用渲染到屏幕.

着色器中框架提供三个默认"Uniform"
```glsl
uniform mat4  system_mvp_matrix;  // Ortho2D MVP矩阵
uniform vec2  system_window_size; // 窗口大小
uniform float system_tick;        // 每个包的独立计时器(虚拟Tick)
```
---

### 调用&操作着色器
> namespace ImFxFind

查询获取渲染项(包), 详细"FxRenderItem"结构体定义查看"framework_define.hpp"
```cpp
// @param  "input" (输入"DataFxRender"), "text" (包名称"shaderfx_package"配置项)
// @return 渲染项(包)
FxRenderItem ImFxFind::FindRenderItem(std::unordered_map<std::string, FxRenderItem>& input, const char* text);
```

查询获取渲染项(包)FBO附着纹理(着色器渲染完成的图像).
```cpp
// @param  "input" (输入"DataFxRender"), "text" (包名称"shaderfx_package"配置项)
// @return OpenGL纹理句柄
uint32_t ImFxFind::FindRenderItemFBO(std::unordered_map<std::string, FxRenderItem>& input, const char* text);
```

查询获取设置渲染项(包)自定义"Uniform"变量float.
```cpp
// @param "input" (输入"DataFxRender"), "text" (包名称"shaderfx_package"配置项)
// @param "ufn" (变量名称"shader_uniform_param"配置项), "value" (设置值)
void ImFxFind::FindSetRenderItemParam(
    std::unordered_map<std::string, FxRenderItem>& input, 
    const char* text, 
    const char* ufn, 
    float       value
);
```

查询获取设置渲染项(包)渲染状态(是否运行此着色器的标志).
```cpp
// @param "input" (输入"DataFxRender"), "text" (包名称"shaderfx_package"配置项)
// @param "flag" (设置状态true:运行,false:暂停)
void ImFxFind::FindSetRenderItemFlag(std::unordered_map<std::string, FxRenderItem>& input, const char* text, bool flag);
```

查询获取Raw纹理项(与着色器无关"core_raw_texture_config"配置项).
```cpp
// @param  "input" (输入"DataRawTexture"), "text" (纹理名称"image_unique_name"配置项)
// @return OpenGL纹理句柄.
uint32_t ImFxFind::FindRawTextureImg(std::unordered_map<std::string, FStextureHandle>& input, const char* text);
```

框架内平滑采样系统值(缩放), 平滑采样参数设置在文件"framework_render_draw.cpp".
```cpp
float CoreModuleRender::FrameworkRender::TimerSmoothValue[1];
```

框架内获取设备纹理采样器数量(GPU Texture Mapping Unit).
```cpp
// 框架日志会在控制台打印TMU的数量 "[ogl_shader]: glinfo texture_max[units]:"
uint32_t CoreModuleRender::FrameworkRender::MaxNumberTMU;
```

### 调用静态标志
> namespace ImFxFind

静态标志(bool)对应框架内定义, 在每个Loop中不会改变(一般窗口状态等需要保证全局状态时调用).
```cpp
std::unordered_map<std::string, bool> CoreModuleRender::FrameworkRender::StaticFlag = {};
```

申请静态标志(bool), 可在Init和Loop部分.
```cpp
// @param "input" (输入"StaticFlag"), "text" (Key标识), "init" (初始标志)
// @param "REPEAT" (查重 true:如果遇到相同"text"[Key]则不覆盖写入)
void ImFxFind::StaticFlagAlloc(std::unordered_map<std::string, bool>& input, const char* text, bool init = false, bool REPEAT = false);
```

删除静态标志(bool) delete item(key-value).
```cpp
// @param "input" (输入"StaticFlag"), "text" (Key标识)
void ImFxFind::StaticFlagDelete(std::unordered_map<std::string, bool>& input, const char* text);
```

查询获取静态标志指针.
```cpp
// @param "input" (输入"StaticFlag"), "text" (Key标识)
// @return bool pointer
bool* ImFxFind::StaticFlagFind(std::unordered_map<std::string, bool>& input, const char* text);
```

---

- 着色器包内资源有自动回收
- 在窗口不可视的情况下尽量把该窗口内的渲染项暂停运行以减轻GPU负担
- 合理设置渲染条件和着色器最大帧率也能得到比较好的运行速度

 > 目前没用纹理合并所以并行纹理采样器, 所以全局纹理数量通过GL_MAX_TEXTURE_IMAGE_UNITS决定

Update 20231105
