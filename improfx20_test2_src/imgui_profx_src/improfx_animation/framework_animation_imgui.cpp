// framework_animation_imgui.
#include "framework_animation.h"

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

IFC_ANIM::ImAnimBaseStateful* FindAnimComp(unordered_map<string, IFC_ANIM::ImAnimBaseStateful*>& input, const char* text) {
	return ((input.find(text) != input.end()) ? input[text] : nullptr);
}

namespace ImGuiProAnim {

	bool AnimCompReg(unordered_map<string, IFC_ANIM::ImAnimBaseStateful*>& input, const char* key, IFC_ANIM::ImAnimBaseStateful* comp) {
		if (comp) {
			IFC_ANIM::ImAnimBaseStateful* repeat_comp = FindAnimComp(input, key);
			// 当前资源中无重复Key, repeat_comp == nullptr.
			if (!repeat_comp) {
				comp->ComponentsInit();
				input[key] = comp;

				const type_info& objinfo = typeid(*comp);
				PushLogger(LOG_TRC, SYSMD_LB_ANIM, "register comp: %s", key);
				PushLogger(LOG_INFO, SYSMD_LB_ANIM, "register object_name: %s", objinfo.name());
				return true;
			}
			else {
				PushLogger(LOG_ERR, SYSMD_LB_ANIM, "failed register comp duplicate_key.");
				return false;
			}
		}
		else {
			PushLogger(LOG_ERR, SYSMD_LB_ANIM, "failed register comp nullptr.");
			return false;
		}
	}

	bool AnimCompDelete(std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input, const char* key) {
		IFC_ANIM::ImAnimBaseStateful* comp = FindAnimComp(input, key);
		if (comp) {
			delete comp;
			PushLogger(LOG_TRC, SYSMD_LB_ANIM, "delete comp: %s", key);
			return true;
		}
		else {
			PushLogger(LOG_WARN, SYSMD_LB_ANIM, "failed delete comp, not found key.");
			return false;
		}
	}

	ImAnimButton* CreateAnimButton(ImAnimButton* copy) {
		ImAnimButton* CompCreate = new ImAnimButton();

		if (copy) {
			CompCreate->ConfigNormalColor = copy->ConfigNormalColor;
			CompCreate->ConfigHoverColor = copy->ConfigHoverColor;
			CompCreate->ConfigActiveColor = copy->ConfigActiveColor;

			CompCreate->ConfigNormalSize = copy->ConfigNormalSize;
			CompCreate->ConfigHoverSize = copy->ConfigHoverSize;
			CompCreate->ConfigActiveSize = copy->ConfigActiveSize;

			CompCreate->ConfigColorTransSpeed = copy->ConfigColorTransSpeed;
			CompCreate->ConfigSizeTransSpeed = copy->ConfigSizeTransSpeed;
		}
		else {
			ImVec4 ColTmp = ImGui::GetStyleColorVec4(ImGuiCol_Button);
			CompCreate->ConfigNormalColor = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);

			ColTmp = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
			CompCreate->ConfigHoverColor = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);

