// imps_editor.
#include "imps_editor.h"

using namespace std;
using namespace ProFxFramework;

#define VALUE_CLAMP(value, vmin, vmax) value < vmin ? vmin : value && value > vmax ? vmax : value

bool IMPS_EditorMain::LogicInitialization(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat) {
	

	return true;
}

bool IMPS_EditorMain::LogicEventLoop(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat, CoreModuleIMFX::FRMCORE_INFO& FrmInfo) {

	FrmInfo.GuiFinalFxSettings.FxEnableBloom = true;
	FrmInfo.GuiFinalFxSettings.FxBloomRadius = 12;
	FrmInfo.GuiFinalFxSettings.FxBloomFilter = 0.32f;
	FrmInfo.GuiFinalFxSettings.FxBloomColorBlend.vector_x = 0.58f;
	FrmInfo.GuiFinalFxSettings.FxBloomColorBlend.vector_y = 1.72f;

	static ImVec4 DashColor = ImVec4(0.0f, 1.0f, 1.0f, 0.96f);
	FrmInfo.ImProFXGUI->ImGuiWindow("DevPanel", [&]() {

		ImGui::SliderFloat("Test", &DevTestValue, SetLimit.x, SetLimit.y);
		ImGui::InputFloat("Value", &DevTestValue);

		if (ImGui::InputFloat2("Limit", &SetLimit.x))
			SetLimit = DevTest.SetDashboardValueLimit(SetLimit);

		ImGui::Checkbox("Start", &DevTest.DashboardStart);
		ImGui::SameLine();
		ImGui::ColorEdit4("Color", &DashColor.x);

		ImGui::Button("TestAdd");
		if (ImGui::IsItemActive())
			DevTestValue += 0.25f;
		else
			DevTestValue -= 0.25f;
		DevTestValue = VALUE_CLAMP(DevTestValue, SetLimit.x, SetLimit.y);
		}
	);
	
	FrmInfo.ImProFXGUI->SetNextWindowSize(ImVec2(768.0f + IMGUI_ITEM_SPC * 2.0f, 768.0f + IMGUI_ITEM_SPC * 2.0f));
	FrmInfo.ImProFXGUI->ImGuiWindow("DevTest", [&]() {

		DevTest.DrawDashboardWindow("Dashboard", DevTestValue, 10, DashColor);

		}, (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar
	);

	return true;
}