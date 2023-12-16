// improfx21_demo.
#include "improfx21_demo.h"

using namespace std;

bool ImprofxDemoClass21::LogicInitialization(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat) {

	

	return true;
}

const vector<string> TestMenuItems = 
{ "ImProFX21", "Item.AA", "Item.BBBB", "Item.C", "Item.DDDDDDDD", "Item.EEE", "Item.FFFFF", "Item.GGGGGG", "END" };

bool ImprofxDemoClass21::LogicEventLoop(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat, CoreModuleIMFX::FRMCORE_INFO& FrmInfo) {

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

			ImGuiPro::EndEnterLine();
			ImGui::PushStyleColor(ImGuiCol_Text, MenuConfigTextColor);
			// ªÊ÷∆≤Àµ•.
			TestMenu.DrawMenuWindow("TestMenu", TestMenuItems, MenuTypeCount, MenuConfigItemColor, ImVec2(256.0f, 270.0f), MenuConfigTextSize);
			ImGui::PopStyleColor();

			ImGui::SameLine();
			ImGui::Text(TestMenuItems[MenuTypeCount].c_str());
		}
		ImGui::End();
	}
	ImGui::PopStyleColor(4);
	
	return true;
}

void ImprofxDemoClass21::LogicCloseFree() {

}