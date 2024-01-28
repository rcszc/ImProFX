## 当前停更三个月 框架在计划改革方案.

---

# ImProFX
__当前开发路线全部为早期实践版(更新速度非常快),__ __也希望你能友好的提出建议,__ __我也是在学习中摸索~__

基于ImGui和OpenGL着色器来优化原版ImGui视觉效果的 __桌面GUI开发轻量级框架__.
正式发行版预计: __2025 - 2026__

> - 爱发电: https://afdian.net/a/rcszvs
> - BiliBili: https://space.bilibili.com/475659860
> - 图形学QQ交流闲聊群: 862410678

__当前版本:__ __V2.2.0ALPHA__
__建议配置:__ __RTX3050__ __I5-10400__ __及以上__

- __好用的无依赖(框架)ImGui控件:__ [ImProFXcontrol](https://github.com/rcszc/ImProFXcontrol)
> 这些控件是和框架同步开发的, 但是无需依赖框架, 可以直接集成到自己的ImGui项目中~

---

__版本说明:__ 小版本(.x.x)主要进行模块优化&修改, 大版本(x.)架构会有大的更改或者推翻重写.

- __公告:__ 2.2.0 ALPHA 更新, 文档写好了(之前的改的)
- __新增:__ 渲染抽象层 IMFX-GL
- __当前:__ 完善各大模块, 优化着色器

---

- __下一次大更新:__ 3.0.0 ALPHA
> - 预计: 2024年5月~9月 架构: IMPROFX-PM-IMFXGL 架构图会在这之前公布...
> - 由于当前为早期开发路线所以历史版本不保留分支.

---

| Time | Version | Lines | Architecture | Graphical | Developers |
| :---: | :---: | :---: | :---: | :---: | :---: |
| 2024.01.10 | v2.2.0 ALPHA | 9745 | IMPROFX-ST-IMFXGL | OpenGL4.6 | RCSZ | 

[框架历史](VERSION.md)

__IMPROFX-ST-IMFXGL(架构)__
<img src="docs/arch/ImProFX23t-ARCH.png"/>

<p align="center">
  <img src="docs/images/improfx22a.png" style="width:92%;"/>
</p>

### 以下框架文档
> 包含9篇主要使用文档, 暂时无开发文档

[使用文档目录](docs/improfx_usage.md)

### VisualStudio2022项目
开袋即食 /doge
- imgui-profx22a-vs2022.zip

__第三方依赖:__
- OpenGL GLFW3
- OpenGL GLEW
- ImGui
- RapidJSON
- STB_IMAGE
- ImGui第三方控件 (具体见第三方许可证)

> 开发测试设备: __CPU:__ I7-13700HX, E5-2683v3(*2) __GPU:__ RTX4060LabTop

> __PS:__ QT以及一些其他的桌面GUI库的历史包袱太重了(个人观点), 趁着这几年在学校时间多学习开发一个自用桌面GUI框架, 构思整个框架体系大概几周吧, 然后1.0-2.0版本重写了一次架构(后面还会有很多次重写), 相较于原版ImGui极大的提高了"舒适度", 再加上原生支持3D图形库, 可以提供更高抽象的着色器调用以及底层资源管理(LLRES)回收, ~~奈何我没有艺术细胞写的Demo难看死了~~, 也看到了很多dalao的渲染引擎争取以后能做底层兼容; 目前是属于塞了很多东西进去, 但是每样都是一个巨坑得慢慢填(比如着色器资源管理&反射等) /滑稽; 要说有啥意义我只能说造轮子永不过时.

---
（我菜, 佬勿喷
