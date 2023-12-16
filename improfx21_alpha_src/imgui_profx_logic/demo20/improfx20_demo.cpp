// improfx2_demo.
#include "improfx20_demo.h"

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

- RCSZ 2023.12.07)";

const char* vArch[5][2] = {
	{ "1.0 Alpha", "EA-OGL" },
	{ "1.1 Alpah", "EA-OGL" },
	{ "2.0 Test-1","ST-OGL" },
	{ "2.0 Test-2","ST-OGL" },
	{ "2.0 Alpha", "ST-OGL" }
};

bool ImprofxDemoClass20::LogicInitialization(FRMCORE_PACKAGE& FrmDat) {

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
	
	return true;
}

void ImprofxDemoClass20::SecondaryWindow(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat) {
	// 副窗口.
	ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC + WindowPageYpos.vector_y));
	ImGui::BeginChild("AttachWindow", ImVec2(104.0f, 945.0f - IMGUI_ITEM_SPC * 2.0f));
	{
		ImGui::SetCursorPosX(IMGUI_ITEM_SPC / 2.0f);
		ImGui::Image(CVT_IMPTR(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "improfx2_logo")), ImVec2(88.0f, 88.0f));
		ImGui::Text("");

		ImGui::SetCursorPosX(IMGUI_ITEM_SPC - 2.0f);
		if (ImGuiProAnim::CallAnimButtonImage("img1", FrmDat.DataAnimation, "SecWinButA", FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "icon_settings"))) {
			SecWindowFlags.vector_x = !SecWindowFlags.vector_x;
			SecWindowFlags.vector_y = false;
			SecWindowFlags.vector_z = false;
		}

		ImGui::SetCursorPosX(IMGUI_ITEM_SPC - 2.0f);
		if (ImGuiProAnim::CallAnimButtonImage("img2", FrmDat.DataAnimation, "SecWinButB", FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "icon_arch"))) {
			SecWindowFlags.vector_y = !SecWindowFlags.vector_y;
			SecWindowFlags.vector_x = false;
			SecWindowFlags.vector_z = false;
		}

		ImGui::SetCursorPosX(IMGUI_ITEM_SPC - 2.0f);
		if (ImGuiProAnim::CallAnimButtonImage("img3", FrmDat.DataAnimation, "SecWinButC", FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "icon_info"))) {
			SecWindowFlags.vector_z = !SecWindowFlags.vector_z;
			SecWindowFlags.vector_x = false;
			SecWindowFlags.vector_y = false;
		}

		if (!SecWindowFlags.vector_z)
			AboutTextAnim = 0.0f;

		ImGuiProAnim::CallAnimFixedWindow("Settings", FrmDat.DataAnimation, "WindowA", SecWindowFlags.vector_x,
			[&]() {
				ImGuiPro::InterSliderFloat(u8"背景亮度", SettingsBgColor, ImVec2(0.5f, 1.5f), 0.42f);
				ImGuiPro::InterSliderFloat(u8"背景速度", SettingsBgSpeed, ImVec2(0.5f, 1.5f), 0.42f);

				FrmCore.FindSetRenderItemParam(FrmDat.DataShaderFx, "fx_background_shader", "brightness", SettingsBgColor.vector_y);
				FrmCore.FindSetRenderItemParam(FrmDat.DataShaderFx, "fx_background_shader", "speed", SettingsBgSpeed.vector_y);

				ImGuiPro::EndEnterLine();
				ImGui::Text(u8"界面帧率(FPS): %.2f", ImGui::GetIO().Framerate);
				ImGui::Text(u8"渲染帧数: %d", ImGui::GetFrameCount());
				ImGui::Text(u8"鼠标坐标: %.0f, %.0f", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

				if (ImGui::Button("PageReturn") && WindowPageYpos.vector_x > 937.0f)
					WindowPageYpos.vector_x -= 945.0f;
			},
			ImGuiWindowFlags_NoScrollbar
		);

		ImGuiProAnim::CallAnimFixedWindow("Architecture", FrmDat.DataAnimation, "WindowB", SecWindowFlags.vector_y,
			[&]() {
				ImGui::Text("Base: IMPROFX-");
				if (ImGui::BeginTable(u8"历史架构", 2, ImGuiTableFlags_Borders)) {
					// 表头.
					ImGui::TableSetupColumn(u8"版本", ImGuiTableColumnFlags_WidthFixed, 128.0f);
					ImGui::TableSetupColumn(u8"架构");
					ImGui::TableHeadersRow();

					for (int row = 0; row < 5; ++row) {
						ImGui::TableNextRow();
						for (int col = 0; col < 2; ++col) {
							ImGui::TableSetColumnIndex(col);
							ImGui::Text(vArch[row][col], row, col);
						}
					}
					ImGui::EndTable();
				}
				ImGui::Text("RCSZ. 2023 - 2024.");
			},
			ImGuiWindowFlags_NoScrollbar
			);

		ImGuiProAnim::CallAnimFixedWindow("About", FrmDat.DataAnimation, "WindowC", SecWindowFlags.vector_z,
			[&]() {
				ImGuiPro::ViewTextMultilineAnim("#A3F7B2D8", AboutText, AboutTextAnim, SecWindowFlags.vector_z, ImVec2(240.0f, 342.0f), 1.8f);
			},
			ImGuiWindowFlags_NoScrollbar
		);
	}
	ImGui::EndChild();
	ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC));
}

