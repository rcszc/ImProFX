// improfx2_demo.
#include "improfx2_demo.h"

using namespace std;
using namespace ProFxFramework;

const string AboutText = 
u8R"(ImProFX Framework
基于ImGui和OpenGL4.6
开发的轻量GUI框架
使用着色器提高了原版
的视觉效果,以及提供了
动画、粒子系统、
线程池、反射等开发工具.

边学习边开发中,一个GUI
框架需要非常长的时间
打磨,所以今后还需要经历
许多版本和架构的迭代...

- RCSZ 2024.01.10)";

const char* vArch[8][2] = {
	{ "1.0 Alpha", "EA-OGL" },
	{ "1.1 Alpah", "EA-OGL" },
	{ "2.0 Test1", "ST-OGL" },
	{ "2.0 Test2", "ST-OGL" },
	{ "2.0 Alpha", "ST-OGL" },
	{ "2.1 Alpha", "ST-OGL" },
    { "2.2 *ImFX", "ST-IMFXGL" },
	{ "2.2 Alpha", "ST-IMFXGL" }
};

bool ImprofxDemoClass22::LogicInitialization(FRMCORE_PACKAGE& FrmDat) {

	SecWindowIcon.push_back(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "icon_settings"));
	SecWindowIcon.push_back(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "icon_arch"));
	SecWindowIcon.push_back(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "icon_info"));

	DemoMainPage.push_back(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "demo2a_main"));
	DemoMainPage.push_back(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "demo2a_arch"));
	DemoMainPage.push_back(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "demo2a_core"));
	DemoMainPage.push_back(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "demo2a_next"));

	DemoMainPageIcon.push_back(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "demo2a_icon_shfx"));
	DemoMainPageIcon.push_back(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "demo2a_icon_anim"));
	DemoMainPageIcon.push_back(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "demo2a_icon_ptcs"));
	DemoMainPageIcon.push_back(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "demo2a_icon_tool"));

	ArchImage = FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "demo2a_arch_img");

	FrmStatic.StaticParticleSysAlloc(FrmDat.ParticleSystem, "DemoParticleSys", Vector2T<float>(1680.0f, 945.0f));

	ConfigAnimation(FrmDat);
	
	return true; // 这里demo懒得返回了.
}

