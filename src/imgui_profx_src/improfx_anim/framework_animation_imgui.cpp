// framework_animation_imgui.
#include <typeinfo>

#include "framework_animation.hpp"

#include "imgui_glfw/imgui.h"
#include "imgui_glfw/imgui_impl_glfw.h"
#include "imgui_glfw/imgui_impl_opengl3.h"

using namespace std;
using namespace LOGCONS;

template <typename base, typename derived>
bool IsInstanceOf(const base* base_ptr) {
	const derived* derived_ptr = dynamic_cast<const derived*>(base_ptr);
	return derived_ptr != nullptr;
}

ImGuiProAnim::ImAnimationBase* FindAnimComp(unordered_map<string, ImGuiProAnim::ImAnimationBase*>& input, const char* text) {
	return ((input.find(text) != input.end()) ? input[text] : nullptr);
}

namespace ImGuiProAnim {

	void AnimCompReg(unordered_map<string, ImAnimationBase*>& input, const char* key, ImAnimationBase* comp) {
		if (comp) {
			comp->comp_init();
			input[key] = comp;

			const type_info& objinfo = typeid(*comp);
			LogInfoPush(LOG_TRC, SYSTEM_MODULE_LABLE_animation + "register comp: " + key);
			LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_animation + "register obj_rtti: " + objinfo.name());
		}
		else
			LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_animation + "failed register comp nullptr.");
	}

	ImAnimButton* CreateAnimButton(ImAnimButton* copy) {
		ImAnimButton* CompCreate = new ImAnimButton();

		if (copy) {
			CompCreate->config_normal_color = copy->config_normal_color;
			CompCreate->config_hover_color = copy->config_hover_color;
			CompCreate->config_active_color = copy->config_active_color;

			CompCreate->config_normal_size = copy->config_normal_size;
			CompCreate->config_hover_size = copy->config_hover_size;
			CompCreate->config_active_size = copy->config_active_size;
		}
		else {
			ImVec4 ColTmp = ImGui::GetStyleColorVec4(ImGuiCol_Button);
			CompCreate->config_normal_color = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);

			ColTmp = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
			CompCreate->config_hover_color = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);

			ColTmp = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
			CompCreate->config_active_color = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);

			// imgui system preset button_size.
			CompCreate->config_normal_size = Vector2T<float>(128.0f, 42.0f);
			CompCreate->config_hover_size = Vector2T<float>(128.0f, 42.0f);
			CompCreate->config_active_size = Vector2T<float>(128.0f, 42.0f);
		}
		return CompCreate;
	}

	bool CallAnimButton(const char* unique_name, unordered_map<string, ImAnimationBase*>& input, const char* comp_name, float trans_smooth) {
		const auto& comp = FindAnimComp(input, comp_name);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(comp->animtrans_color().vector_x, comp->animtrans_color().vector_y, comp->animtrans_color().vector_z, comp->animtrans_color().vector_w));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(comp->animtrans_color().vector_x, comp->animtrans_color().vector_y, comp->animtrans_color().vector_z, comp->animtrans_color().vector_w));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(comp->animtrans_color().vector_x, comp->animtrans_color().vector_y, comp->animtrans_color().vector_z, comp->animtrans_color().vector_w));

		bool flagtemp = ImGui::Button(
			unique_name,
			ImVec2(comp->animtrans_size().vector_x, comp->animtrans_size().vector_y)
		);
		comp->update_tick(ImGui::IsItemHovered(), ImGui::IsItemHovered() && ImGui::IsMouseDown(0), trans_smooth);

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return flagtemp;
	}

	bool CallAnimButtonImage(
		const char*                              unique_name, 
		unordered_map<string, ImAnimationBase*>& input, 
		const char*                              comp_name,
		uint32_t                                 texture_hd,
		float                                    trans_smooth
	) {
		const auto& comp = FindAnimComp(input, comp_name);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(comp->animtrans_color().vector_x, comp->animtrans_color().vector_y, comp->animtrans_color().vector_z, comp->animtrans_color().vector_w));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(comp->animtrans_color().vector_x, comp->animtrans_color().vector_y, comp->animtrans_color().vector_z, comp->animtrans_color().vector_w));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(comp->animtrans_color().vector_x, comp->animtrans_color().vector_y, comp->animtrans_color().vector_z, comp->animtrans_color().vector_w));

		bool flagtemp = ImGui::ImageButton(
			CVT_IMPTR(texture_hd),
			ImVec2(comp->animtrans_size().vector_x, comp->animtrans_size().vector_y)
		);
		comp->update_tick(ImGui::IsItemHovered(), ImGui::IsItemHovered() && ImGui::IsMouseDown(0), trans_smooth);

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return flagtemp;
	}

	ImAnimFixedWindow* CreateAnimFixedWindow(ImAnimFixedWindow* copy) {
		ImAnimFixedWindow* CompCreate = new ImAnimFixedWindow();

		if (copy) {
			CompCreate->config_open_color = copy->config_open_color;
			CompCreate->config_close_color = copy->config_close_color;

			CompCreate->config_open_size = copy->config_open_size;
			CompCreate->config_close_size = copy->config_close_size;
		}
		else {
			ImVec4 ColTmp = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
			CompCreate->config_open_color = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);
			CompCreate->config_close_color = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);

			CompCreate->config_open_size = Vector2T<float>(350.0f, 200.0f);
			CompCreate->config_close_size = Vector2T<float>(350.0f, 200.0f);
		}
		return CompCreate;
	}

	void CallAnimFixedWindow(
		const char*                                        unique_name,
		std::unordered_map<std::string, ImAnimationBase*>& input,
		const char*                                        comp_name,
		bool&                                              open_flag,
		function<void()>                                   window_ui,
		ImGuiWindowFlagsAm                                 flags,
		float                                              trans_smooth
	) {
		const auto& comp = FindAnimComp(input, comp_name);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(comp->animtrans_color().vector_x, comp->animtrans_color().vector_y, comp->animtrans_color().vector_z, comp->animtrans_color().vector_w));
		ImGui::SetNextWindowSize(ImVec2(comp->animtrans_size().vector_x, comp->animtrans_size().vector_y));

		comp->update_tick(false, open_flag, trans_smooth);
		if (open_flag) {
			ImGui::Begin(unique_name, &open_flag, flags | ImGuiWindowFlags_NoResize);
			window_ui();
			ImGui::End();
		}
		ImGui::PopStyleColor();
	}
}