bool ImprofxDemoClass20::LogicEventLoop(FRMCORE_PACKAGE& FrmDat, FRMCORE_INFO& FrmInfo) {
	
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(1.0f, 0.32f, 0.0f, 0.52f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0f, 0.32f, 0.0f, 0.52f));

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

	// 无标题栏, 固定位置, 固定大小, 无法折叠, 隐藏滑条, 不接受鼠标滚轮事件.
	ImGuiWindowFlags TmpFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::Begin("2.0 ALPHA", nullptr, TmpFlags);
	ImGuiPro::FullWindowBackground(FrmCore.FindRenderItemFBO(FrmDat.DataShaderFx, "fx_background_shader"), ImVec2(0.85f, 1.0f), 8.0f);
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		// 主页面.
		ImGui::SetCursorPos(ImVec2(ImGuiPro::ItemCalcCentered(1560.0f) + 50.0f + IMGUI_ITEM_SPC, IMGUI_ITEM_SPC));
		ImGuiProRZ::TemplatePageLSA("MemoMainPage", ImVec2(1560.0f, 877.5f), DemoMainPage, DemoMainPagePlayflags, DemoMainPageXpos, false);

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(768.0f + IMGUI_ITEM_SPC) + 50.0f + IMGUI_ITEM_SPC);
		DemoMainPagePlayflags.vector_x = ImGuiProAnim::CallAnimButton("PreviousPage", FrmDat.DataAnimation, "PalyButA");
		ImGui::SameLine();
		DemoMainPagePlayflags.vector_y = ImGuiProAnim::CallAnimButton("NextPage", FrmDat.DataAnimation, "PlayButB");

		size_t PageCount = size_t(DemoMainPageXpos.vector_x / 1560.0f);
		
		ImGuiProAnimNodes::CallAnimNodesVector(PageCount, FrmDat.DataAnimationNodes, "NodesArch",
			NODESFUNC{
				ImGui::Image(
					CVT_IMPTR(ArchImage),
					ImVec2(size.vector_x, size.vector_y),
					ImVec2(0.0f, 0.0f),
					ImVec2(1.0f, 1.0f),
					ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
				);
			}
		);
		
		ImGuiProAnimNodes::CallAnimNodesVector(PageCount, FrmDat.DataAnimationNodes, "NodesIcon1", NODESFUNC{
				ImGui::Image(
					CVT_IMPTR(DemoMainPageIcon[0]),
					ImVec2(size.vector_x, size.vector_y),
					ImVec2(0.0f, 0.0f),
					ImVec2(1.0f, 1.0f),
					ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
				);
			}
		);
		ImGuiProAnimNodes::CallAnimNodesVector(PageCount, FrmDat.DataAnimationNodes, "NodesIcon2", NODESFUNC{
				ImGui::Image(
					CVT_IMPTR(DemoMainPageIcon[1]),
					ImVec2(size.vector_x, size.vector_y),
					ImVec2(0.0f, 0.0f),
					ImVec2(1.0f, 1.0f),
					ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
				);
			}
		);
		ImGuiProAnimNodes::CallAnimNodesVector(PageCount, FrmDat.DataAnimationNodes, "NodesIcon3", NODESFUNC{
				ImGui::Image(
					CVT_IMPTR(DemoMainPageIcon[2]),
					ImVec2(size.vector_x, size.vector_y),
					ImVec2(0.0f, 0.0f),
					ImVec2(1.0f, 1.0f),
					ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
				);
			}
		);
		ImGuiProAnimNodes::CallAnimNodesVector(PageCount, FrmDat.DataAnimationNodes, "NodesIcon4", NODESFUNC{
				ImGui::Image(
					CVT_IMPTR(DemoMainPageIcon[3]),
					ImVec2(size.vector_x, size.vector_y),
					ImVec2(0.0f, 0.0f),
					ImVec2(1.0f, 1.0f),
					ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
				);
			}
		);

		ImGuiProAnimNodes::CallAnimNodesVector(PageCount, FrmDat.DataAnimationNodes, "NodesNext", NODESFUNC{
				ImGui::Image(
					CVT_IMPTR(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "demo2a_button_next")),
					ImVec2(size.vector_x, size.vector_y),
					ImVec2(0.0f, 0.0f),
					ImVec2(1.0f, 1.0f),
					ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
				);
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped) && ImGui::IsMouseDown(0))
					WindowPageYpos.vector_x += 945.0f;
			}
		);
		
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.32f, 0.0f, 0.92f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.55f, 0.0f, 1.0f, 0.32f));

		ImGui::SetCursorPos(ImVec2(110.0f, ImGui::GetCursorPos().y + 4.0f));
		ImGui::BeginChild("NextPage1", ImVec2(1560.0f, 945.0f), true);
		{
			ImGui::SetNextItemWidth(372.0f);
			ImGui::InputFloat2(u8"粒子生命随机分布", ParticleConfigLife.data(), "%.2f");
			ImGui::SetNextItemWidth(372.0f);
			ImGui::InputFloat2(u8"粒子位置随机分布", ParticleConfigPosition.data(), "%.2f");
			ImGui::SetNextItemWidth(372.0f);
			ImGui::InputFloat2(u8"粒子向量随机分布", ParticleConfigVector.data(), "%.2f");

			ImGui::SetNextItemWidth(240.0f);
			ImGui::InputFloat(u8"粒子数量", &ParticleNumber);
			ImGui::SameLine();
			ImGuiPro::CheckboxTriangle("R", &ParticleConfigColorSys.vector_x);
			ImGui::SameLine();
			ImGuiPro::CheckboxTriangle("G", &ParticleConfigColorSys.vector_y);
			ImGui::SameLine();
			ImGuiPro::CheckboxTriangle("B", &ParticleConfigColorSys.vector_z);

			if (ImGui::RadioButton(u8"扩散模式: 点阵", ParticleMode == 1 << 1)) ParticleMode = 1 << 1;
			if (ImGui::RadioButton(u8"扩散模式: 圆形扩散[向量]", ParticleMode == 1 << 2)) ParticleMode = 1 << 2;
			if (ImGui::RadioButton(u8"扩散模式: 矩形扩散[向量]", ParticleMode == 1 << 3)) ParticleMode = 1 << 3;
			if (ImGui::RadioButton(u8"扩散模式: 飘落扩散[向量]", ParticleMode == 1 << 4)) ParticleMode = 1 << 4;

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

			ImGui::SetCursorPos(ImVec2(528.0f, IMGUI_ITEM_SPC));
			ImGui::Image(
				CVT_IMPTR(TempPtcObj->GetParticleRenderHandle()),
				ImVec2(1024.0f, 576.0f),
				ImVec2(0.0f, 0.0f),
				ImVec2(1.0f, 1.0f),
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
				ImVec4(0.55f, 0.0f, 1.0f, 0.85f)
			);

			ImGui::BeginChild("LogView", ImVec2(1544.0f, 328.0f));
			{
				ImGuiPro::SmoothYSlide(NextPageYpos);

				ImGui::SetWindowFontScale(0.872f);
				ImVec4 color = ImVec4();
				for (const auto& logstr : LOGCONS::ReadLogCache::ExtractLogSegment(128)) {

					switch (logstr.LogLabel) {
					case(LOG_INFO): { color = ImVec4(1.0f, 1.0f, 1.0f, 0.72f);  break; };
					case(LOG_ERR):  { color = ImVec4(1.0f, 0.0f, 0.0f, 0.72f);  break; };
					case(LOG_WARN): { color = ImVec4(1.0f, 1.0f, 0.0f, 0.72f);  break; };
					case(LOG_TRC):  { color = ImVec4(0.0f, 1.0f, 1.0f, 0.72f);  break; };
					case(LOG_PERF): { color = ImVec4(0.78f, 0.0f, 1.0f, 0.72f); break; };
					}
					ImGui::TextColored(color, logstr.LogString.c_str());
				}
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::SetCursorPosX(110.0f);
		ImGui::BeginChild("NextPage2", ImVec2(1560.0f, 945.0f), true);
		{
			ImGui::Image(CVT_IMPTR(FrmCore.FindRenderItemFBO(FrmDat.DataShaderFx, "fx_circle_shader")), ImVec2(768.0f, 432.0f));
			ImGui::SameLine();
			ImGui::Image(CVT_IMPTR(FrmCore.FindRenderItemFBO(FrmDat.DataShaderFx, "fx_hexerino_shader")), ImVec2(768.0f, 432.0f));

			ImGuiPro::EndEnterLine();
			ImVec2 temp_curpos = ImGui::GetCursorPos();

			ImGuiPro::TextTitleP1("Hello World! P1", 384.0f);
			ImGuiPro::TextTitleP2("Hello World! P2", 384.0f);
			ImGuiPro::TextTitleP3("Hello World! P3", 384.0f);
			ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "Hello World!");

#define NULLFUNC [](){}
			ImGui::SetCursorPos(ImVec2(384.0f - IMGUI_ITEM_SPC * 3.0f, temp_curpos.y));

			ImGuiProRZ::TemplatePageApp169A("DemoT1", NULLFUNC, NULLFUNC, NULLFUNC, NULLFUNC, 0.5f);
			ImGui::SameLine();
			ImGuiProRZ::TemplatePageApp169B("DemoT2", NULLFUNC, NULLFUNC, NULLFUNC, NULLFUNC, 0.5f);
			ImGui::SameLine();
			ImGuiProRZ::TemplatePageApp1216A("DemoT3", NULLFUNC, NULLFUNC, NULLFUNC, 0.5f);
		}
		ImGui::EndChild();

		SecondaryWindow(FrmDat);
		ImGuiPro::SmoothYSlide(WindowPageYpos, 0.42f, false, false);
	}
	ImGui::End();

	return true;
}

void ImprofxDemoClass20::LogicCloseFree() {
	
}