void ImprofxDemoClass22::SecondaryWindow(FRMCORE_PACKAGE& FrmDat, FRMCORE_INFO& FrmInfo) {
	// 副窗口.
	FrmInfo.ImProFXGUI->SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC + WindowPageYpos.vector_y));
	// 子窗口.
	FrmInfo.ImProFXGUI->ImGuiWindowChild("AttachWindow",
		[&]() {
			FrmInfo.ImProFXGUI->SetCursorPosX(IMGUI_ITEM_SPC / 2.0f);
			FrmInfo.ImProFXGUI->Image(CVT_IMPTR(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "improfx2_logo")), ImVec2(88.0f, 88.0f));
			FrmInfo.ImProFXGUI->Text("");

			FrmInfo.ImProFXGUI->SetCursorPosX(IMGUI_ITEM_SPC - 2.0f);
			if (ImGuiProAnim::CallAnimButtonImage("img1", FrmDat.DataAnimation, "SecWinButA", FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "icon_settings"))) {
				SecWindowFlags.vector_x = !SecWindowFlags.vector_x;
				SecWindowFlags.vector_y = false;
				SecWindowFlags.vector_z = false;
			}

			FrmInfo.ImProFXGUI->SetCursorPosX(IMGUI_ITEM_SPC - 2.0f);
			if (ImGuiProAnim::CallAnimButtonImage("img2", FrmDat.DataAnimation, "SecWinButB", FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "icon_arch"))) {
				SecWindowFlags.vector_y = !SecWindowFlags.vector_y;
				SecWindowFlags.vector_x = false;
				SecWindowFlags.vector_z = false;
			}

			FrmInfo.ImProFXGUI->SetCursorPosX(IMGUI_ITEM_SPC - 2.0f);
			if (ImGuiProAnim::CallAnimButtonImage("img3", FrmDat.DataAnimation, "SecWinButC", FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "icon_info"))) {
				SecWindowFlags.vector_z = !SecWindowFlags.vector_z;
				SecWindowFlags.vector_x = false;
				SecWindowFlags.vector_y = false;
			}

			if (!SecWindowFlags.vector_z)
				AboutTextAnim = 0.0f;

			ImGuiProAnim::CallAnimFixedWindow("Settings", FrmDat.DataAnimation, "WindowA", SecWindowFlags.vector_x,
				[&]() {
					FrmInfo.ImProFXGUI->PRO_InterSliderFloat(u8"背景亮度", SettingsBgColor, ImVec2(0.5f, 1.5f), 0.42f);
					FrmInfo.ImProFXGUI->PRO_InterSliderFloat(u8"背景速度", SettingsBgSpeed, ImVec2(0.5f, 1.5f), 0.42f);

					FrmCore.FindSetRenderItemParam(FrmDat.DataShaderFx, "fx_background_shader", "brightness", SettingsBgColor.vector_y);
					FrmCore.FindSetRenderItemParam(FrmDat.DataShaderFx, "fx_background_shader", "speed", SettingsBgSpeed.vector_y);

					FrmInfo.ImProFXGUI->PRO_EndEnterLine();
					FrmInfo.ImProFXGUI->Text(u8"界面帧率(FPS): %.2f", FrmInfo.ImProFXGUI->GetImGuiIO().Framerate);

					// "ImGui::GetFrameCount()" 目前还没有封装.
					FrmInfo.ImProFXGUI->Text(u8"渲染帧数: %d", ImGui::GetFrameCount());

					FrmInfo.ImProFXGUI->Text(u8"鼠标坐标: %.0f, %.0f", FrmInfo.ImProFXGUI->GetMousePos().x, FrmInfo.ImProFXGUI->GetMousePos().y);

					if (FrmInfo.ImProFXGUI->Button("PageReturn") && WindowPageYpos.vector_x > 937.0f)
						WindowPageYpos.vector_x -= 945.0f;
				},
				ImGuiWindowFlags_NoScrollbar
			);

			ImGuiProAnim::CallAnimFixedWindow("Architecture", FrmDat.DataAnimation, "WindowB", SecWindowFlags.vector_y,
				[&]() {
					FrmInfo.ImProFXGUI->Text("Base: IMPROFX-");
					/*
					* 表格封装目前还未完善所以使用ImGui函数.
					* ImProFX 2.2.0 ALPHA.
					*/
					if (ImGui::BeginTable(u8"历史架构", 2, ImGuiTableFlags_Borders)) {
						// 表头.
						ImGui::TableSetupColumn(u8"版本", ImGuiTableColumnFlags_WidthFixed, 128.0f);
						ImGui::TableSetupColumn(u8"架构");
						ImGui::TableHeadersRow();

						for (int row = 0; row < 8; ++row) {
							ImGui::TableNextRow();
							for (int col = 0; col < 2; ++col) {
								ImGui::TableSetColumnIndex(col);
								ImGui::Text(vArch[row][col], row, col);
							}
						}
						ImGui::EndTable();
					}
					FrmInfo.ImProFXGUI->Text("RCSZ. 2024 - 2025.");
				},
				ImGuiWindowFlags_NoScrollbar
			);

			ImGuiProAnim::CallAnimFixedWindow("About", FrmDat.DataAnimation, "WindowC", SecWindowFlags.vector_z,
				[&]() {
					FrmInfo.ImProFXGUI->PRO_ViewTextMultilineAnim("#A3F7B2D8", AboutText, AboutTextAnim, SecWindowFlags.vector_z, ImVec2(240.0f, 342.0f), 1.8f);
				},
				ImGuiWindowFlags_NoScrollbar
			);
		},
		ImVec2(104.0f, 945.0f - IMGUI_ITEM_SPC * 2.0f)
	);
	FrmInfo.ImProFXGUI->SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC));
}

