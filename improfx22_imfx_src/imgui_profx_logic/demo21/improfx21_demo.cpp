// improfx21_demo.
#include "improfx21_demo.h"

using namespace std;
using namespace ProFxFramework;

bool ImprofxDemoClass21::LogicInitialization(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat) {

	FrmStatic.StaticTimerAlloc(FrmDat.StaticTimer, "TestUpWin");

	return true;
}

const vector<string> TestMenuItems = 
{ "ImProFX21", "Item.AA", "Item.BBBB", "Item.C", "Item.DDDDDDDD", "Item.EEE", "Item.FFFFF", "Item.GGGGGG", "END" };

std::vector<AnimCoordSample> TestAnimSample =
{
	AnimCoordSample(10.0f, 16.0f, -12.0f, 50.0f, -12.0f, -42.0f, 0.0f),
	AnimCoordSample(50.0f, -10.0f, 30.0f, 32.0f, 16.0f, -12.0f, 100.0f),
	AnimCoordSample(20.0f, -32.0f, -42.0f, 50.0f, -10.0f, 30.0f, 200.0f),
	AnimCoordSample(-3.0f, -42.0f, 20.0f, 20.0f, -10.0f, 30.0f, 300.0f),
	AnimCoordSample(50.0f, -10.0f, 30.0f, -20.0f, -32.0f, -42.0f, 400.0f),
	AnimCoordSample(20.0f, -32.0f, -42.0f, -3.0f, -42.0f, 33.0f, 500.0f)
};

void sPRO_RoundImage(
	const uint32_t& texture_hd,
	const ImVec2& position,
	const ImVec2& size,
	float           round = 5.8f,
	const ImVec2& uv_cropping = ImVec2(1.0f, 1.0f)
) {
	ImGui::GetWindowDrawList()->AddImageRounded(
		CVT_IMPTR(texture_hd),
		ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
		ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y),
		ImVec2(0.5f - uv_cropping.x / 2.0f, 0.5f - uv_cropping.y / 2.0f),
		ImVec2(0.5f + uv_cropping.x / 2.0f, 0.5f + uv_cropping.y / 2.0f),
		IM_COL32_WHITE,
		round
	);
}

bool ImprofxDemoClass21::LogicEventLoop(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat, CoreModuleIMFX::FRMCORE_INFO& FrmInfo) {
	// 施工测试现场 (非常非常乱

	ImGui::Begin("TestGuiBloom");
	{
		ImGui::Checkbox("EnableBloom", &FrmInfo.GuiFinalFxSettings.FxEnableBloom);
		ImGui::SliderInt("BloomRadius", &FrmInfo.GuiFinalFxSettings.FxBloomRadius, 1, 32);
		ImGui::SliderFloat2("BloomBlend", FrmInfo.GuiFinalFxSettings.FxBloomColorBlend.data(), 0.0f, 1.0f);
		ImGui::SliderFloat("Background", &BgColor, 0.0f, 1.0f);
		FrmCore.FindSetRenderItemParam(FrmDat.DataShaderFx, "fx_background_shader", "brightness", BgColor);
	}
	ImGui::End();

	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.2f, 0.2f, 0.2f, 0.92f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.2f, 0.2f, 0.2f, 0.92f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.2f, 0.2f, 0.2f, 0.92f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.12f, 0.92f));
	{
		ImGui::Begin("TestMenu Window", (bool*)0, WindowFlags);
		{
			ImGui::ColorEdit4("MenuItemColor", &MenuConfigItemColor.x);
			ImGui::ColorEdit4("MenuTextColor", &MenuConfigTextColor.x);

			ImGui::SliderFloat("MenuTextSize", &MenuConfigTextSize, 0.5f, 1.5f, "%.2f");

			ImGui::Checkbox("Fixed Window", &FixedWindowFlag);
			if (FixedWindowFlag)
				WindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
			else
				WindowFlags = 0;

			FrmInfo.ImProFXGUI->PRO_EndEnterLine();
			// 绘制菜单.
			TestMenu.DrawMenuWindow("TestMenu", TestMenuItems, MenuTypeCount, MenuConfigItemColor, ImVec2(220.0f, 270.0f), FrmInfo.FrameSmoothValue * 2.0f, MenuConfigTextSize);
			
			ImGui::SameLine();
			ImGui::Text(TestMenuItems[MenuTypeCount].c_str());
		}
		ImGui::End();
	}
	ImGui::PopStyleColor(4);

	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.2f, 0.2f, 0.2f, 0.92f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.2f, 0.2f, 0.2f, 0.92f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.2f, 0.2f, 0.2f, 0.92f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.12f, 0.92f));
	{
		TestNE.DrawEditorWindow(FrmInfo.GetUniqueID(), "AnimationEditor", 2000.0f, 0.42f, TestAnimSample, false);
	}
	ImGui::PopStyleColor(4);

	auto TestFunc = [&](CoordSystemInfo INFO) {

		ImVec2 RenderSize(1000.0f, 1000.0f);
		ImVec2 ImgPos(INFO.CenterPosition.x - RenderSize.x * 0.5f * INFO.ScaleCoord, INFO.CenterPosition.y - RenderSize.y * 0.5f * INFO.ScaleCoord);
		
		sPRO_RoundImage(
			FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "TestImage"),
			ImgPos,
			ImVec2(RenderSize.x * INFO.ScaleCoord, RenderSize.y * INFO.ScaleCoord)
		);
	};
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.2f, 0.2f, 0.2f, 0.92f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.2f, 0.2f, 0.2f, 0.92f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.2f, 0.2f, 0.2f, 0.92f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.12f, 0.92f));

	TestCoord.DrawEditorWindow(
		FrmInfo.GetUniqueID(), 
		"CoordEditor", 
		[](CoordSystemInfo INFO) {},
		CVT_IMPTR(FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "TestImage")),
		false
	);

	ImGui::PopStyleColor(4);

	return true;
}

void ImprofxDemoClass21::LogicCloseFree() {

}