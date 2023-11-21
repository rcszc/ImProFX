# ImProFX

基于ImGui和OpenGL着色器来提高原版ImGui视觉效果的GUI框架.

__建议配置:__ __RTX3050__ __I5-10400__ __及以上__

>- Version 1.0.0 Ahpla 2023_11_02 RCSZ.
>- Version 1.1.0 Ahpla 2023_11_06 RCSZ.
>- Version 2.0.0 Test  2023_11_21 RCSZ.

- 公告: 2.0.0 TEST版本为前瞻版本, 可能有很多Bug, Ahpla版本还在肝(还早 /滑稽)
- 预告: 2023除夕晚跨年争取2.0.0 Ahpla版本上线.

<p align="center">
  <img src="docs/images/improfx_demo1.0.png" style="width:32%;"/>
  <img src="docs/images/improfx_demo1.1.png" style="width:32%;"/>
  <img src="docs/images/improfx_demo2.0T.png" style="width:32%;"/>
</p>

### 以下核心部分文档
> 建议按照顺序
- [框架使用文档](https://github.com/rcszc/ImProFX/blob/main/docs/improfx_usgae.md)
- [框架核心文档](blob/main/docs/improfx_corefx.md)
- [框架ImGui动画控件文档](blob/main/docs/improfx_animation.md)
- [框架ImGui扩展控件文档](blob/main/docs/improfx_imguipro.md)
- [框架内存对象文档](blob/main/docs/improfx_memory.md)
- [框架线程池文档](blob/main/docs/improfx_threadpool.md)

### VisualStudio2022项目
开袋即食
- imgui-profx-vs2022.zip

第三方依赖库:
- OpenGL GLFW3
- OpenGL GLEW
- ImGui
- RapidJSON
- stb_image

2023-11-21 (3)
| Lines | Version | Device(GPU) | IDE |
| :---: | :---: | :---: | :---: |
| 2981 | v1.0.0a | RTX4060 Labtop | VisualStudio2022 |
| 3347 | v1.1.0a | RTX4060 Labtop | VisualStudio2022 |
| 4857 | v2.0.0t | RTX4060 Labtop | VisualStudio2022 |

> 因为这学期因为闲暇时间非常多, 刚好以前一直在用ImGui, 结合OpenGL的Shader和一些自写的基于ImGui的扩展控件开发一个桌面GUI框架, 相较于原版ImGui极大的提高了"舒适度", 奈何我没有艺术细胞写的Demo难看死了hhh, 目前是属于塞了很多东西进去, 但是每样都是一个大坑得慢慢填; 要说有啥意义我只能说造轮子永不过时.

---
（我菜佬勿喷