bool ImprofxDemoClass22::LogicEventLoop(FRMCORE_PACKAGE& FrmDat, FRMCORE_INFO& FrmInfo) {
	
	FrmInfo.ImProFXGUI->PushStyleColor(ImGuiCol_TitleBg, ImVec4(1.0f, 0.32f, 0.0f, 0.52f));
	FrmInfo.ImProFXGUI->PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0f, 0.32f, 0.0f, 0.52f));
	FrmInfo.ImProFXGUI->PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.32f));

	FrmInfo.ImProFXGUI->SetNextWindowPos(ImVec2(0, 0));
	FrmInfo.ImProFXGUI->SetNextWindowSize(FrmInfo.ImProFXGUI->GetImGuiIO().DisplaySize);

	// 无标题栏, 固定位置, 固定大小, 无法折叠, 隐藏滑条, 不接受鼠标滚轮事件.
	ImGuiWindowFlags TmpFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	FrmInfo.ImProFXGUI->ImGuiWindow("2.0 ALPHA", 
		[&]() {
			FrmInfo.ImProFXGUI->PopStyleColor(3);

			// 主页面.
			ImGui::SetCursorPos(ImVec2(FrmInfo.ImProFXGUI->PRO_ItemCalcCentered(1560.0f) + 50.0f + IMGUI_ITEM_SPC, IMGUI_ITEM_SPC));
			FrmInfo.ImProFXGUI->PRORZ_TemplatePageLSA("MemoMainPage", ImVec2(1560.0f, 877.5f), DemoMainPage, DemoMainPagePlayflags, DemoMainPageXpos, false);

			FrmInfo.ImProFXGUI->SetCursorPosX(FrmInfo.ImProFXGUI->PRO_ItemCalcCentered(768.0f + IMGUI_ITEM_SPC) + 50.0f + IMGUI_ITEM_SPC);
			DemoMainPagePlayflags.vector_x = ImGuiProAnim::CallAnimButton("PreviousPage", FrmDat.DataAnimation, "PalyButA");
			FrmInfo.ImProFXGUI->SameLine();
			DemoMainPagePlayflags.vector_y = ImGuiProAnim::CallAnimButton("NextPage", FrmDat.DataAnimation, "PlayButB");

			size_t PageCount = size_t(DemoMainPageXpos.vector_x / 1560.0f);

			ImGuiProAnimNodes::CallAnimNodesVector(PageCount, FrmDat.DataAnimationNodes, "NodesArch",
				NODESFUNC{
					FrmInfo.ImProFXGUI->Image(
						CVT_IMPTR(ArchImage),
						ImVec2(size.vector_x, size.vector_y),
						ImVec2(0.0f, 0.0f),
						ImVec2(1.0f, 1.0f),
						ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
					);
				}
			);

			ImGuiProAnimNodes::CallAnimNodesVector(PageCount, FrmDat.DataAnimationNodes, "NodesIcon1", NODESFUNC{
					FrmInfo.ImProFXGUI->Image(
						CVT_IMPTR(DemoMainPageIcon[0]),
						ImVec2(size.vector_x, size.vector_y),
						ImVec2(0.0f, 0.0f),
						ImVec2(1.0f, 1.0f),
						ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
					);
				}
			);
			ImGuiProAnimNodes::CallAnimNodesVector(PageCount, FrmDat.DataAnimationNodes, "NodesIcon2", NODESFUNC{
					FrmInfo.ImProFXGUI->Image(
						CVT_IMPTR(DemoMainPageIcon[1]),
						ImVec2(size.vector_x, size.vector_y),
						ImVec2(0.0f, 0.0f),
						ImVec2(1.0f, 1.0f),
						ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
					);
				}
			);
			ImGuiProAnimNodes::CallAnimNodesVector(PageCount, FrmDat.DataAnimationNodes, "NodesIcon3", NODESFUNC{
					FrmInfo.ImProFXGUI->Image(
						CVT_IMPTR(DemoMainPageIcon[2]),
						ImVec2(size.vector_x, size.vector_y),
						ImVec2(0.0f, 0.0f),
						ImVec2(1.0f, 1.0f),
						ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
					);
				}
			);
			ImGuiProAnimNodes::CallAnimNodesVector(PageCount, FrmDat.DataAnimationNodes, "NodesIcon4", NODESFUNC{
					FrmInfo.ImProFXGUI->Image(
						CVT_IMPTR(DemoMainPageIcon[3]),
						ImVec2(size.vector_x, size.vector_y),
						ImVec2(0.0f, 0.0f),
						ImVec2(1.0f, 1.0f),
						ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
					);
				}
			);

			ImGuiProAnimNodes::CallAnimNodesVector(PageCount, FrmDat.DataAnimationNodes, "NodesNext", NODESFUNC{
					FrmInfo.ImProFXGUI->Image(
						CVT_IMPTR(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "demo2a_button_next")),
						ImVec2(size.vector_x, size.vector_y),
						ImVec2(0.0f, 0.0f),
						ImVec2(1.0f, 1.0f),
						ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
					);
					if (FrmInfo.ImProFXGUI->IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped) && FrmInfo.ImProFXGUI->IsMouseDown(0))
						WindowPageYpos.vector_x += 945.0f;
				}
			);

			FrmInfo.ImProFXGUI->PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.32f, 0.0f, 0.92f));
			FrmInfo.ImProFXGUI->PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.55f, 0.0f, 1.0f, 0.32f));

			FrmInfo.ImProFXGUI->SetCursorPos(ImVec2(110.0f, ImGui::GetCursorPos().y + 4.0f));
			FrmInfo.ImProFXGUI->ImGuiWindowChild("NextPage1",
				[&]() {
					FrmInfo.ImProFXGUI->SetNextItemWidth(372.0f);
					FrmInfo.ImProFXGUI->InputFloat(InputFloat2, u8"粒子生命随机分布", ParticleConfigLife.data(), 0.0f, 0.0f, "%.2f");

					FrmInfo.ImProFXGUI->SetNextItemWidth(372.0f);
					FrmInfo.ImProFXGUI->InputFloat(InputFloat2, u8"粒子位置随机分布", ParticleConfigPosition.data(), 0.0f, 0.0f, "%.2f");

					FrmInfo.ImProFXGUI->SetNextItemWidth(372.0f);
					FrmInfo.ImProFXGUI->InputFloat(InputFloat2, u8"粒子向量随机分布", ParticleConfigVector.data(), 0.0f, 0.0f, "%.2f");

					FrmInfo.ImProFXGUI->SetNextItemWidth(240.0f);
					FrmInfo.ImProFXGUI->InputFloat(InputFloat1, u8"粒子数量", &ParticleNumber);

					FrmInfo.ImProFXGUI->SameLine();
					FrmInfo.ImProFXGUI->PRO_CheckboxTriangle("R", &ParticleConfigColorSys.vector_x);

					FrmInfo.ImProFXGUI->SameLine();
					FrmInfo.ImProFXGUI->PRO_CheckboxTriangle("G", &ParticleConfigColorSys.vector_y);

					FrmInfo.ImProFXGUI->SameLine();
					FrmInfo.ImProFXGUI->PRO_CheckboxTriangle("B", &ParticleConfigColorSys.vector_z);

					if (FrmInfo.ImProFXGUI->RadioButton(u8"扩散模式: 点阵", ParticleMode == 1 << 1)) ParticleMode = 1 << 1;
					if (FrmInfo.ImProFXGUI->RadioButton(u8"扩散模式: 圆形扩散[向量]", ParticleMode == 1 << 2)) ParticleMode = 1 << 2;
					if (FrmInfo.ImProFXGUI->RadioButton(u8"扩散模式: 矩形扩散[向量]", ParticleMode == 1 << 3)) ParticleMode = 1 << 3;
					if (FrmInfo.ImProFXGUI->RadioButton(u8"扩散模式: 飘落扩散[向量]", ParticleMode == 1 << 4)) ParticleMode = 1 << 4;

					auto TempPtcObj = FrmStatic.StaticParticleSysFind(FrmDat.ParticleSystem, "DemoParticleSys");
					TempPtcObj->ParticleSystemUpdate();

					if (ImGuiProAnim::CallAnimButton(u8"生成粒子", FrmDat.DataAnimation, "PlayButPTC")) {

						ImParticle::ImPTCgenerator GenParticles;

						GenParticles.ConfigCreateMode((ParticleCreateMode)ParticleMode);
						GenParticles.ConfigCreateNumber(ParticleNumber);
						GenParticles.ConfigLifeDispersion(ParticleConfigLife);
						GenParticles.ConfigRandomDispersion(ParticleConfigVector, ParticleConfigPosition, ParticleConfigColorSys);

						TempPtcObj->ParticleCreate(&GenParticles);
					}

					if (ImGuiProAnim::CallAnimButton("NextPage", FrmDat.DataAnimation, "PlayButNEXT"))
						WindowPageYpos.vector_x += 945.0f;

					FrmInfo.ImProFXGUI->SetCursorPos(ImVec2(528.0f, IMGUI_ITEM_SPC));
					FrmInfo.ImProFXGUI->Image(
						CVT_IMPTR(TempPtcObj->GetParticleRenderHandle()),
						ImVec2(1024.0f, 576.0f),
						ImVec2(0.0f, 0.0f),
						ImVec2(1.0f, 1.0f),
						ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
						ImVec4(0.55f, 0.0f, 1.0f, 0.85f)
					);

					FrmInfo.ImProFXGUI->ImGuiWindowChild("LogView",
						[&]() {
							FrmInfo.ImProFXGUI->PRO_SmoothYSlide(NextPageYpos);

							// "ImGui::SetWindowFontScale"还未封装.
							ImGui::SetWindowFontScale(0.872f);

							ImVec4 color = ImVec4();
							for (const auto& logstr : LOGCONS::ReadLogCache::ExtractLogSegment(128)) {

								switch (logstr.LogLabel) {
								case(LOG_INFO): { color = ImVec4(1.0f, 1.0f, 1.0f, 0.72f);  break; };
								case(LOG_ERROR): { color = ImVec4(1.0f, 0.0f, 0.0f, 0.72f);  break; };
								case(LOG_WARNING): { color = ImVec4(1.0f, 1.0f, 0.0f, 0.72f);  break; };
								case(LOG_TRACE): { color = ImVec4(0.0f, 1.0f, 1.0f, 0.72f);  break; };
								case(LOG_PERF): { color = ImVec4(0.78f, 0.0f, 1.0f, 0.72f); break; };
								}
								FrmInfo.ImProFXGUI->TextColored(color, logstr.LogString.c_str());
							}
						},
						ImVec2(1544.0f, 328.0f), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
					);
				},
				ImVec2(1560.0f, 945.0f), true
			);

			FrmInfo.ImProFXGUI->PopStyleColor(2);

			FrmInfo.ImProFXGUI->SetCursorPosX(110.0f);
			FrmInfo.ImProFXGUI->ImGuiWindowChild("NextPage2",
				[&]() {
					FrmInfo.ImProFXGUI->Image(CVT_IMPTR(FrmCore.FindRenderItemFBO(FrmDat.DataShaderFx, "fx_circle_shader")), ImVec2(768.0f, 432.0f));
					FrmInfo.ImProFXGUI->SameLine();
					FrmInfo.ImProFXGUI->Image(CVT_IMPTR(FrmCore.FindRenderItemFBO(FrmDat.DataShaderFx, "fx_hexerino_shader")), ImVec2(768.0f, 432.0f));

					FrmInfo.ImProFXGUI->PRO_EndEnterLine();
					ImVec2 temp_curpos = FrmInfo.ImProFXGUI->GetCursorPos();

					FrmInfo.ImProFXGUI->PRO_TextTitleP1("Hello World! P1", 384.0f);
					FrmInfo.ImProFXGUI->PRO_TextTitleP2("Hello World! P2", 384.0f);
					FrmInfo.ImProFXGUI->PRO_TextTitleP3("Hello World! P3", 384.0f);
					FrmInfo.ImProFXGUI->TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "Hello World!");

#define NULLFUNC [](){}
					FrmInfo.ImProFXGUI->SetCursorPos(ImVec2(384.0f - IMGUI_ITEM_SPC * 3.0f, temp_curpos.y));
					FrmInfo.ImProFXGUI->PRORZ_TemplatePageLayout169A("DemoT1", NULLFUNC, NULLFUNC, NULLFUNC, NULLFUNC, 0.5f);

					FrmInfo.ImProFXGUI->SameLine();
					FrmInfo.ImProFXGUI->PRORZ_TemplatePageLayout169B("DemoT2", NULLFUNC, NULLFUNC, NULLFUNC, NULLFUNC, 0.5f);

					FrmInfo.ImProFXGUI->SameLine();
					FrmInfo.ImProFXGUI->PRORZ_TemplatePageLayout1216A("DemoT3", NULLFUNC, NULLFUNC, NULLFUNC, 0.5f);
				},
				ImVec2(1560.0f, 945.0f), true
			);

			SecondaryWindow(FrmDat, FrmInfo);
			FrmInfo.ImProFXGUI->PRO_SmoothYSlide(WindowPageYpos, 0.42f, false, false);
		}, 
		nullptr, TmpFlags
	);
	return true;
}

void ImprofxDemoClass22::LogicCloseFree() {
	
}