			ColTmp = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
			CompCreate->ConfigActiveColor = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);

			// imgui system preset button_size.
			CompCreate->ConfigNormalSize = Vector2T<float>(128.0f, 42.0f);
			CompCreate->ConfigHoverSize = Vector2T<float>(128.0f, 42.0f);
			CompCreate->ConfigActiveSize = Vector2T<float>(128.0f, 42.0f);
		}
		return CompCreate;
	}

	bool CallAnimButton(
		const char*                                           unique_name, 
		unordered_map<string, IFC_ANIM::ImAnimBaseStateful*>& input, 
		const char*											  comp_name, 
		float											      trans_smooth
	) {
		const auto& comp = FindAnimComp(input, comp_name);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w));

		bool flagtemp = ImGui::Button(
			unique_name,
			ImVec2(comp->AnimtransSize()->vector_x, comp->AnimtransSize()->vector_y)
		);
		comp->UpdateTick(ImGui::IsItemHovered(), ImGui::IsItemHovered() && ImGui::IsMouseDown(0), trans_smooth);

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return flagtemp;
	}

	bool CallAnimButtonImage(
		const char*                                           unique_name, 
		unordered_map<string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*                                           comp_name,
		uint32_t                                              texture_hd,
		float                                                 trans_smooth
	) {
		const auto& comp = FindAnimComp(input, comp_name);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w));

		bool flagtemp = ImGui::ImageButton(
			CVT_IMPTR(texture_hd),
			ImVec2(comp->AnimtransSize()->vector_x, comp->AnimtransSize()->vector_y)
		);
		comp->UpdateTick(ImGui::IsItemHovered(), ImGui::IsItemHovered() && ImGui::IsMouseDown(0), trans_smooth);

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return flagtemp;
	}

	ImAnimFixedWindow* CreateAnimFixedWindow(ImAnimFixedWindow* copy) {
		ImAnimFixedWindow* CompCreate = new ImAnimFixedWindow();

		if (copy) {
			CompCreate->ConfigOpenColor = copy->ConfigOpenColor;
			CompCreate->ConfigCloseColor = copy->ConfigCloseColor;

			CompCreate->ConfigOpenSize = copy->ConfigOpenSize;
			CompCreate->ConfigCloseSize = copy->ConfigCloseSize;

			CompCreate->ConfigColorTransSpeed = copy->ConfigColorTransSpeed;
			CompCreate->ConfigSizeTransSpeed = copy->ConfigSizeTransSpeed;
		}
		else {
			ImVec4 ColTmp = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
			CompCreate->ConfigOpenColor = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);
			CompCreate->ConfigCloseColor = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);

			CompCreate->ConfigOpenSize = Vector2T<float>(350.0f, 200.0f);
			CompCreate->ConfigCloseSize = Vector2T<float>(350.0f, 200.0f);
		}
		return CompCreate;
	}

	void CallAnimFixedWindow(
		const char*                                                     unique_name,
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*                                                     comp_name,
		bool&                                                           open_flag,
		function<void()>												window_ui,
		ImGuiWindowFlagsAm												flags,
		float															trans_smooth
	) {
		const auto& comp = FindAnimComp(input, comp_name);

		ImGui::PushStyleColor(
			ImGuiCol_WindowBg, 
			ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w)
		);
		ImGui::SetNextWindowSize(ImVec2(comp->AnimtransSize()->vector_x, comp->AnimtransSize()->vector_y));

		comp->UpdateTick(false, open_flag, trans_smooth);
		if (open_flag) {
			ImGui::Begin(unique_name, &open_flag, flags | ImGuiWindowFlags_NoResize);
			window_ui();
			ImGui::End();
		}
		ImGui::PopStyleColor();
	}

	ImAnimSourceComp* CreateAnimSourceComp(ImAnimSourceComp* copy) {
		ImAnimSourceComp* CompCreate = new ImAnimSourceComp();

		if (copy) {
			CompCreate->ConfigLowColor = copy->ConfigLowColor;
			CompCreate->ConfigHighColor = copy->ConfigHighColor;

			CompCreate->ConfigLowPosition = copy->ConfigLowPosition;
			CompCreate->ConfigHighPosition = copy->ConfigHighPosition;

			CompCreate->ConfigColorTransSpeed = copy->ConfigColorTransSpeed;
			CompCreate->ConfigPositionTransSpeed = copy->ConfigPositionTransSpeed;
		}
		else {
			CompCreate->ConfigLowColor = Vector4T<float>();
			CompCreate->ConfigHighColor = Vector4T<float>();

			CompCreate->ConfigLowPosition = Vector2T<float>();
			CompCreate->ConfigHighPosition = Vector2T<float>();
		}
		return CompCreate;
	}

	void CallAnimSourceComp(
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*                                                     comp_name,
		bool                                                            state_flag,
		std::function<void(Vector4T<float>, Vector2T<float>)>           context,
		float                                                           trans_smooth
	) {
		const auto& comp = FindAnimComp(input, comp_name);

		comp->UpdateTick(false, state_flag, trans_smooth);
		ImVec2 temp_pos = ImGui::GetCursorPos();

		ImGui::SetCursorPos(ImVec2(comp->AnimtransPosition()->vector_x, comp->AnimtransPosition()->vector_y));
		context(*comp->AnimtransColor(), *comp->AnimtransSize());

		// restore cursor_position.
		ImGui::SetCursorPos(temp_pos);
	}

	void CallAnimSourceCompRES(
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*                                                     comp_name,
		Vector4T<float>													setcolor,
		Vector2T<float>													setsize,
		Vector2T<float>													setpoition
	) {
		const auto& comp = FindAnimComp(input, comp_name);

		*comp->AnimtransColor() = setcolor;
		*comp->AnimtransSize() = setsize;
		*comp->AnimtransPosition() = setpoition;
	}
}