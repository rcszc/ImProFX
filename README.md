# ImProFX

基于ImGui和OpenGL着色器来优化原版ImGui视觉效果的 __桌面轻量GUI开发框架__.

> - 爱发电: https://afdian.net/a/rcszvs
> - BiliBili: https://space.bilibili.com/475659860
> - QQ交流闲聊群: 862410678

__当前版本:__ __V2.1.0ALPHA__
__建议配置:__ __RTX3050__ __I5-10400__ __及以上__

- __好用的无依赖(框架)控件:__ [ImProFXcontrol](https://github.com/rcszc/ImProFXcontrol)
> 这些控件是和框架同步开发的, 但是无需依赖框架, 可以直接集成到自己的ImGui项目中 ~

---

__说明:__ 小版本主要进行模块优化&修改, 大版本架构会有大的更改或者推翻重写, 当前开发路线全部为早期版本~

- __公告:__ 2.1.0 ALPHA 因为是早期版本(可以交流学习但是使用就算了), 变动会比较大并且你在源码中可能看到文档上没有的东西, 那些是正在开发的部分 /滑稽, 希望你能友好的提出建议, __我也是在摸索中学习__
- __新增:__ 修改了内存对象, 新增引入第三方控件扩展, 以及动画菜单扩展(非第三方)
- __当前:__ 因为opengl耦合程度太高, 提前进行渲染抽象层开发...
> 具体参见Demo和文档, 预计2.2.0 ALPHA在2024年1~2月更新.

- __下一次大更新:__ 3.0.0 ALPHA 渲染抽象层(ImFX-GL), GUI控件抽象层(ImFX-UI)
> 预计: 2024年5月~9月 架构: IMPROFX-PM-IMFXGL 兼容图形底层: OpenGL Vulkan, 架构图会在这之前公布...

历史 2023-12-16 (6):
| Time | Version | Lines | Architecture | Graphical | Developers |
| :---: | :---: | :---: | :---: | :---: | :---: |
| 2023.11.02 | v1.0.0 Alpha | 2981 | IMPROFX-EA-OGL | OpenGL4.6 | RCSZ |
| 2023.11.06 | v1.1.0 Alpha | 3347 | IMPROFX-EA-OGL | OpenGL4.6 | RCSZ |
| 2023.11.21 | v2.0.0 Test-1 | 4857 | IMPROFX-ST-OGL | OpenGL4.6 | RCSZ |
| 2023.12.05 | v2.0.0 Test-2 | 6057 | IMPROFX-ST-OGL | OpenGL4.6 | RCSZ |
| 2023.12.08 | v2.0.0 Alpha | 7002 | IMPROFX-ST-OGL | OpenGL4.6 | RCSZ |
| 2023.12.16 | v2.1.0 Alpha | 7767 | IMPROFX-ST-OGL | OpenGL4.6 | RCSZ |

---
> 2024年可能会有一些朋友加入开发, 争取release 1.0版本总体量控制在3万行以内.

__IMPROFX-ST-OGL(架构)__
<img src="docs/arch/ImProFX20-ARCH.png"/>
<p align="center">
  <img src="docs/images/improfx_demo1.0A.png" style="width:24%;"/>
  <img src="docs/images/improfx_demo1.1A.png" style="width:24%;"/>
  <img src="docs/images/improfx_demo2.0A.png" style="width:24%;"/>
  <img src="docs/images/improfx_demo2.1A.png" style="width:24%;"/>
</p>

### 以下框架文档
> 建议按照顺序阅读

- [框架使用文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_usage.md)
- [框架核心文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_corefx.md)
- [框架动态信息文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_dyinfo.md)
- [框架ImGui动画控件文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_animation.md)
- [框架ImGui扩展控件文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_imguipro.md)
- [框架粒子系统文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_particle.md)
- [框架内存对象文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_memory.md)
- [框架线程池文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_threadpool.md)
- [框架扩展模块文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_extension.md)

### VisualStudio2022项目
开袋即食 /doge
- imgui-profx-vs2022.zip

第三方依赖:
- OpenGL GLFW3
- OpenGL GLEW
- ImGui
- RapidJSON
- stb_image
- 一些ImGui第三方控件(具体见第三方许可证)

> 开发测试设备: __CPU:__ I7-13700HX, E5-2683v3(*2) __GPU:__ RTX4060LabTop

> __PS:__ 趁着这几年在学校闲暇时间开发个自己的项目, 构思整个框架体系大概几周吧, 然后1.0-2.0版本重写了一次架构(后面还会有很多次重写), 相较于原版ImGui极大的提高了"舒适度", 再加上原生支持3D图形库可以提供更高抽象的着色器调用, ~~奈何我没有艺术细胞写的Demo难看死了~~, 也看到了很多dalao的渲染引擎争取以后能做底层兼容; 目前是属于塞了很多东西进去, 但是每样都是一个巨坑得慢慢填(比如着色器资源管理&反射等) /滑稽; 编程在于多写, 要说有啥意义我只能说造轮子永不过时.

---
（我菜佬勿喷
