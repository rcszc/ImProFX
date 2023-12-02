# ImProFX

基于ImGui和OpenGL着色器来提高原版ImGui视觉效果的GUI框架.

__建议配置:__ __RTX3050__ __I5-10400__ __及以上__

历史(具体见最后表格):
>- Version 1.0.0 Alpha 2023_11_02 RCSZ.
>- Version 1.1.0 Alpha 2023_11_06 RCSZ.
>- Version 2.0.0 Test-1 2023_11_21 RCSZ.

- __公告:__ 2.0.0 TEST版本为前瞻版本, 可能有很多Bug, Alpha版本图形底层暂时保留OpenGL还在慢慢学VK. (Vulkan给了我一点底层的小小震撼 /滑稽)
- __新增:__ 线程池, 反射, 内联着色器, 动态加载纹理, 异步任务(半残) 等. (具体参考文档)

- __当前:__ 主体架构已完成, 最近将发布Test-2.
> Test-2与Alpha版本最接近, 架构基本不会再有变动

<p align="center">
  <img src="docs/images/improfx_demo1.0.png" style="width:32%;"/>
  <img src="docs/images/improfx_demo1.1.png" style="width:32%;"/>
  <img src="docs/images/improfx_demo2.0T.png" style="width:32%;"/>
</p>

### 以下核心部分文档
> 建议按照顺序
- [框架使用文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_usage.md)
- [框架核心文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_corefx.md)
- [框架ImGui动画控件文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_animation.md)
- [框架ImGui扩展控件文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_imguipro.md)
- [框架内存对象文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_memory.md)
- [框架线程池文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_threadpool.md)
- [框架动态信息文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_dyinfo.md)

### VisualStudio2022项目
开袋即食 /doge
- imgui-profx-vs2022.zip

第三方依赖库:
- OpenGL GLFW3
- OpenGL GLEW
- ImGui
- RapidJSON
- stb_image

2023-11-21 (3)
| Version | Lines | Architecture | Graphical |
| :---: | :---: | :---: | :---: | :---: |
|v1.0.0 Alpha | 2981 | IMPROFX-EA-OGL | OpenGL4.6 |
|v1.1.0 Alpha | 3347 | IMPROFX-EA-OGL | OpenGL4.6 |
|v2.0.0 Test-1 | 4857 | IMPROFX-ST-OGL | OpenGL4.6 |

> 开发测试设备: CPU: I7-13700HX GPU: RTX4060LabTop

> 因为这学期因为闲暇时间非常多, 刚好以前一直在用ImGui, 结合OpenGL的Shader和一些自写的基于ImGui的扩展控件开发一个桌面GUI框架, 相较于原版ImGui极大的提高了"舒适度", 奈何我没有艺术细胞写的Demo难看死了hhh, 目前是属于塞了很多东西进去, 但是每样都是一个大坑得慢慢填 /滑稽; 编程在于多练习, 要说有啥意义我只能说造轮子永不过时.

---
（我菜佬勿喷
