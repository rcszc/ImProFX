# ImProFx - Config
- [x] Update.20231216 RCSZ

[返回框架核心文档](improfx_corefx.md)
[返回使用文档](improfx_usage.md)

- 所在目录: imgui_profx_system/config/

> 2023_12_16 version 2.1.0 Alpha

框架的系统配置文件讲解说明(之前和"corefx"在一起现在独立出来).

## 2.1.0A
> 所有配置文件注册资源如果不手动删除, 都会在框架释放时自动回收

### 配置文件结构
- 配置文件类型使用 JSON 文件
- 配置文件解析使用 RapidJSON 库解析

通常此框架需要两个必要系统配置文件 __window_config.json__ 和 __shader_config.json__ 前者为GLFW窗口和ImGui的初始化配置等, 后者为配置加载OpenGL着色器文件&资源配置, 加载顺序为 window_config -> shader_config.

__window_config.json__ 具体参见注释:
```cpp
{
  "window_name": "ImProFX - V2.0 ALPHA",                             // 系统窗口标题名称
  "window_size": [ 1680, 945 ],                                      // 系统窗口大小
  "window_size_fixed": false,                                        // 固定窗口大小(运行时不可缩放)
  "window_full_screen": false,                                       // 开启全屏无框窗口
  "window_MSAA": 4,                                                  // 窗口渲染抗锯齿MSAA采样
  "window_fw_version": [ 3, 3 ],                                     // OpenGL GLFW 版本
  "window_sh_version": "#version 330 core",                          // OpenGL 着色器版本(按照GLSL格式)
  "window_bg_color": [ 0.5, 0.5, 0.5, 0.28 ],                        // 窗口渲染背景颜色(RGBA 0.0f-1.0f)
  "window_bg_shader": false,                                         // 启用窗口背景默认着色器[见:TEX1]
  "window_icon_file": "xxx/xxx.png",                                 // 窗口图标图片文件
  "imgui_fonts_file": "xxx/xxx.ttf",                                 // ImGui字体文件(.ttf)
  "imgui_fonts_size": 35.0,                                          // ImGui全局字体大小
  "imgui_fonts_color": [ 0.0, 1.0, 0.85, 1.0 ],                      // ImGui全局字体颜色(RGBA 0.0f-1.0f)
  "imgui_window_rounding": 5.2,                                      // ImGui窗口圆角值
  "imgui_frame_rounding": 5.5,                                       // ImGui帧控件圆角值
  "shader_reg_file": "imgui_profx_system/config/shader_config.json", // FX着色器配置文件路径(最好不要改)
  "system_create_workers": 32                                        // 框架线程池初始创建工作线程数量
}
```
__TEX1:__ 默认窗口背景着色器为: imgui_profx_system/shader/fx_background_shader .vert / .frag 框架会创建一个覆盖全窗口的ImGui无标题(ImGui)子窗口纹理作为背景渲染.

__shader_config.json__ 具体参见注释(文件路径对应以上 shader_reg_file 配置项):
```cpp
{
  "core_shaderfx_config": [ // FX着色器包 [JSON数组]
    {
      "shaderfx_package": "aaaxxx",                               // 名称(调用时唯一Key)
      "shader_file_vertex": "imgui_profx_system/shader/xxx.vs",   // 顶点着色器代码文件路径
      "shader_file_fragment": "imgui_profx_system/shader/xxx.fs", // 片元着色器代码文件路径
      "render_max_fps": 200.0,                                    // 当前着色器渲染最大帧率FPS(计时精度:微秒)
      "render_scale_xy": [ 1.0,1.0 ],                             // 当前着色器渲染缩放(XY比例)
      "shader_uniform_texture": [ // 着色器资源纹理(通过Uniform传入着色器的纹理, 占用全局TMU资源)
        {
          "image_file_path": "xxx/xxx.png", // 图片纹理文件路径
          "image_ufh_name": "xxx"           // 着色器内"Uniform"纹理名称(1i"sampler2D")
        }
      ],
      "shader_uniform_param": [ "aaa", "bbb" ] // 着色器其他"Uniform"参数名称(全部为Float类型)
    }
  ],
  "core_raw_texture_config": [ // 图片纹理, 与着色器无关, ImGui直接通过句柄调用 [JSON数组]
    {
      "image_file_path": "xxx/xxx.png", // 图片纹理文件路径
      "image_unique_name": "xxx"        // 纹理名称(调用时唯一Key)
    }
  ]
}
```

__core_shaderfx_config__ 框架内定义:
```cpp
// framework_core.hpp
std::unordered_map<std::string, FxRenderItem> CoreModuleIMFX::FRMCORE_PACKAGE::DataFxRender;
```

__core_raw_texture_config__ 框架内定义:
```cpp
// framework_core.hpp
std::unordered_map<std::string, FStextureHandle> CoreModuleIMFX::FRMCORE_PACKAGE::DataRawTexture;
```

__ShaderFX简略流程:__ 加载着色器(每个包内一个独立着色器程序) -> 每个着色器通过FBO附着渲染到附着纹理 __shader_fbo_texture_name__ (而非屏幕) -> 查询搜索对应包(Key) -> 通过ImGui调用纹理句柄渲染到GUI界面(屏幕).

着色器中框架提供三个默认"Uniform":
```glsl
uniform mat4  FxMatMvp;  // Ortho2D MVP矩阵
uniform vec2  FxWinSize; // 窗口大小
uniform float FxTime;    // 每个包的独立时间(虚拟Tick)
```

顶点着色器到片元着色器IO:
```glsl
// Vert => Frag
out vec4 FxColor; // 混合颜色(没用)
out vec2 FxCoord; // 绘制纹理坐标
// Frag => Out
out vec4 FragColor; // 输出片元
```
---

```END```