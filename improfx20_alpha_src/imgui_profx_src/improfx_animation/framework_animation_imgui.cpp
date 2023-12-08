// framework_animation_imgui.
#include "framework_animation.h"

#include "imgui_glfw/imgui.h"
#include "imgui_glfw/imgui_impl_glfw.h"
#include "imgui_glfw/imgui_impl_opengl3.h"

using namespace std;
using namespace LOGCONS;
using namespace IFC_ANIM;

template <typename t_class>
t_class* FindAnimComp(unordered_map<string, t_class*>& input, const char* text) {
	return ((input.find(text) != input.end()) ? input[text] : nullptr);
}

template <typename t_class>
bool TemplateCompReg(unordered_map<string, t_class*>& input, const char* key, t_class* comp, const char* name) {
	if (comp) {
		t_class* repeat_comp = FindAnimComp<t_class>(input, key);
		if (!repeat_comp) {
			comp->ComponentsInit();
			input[key] = comp;

			const type_info& objinfo = typeid(*comp);
			PushLogger(LOG_TRC, SYSMD_LB_ANIM, "register comp[%s]: %s", name, key);
			PushLogger(LOG_INFO, SYSMD_LB_ANIM, "register object: %s", objinfo.name());
			return true;
		}
		else {
			PushLogger(LOG_ERR, SYSMD_LB_ANIM, "failed register comp[%s] duplicate_key.", name);
			return false;
		}
	}
	else {
		PushLogger(LOG_ERR, SYSMD_LB_ANIM, "failed register comp[%s] nullptr.", name);
		return false;
	}
}

template <typename t_class>
bool TemplateCompDelete(unordered_map<string, t_class*>& input, const char* key, const char* name) {
	t_class* comp = FindAnimComp<t_class>(input, key);
	if (comp) {
		delete comp;
		PushLogger(LOG_TRC, SYSMD_LB_ANIM, "delete comp[%s]: %s", name, key);
		return true;
	}
	else {
		PushLogger(LOG_WARN, SYSMD_LB_ANIM, "failed delete comp[%s], not found key.", name);
		return false;
	}
}

namespace ImGuiProAnim {

	bool AnimCompReg(unordered_map<string, ImAnimBaseStateful*>& input, const char* key, ImAnimBaseStateful* comp) {
		return TemplateCompReg<ImAnimBaseStateful>(input, key, comp, "anim");
	}

	bool AnimCompDelete(unordered_map<string, ImAnimBaseStateful*>& input, const char* key) {
		return TemplateCompDelete<ImAnimBaseStateful>(input, key, "anim");
	}

	ImAnimButton* CreateAnimButton(ImAnimButton* copy) {
		ImAnimButton* comp_create = new ImAnimButton();

		if (copy) {
			comp_create->ConfigNormalColor = copy->ConfigNormalColor;
			comp_create->ConfigHoverColor = copy->ConfigHoverColor;
			comp_create->ConfigActiveColor = copy->ConfigActiveColor;

			comp_create->ConfigNormalSize = copy->ConfigNormalSize;
			comp_create->ConfigHoverSize = copy->ConfigHoverSize;
			comp_create->ConfigActiveSize = copy->ConfigActiveSize;

			comp_create->ConfigColorTransSpeed = copy->ConfigColorTransSpeed;
			comp_create->ConfigSizeTransSpeed = copy->ConfigSizeTransSpeed;
		}
		else {
			ImVec4 ColTmp = ImGui::GetStyleColorVec4(ImGuiCol_Button);
			comp_create->ConfigNormalColor = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);

			ColTmp = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
			comp_create->ConfigHoverColor = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);

			ColTmp = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
			comp_create->ConfigActiveColor = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);

			// imgui system preset button_size.
			comp_create->ConfigNormalSize = Vector2T<float>(128.0f, 42.0f);
			comp_create->ConfigHoverSize = Vector2T<float>(128.0f, 42.0f);
			comp_create->ConfigActiveSize = Vector2T<float>(128.0f, 42.0f);
		}
		return comp_create;
	}

	bool CallAnimButton(
		const char*                                 unique_name, 
		unordered_map<string, ImAnimBaseStateful*>& input, 
		const char*									comp_name, 
		const float&                                trans_smooth
	) {
		const auto& comp = FindAnimComp<ImAnimBaseStateful>(input, comp_name);
		bool flagtemp = false;

		if (comp) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w));

			const auto& comp_class = dynamic_cast<ImAnimButton*>(comp);
			// 重叠绘制, InvisibleButton & Button.
			ImVec2 temp_pos = ImGui::GetCursorPos();
			{
				flagtemp = ImGui::InvisibleButton(unique_name, ImVec2(comp_class->ConfigNormalSize.vector_x, comp_class->ConfigNormalSize.vector_y));
				comp->UpdateTick(ImGui::IsItemHovered(), ImGui::IsItemHovered() && ImGui::IsMouseDown(0), trans_smooth);
			}
			ImGui::SetCursorPos(temp_pos);
			// 固定中心位置.
			ImGui::SetCursorPos(ImVec2(
				ImGui::GetCursorPos().x + (comp_class->ConfigNormalSize.vector_x - comp->AnimtransSize()->vector_x) / 2.0f,
				ImGui::GetCursorPos().y + (comp_class->ConfigNormalSize.vector_y - comp->AnimtransSize()->vector_y) / 2.0f
			));
			ImGui::Button(unique_name, ImVec2(comp->AnimtransSize()->vector_x, comp->AnimtransSize()->vector_y));

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}
		return flagtemp;
	}

	bool CallAnimButtonImage(
		const char*                                 unique_name,
		unordered_map<string, ImAnimBaseStateful*>& input,
		const char*                                 comp_name,
		uint32_t                                    texture_hd,
		const float&                                trans_smooth
	) {
		const auto& comp = FindAnimComp<ImAnimBaseStateful>(input, comp_name);
		bool flagtemp = false;

		if (comp) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(comp->AnimtransColor()->vector_x, comp->AnimtransColor()->vector_y, comp->AnimtransColor()->vector_z, comp->AnimtransColor()->vector_w));

			const auto& comp_class = dynamic_cast<ImAnimButton*>(comp);
			// 重叠绘制, InvisibleButton & Button.
			ImVec2 temp_pos = ImGui::GetCursorPos();
			{
				flagtemp = ImGui::InvisibleButton(unique_name, ImVec2(comp_class->ConfigNormalSize.vector_x, comp_class->ConfigNormalSize.vector_y));
				comp->UpdateTick(ImGui::IsItemHovered(), ImGui::IsItemHovered() && ImGui::IsMouseDown(0), trans_smooth);
			}
			ImGui::SetCursorPos(temp_pos);
			// 固定中心位置.
			ImGui::SetCursorPos(ImVec2(
				ImGui::GetCursorPos().x + (comp_class->ConfigNormalSize.vector_x - comp->AnimtransSize()->vector_x) / 2.0f,
				ImGui::GetCursorPos().y + (comp_class->ConfigNormalSize.vector_y - comp->AnimtransSize()->vector_y) / 2.0f
			));
			// ImageButton 比普通按钮尺寸大(3.0f*2).
			ImGui::ImageButton(
				CVT_IMPTR(texture_hd),
				ImVec2(comp->AnimtransSize()->vector_x - 6.0f, comp->AnimtransSize()->vector_y - 6.0f)
			);

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}
		return flagtemp;
	}

	ImAnimFixedWindow* CreateAnimFixedWindow(ImAnimFixedWindow* copy) {
		ImAnimFixedWindow* comp_create = new ImAnimFixedWindow();

		if (copy) {
			comp_create->ConfigOpenColor = copy->ConfigOpenColor;
			comp_create->ConfigCloseColor = copy->ConfigCloseColor;

			comp_create->ConfigOpenSize = copy->ConfigOpenSize;
			comp_create->ConfigCloseSize = copy->ConfigCloseSize;

			comp_create->ConfigColorTransSpeed = copy->ConfigColorTransSpeed;
			comp_create->ConfigSizeTransSpeed = copy->ConfigSizeTransSpeed;
		}
		else {
			ImVec4 ColTmp = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
			comp_create->ConfigOpenColor = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);
			comp_create->ConfigCloseColor = Vector4T<float>(ColTmp.x, ColTmp.y, ColTmp.z, ColTmp.w);

			comp_create->ConfigOpenSize = Vector2T<float>(350.0f, 200.0f);
			comp_create->ConfigCloseSize = Vector2T<float>(350.0f, 200.0f);
		}
		return comp_create;
	}

	void CallAnimFixedWindow(
		const char*                                 unique_name,
		unordered_map<string, ImAnimBaseStateful*>& input,
		const char*                                 comp_name,
		bool&                                       open_flag,
		function<void()>							window_ui,
		ImGuiWindowFlagsAm							flags,
		const float&                                trans_smooth
	) {
		const auto& comp = FindAnimComp<ImAnimBaseStateful>(input, comp_name);

		if (comp) {
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
	}

	ImAnimSourceComp* CreateAnimSourceComp(ImAnimSourceComp* copy) {
		ImAnimSourceComp* comp_create = new ImAnimSourceComp();

		if (copy) {
			comp_create->ConfigLowColor = copy->ConfigLowColor;
			comp_create->ConfigHighColor = copy->ConfigHighColor;

			comp_create->ConfigLowPosition = copy->ConfigLowPosition;
			comp_create->ConfigHighPosition = copy->ConfigHighPosition;

			comp_create->ConfigColorTransSpeed = copy->ConfigColorTransSpeed;
			comp_create->ConfigPositionTransSpeed = copy->ConfigPositionTransSpeed;
		}
		else {
			comp_create->ConfigLowColor = Vector4T<float>();
			comp_create->ConfigHighColor = Vector4T<float>();

			comp_create->ConfigLowPosition = Vector2T<float>();
			comp_create->ConfigHighPosition = Vector2T<float>();
		}
		return comp_create;
	}

	void CallAnimSourceComp(
		unordered_map<string, ImAnimBaseStateful*>&      input,
		const char*                                      comp_name,
		const bool&                                      state_flag,
		function<SYSDEF_FUNC_PARAM>                      context,
		const float&                                     trans_smooth
	) {
		const auto& comp = FindAnimComp<ImAnimBaseStateful>(input, comp_name);

		if (comp) {
			comp->UpdateTick(false, state_flag, trans_smooth);
			ImVec2 temp_pos = ImGui::GetCursorPos();

			ImGui::SetCursorPos(ImVec2(comp->AnimtransPosition()->vector_x, comp->AnimtransPosition()->vector_y));
			context(*comp->AnimtransSize(), *comp->AnimtransColor());

			// restore cursor_position.
			ImGui::SetCursorPos(temp_pos);
		}
	}

	void CallAnimSourceCompRES(
		unordered_map<string, ImAnimBaseStateful*>& input,
		const char*                                 comp_name,
		Vector4T<float>								setcolor,
		Vector2T<float>								setsize,
		Vector2T<float>								setpoition
	) {
		const auto& comp = FindAnimComp(input, comp_name);

		*comp->AnimtransColor() = setcolor;
		*comp->AnimtransSize() = setsize;
		*comp->AnimtransPosition() = setpoition;
	}
}

namespace ImGuiProAnimNodes {

	bool AnimNodesCompReg(unordered_map<string, ImAnimBaseDynamic*>& input, const char* key, ImAnimBaseDynamic* comp) {
		return TemplateCompReg<ImAnimBaseDynamic>(input, key, comp, "anim_nodes");
	}
	
	bool AnimNodesCompDelete(unordered_map<string, ImAnimBaseDynamic*>& input, const char* key) {
		return TemplateCompDelete<ImAnimBaseDynamic>(input, key, "anim_nodes");
	}

	void ImAnimNodesHashmap::ConfigNodeAdd(const char* key, ImAnimDynamicNode value) {
		auto it = animation_nodes.find(key);
		if (it != animation_nodes.end())
			PushLogger(LOG_WARN, SYSMD_LB_ANIM, "failed add node_hashmap duplicate_key: %s", key);
		else {
			animation_nodes[key] = value;
			PushLogger(LOG_INFO, SYSMD_LB_ANIM, "add node_hashmap key: %s", key);
		}
	}

	void ImAnimNodesHashmap::ConfigNodeDelete(const char* key) {
		auto it = animation_nodes.find(key);
		if (it != animation_nodes.end()) {
			animation_nodes.erase(it);
			PushLogger(LOG_INFO, SYSMD_LB_ANIM, "delete node_hashmap key: %s", key);
		}
		else
			PushLogger(LOG_WARN, SYSMD_LB_ANIM, "failed delete node_hashmap, not found key.");
	}

	IFC_ANIM::ImAnimDynamicNode* ImAnimNodesHashmap::ConfigNodeFind(const char* key) {
		auto it = animation_nodes.find(key);
		if (it != animation_nodes.end())
			return &it->second;
		else {
			PushLogger(LOG_WARN, SYSMD_LB_ANIM, "failed find node_hashmap, not found key.");
			return nullptr;
		}
	}

	void ImAnimNodesVector::ConfigNodeAdd(const char* key, ImAnimDynamicNode value) {
		animation_nodes.push_back(make_pair(key, value));
		PushLogger(LOG_INFO, SYSMD_LB_ANIM, "add node_vector key: %s", key);
	}

	void ImAnimNodesVector::ConfigNodeDelete(const char* key) {
		size_t erase_count = NULL;
		for (auto it = animation_nodes.begin(); it != animation_nodes.end(); ++it) {
			if (it->first == key) {
				animation_nodes.erase(it);
				++erase_count;
				PushLogger(LOG_INFO, SYSMD_LB_ANIM, "delete node_vector key: %s count: %u", key, erase_count);
			}
		}
	}

	IFC_ANIM::ImAnimDynamicNode* ImAnimNodesVector::ConfigNodeFind(const char* key) {
		size_t find_count = stoul(key);
		if (animation_nodes.size() > find_count)
			return &animation_nodes[find_count].second;
		else {
			PushLogger(LOG_WARN, SYSMD_LB_ANIM, "failed find node_vector, not found count.");
			return nullptr;
		}
	}

	ImAnimBaseDynamic* CreateAnimNodes(AnimCompType type, ImAnimBaseDynamic* copy, const Vector3T<float>& speed) {
		ImAnimBaseDynamic* return_comp = nullptr;

		switch (type) {
		case(NodesHashmap): {
			ImAnimNodesHashmap* comp_create = new ImAnimNodesHashmap();

			comp_create->ConfigPositionTransSpeed = speed.vector_x;
			comp_create->ConfigSizeTransSpeed = speed.vector_y;
			comp_create->ConfigColorTransSpeed = speed.vector_z;
			if (copy) {
				ImAnimNodesHashmap* temp_object = dynamic_cast<ImAnimNodesHashmap*>(copy);
				if (temp_object)
					*comp_create->getnodes_refe() = *temp_object->getnodes_refe();
				else
					PushLogger(LOG_WARN, SYSMD_LB_ANIM, "create node_hashmap, configuration object not'ImAnimNodesHashmap'.");
			}
			return_comp = comp_create;
			break;
		}
		case(NodesVector): {
			ImAnimNodesVector* comp_create = new ImAnimNodesVector();

			comp_create->ConfigPositionTransSpeed = speed.vector_x;
			comp_create->ConfigSizeTransSpeed = speed.vector_y;
			comp_create->ConfigColorTransSpeed = speed.vector_z;
			if (copy) {
				ImAnimNodesVector* temp_object = dynamic_cast<ImAnimNodesVector*>(copy);
				if (temp_object)
					*comp_create->getnodes_refe() = *temp_object->getnodes_refe();
				else
					PushLogger(LOG_WARN, SYSMD_LB_ANIM, "create node_hashmap, configuration object not'ImAnimNodesVector'.");
			}
			return_comp = comp_create;
			break;
		}
		}
		return return_comp;
	}

	ImAnimDynamicNode ConfigAnimNode(
		const bool&            state,
		const Vector2T<float>& pos,
		const Vector2T<float>& size,
		const Vector4T<float>& color
	) {
		return ImAnimDynamicNode(state, pos, size, color);
	}

	bool CallAnimNodesHashmap(
		const char*                                      nodes_key,
		unordered_map<string, ImAnimBaseDynamic*>&       input,
		const char*                                      comp_name,
		function<SYSDEF_FUNC_NODES_PARAM>                context,
		const float&                                     trans_smooth
	) {
		const auto& comp = FindAnimComp<ImAnimBaseDynamic>(input, comp_name);
		bool returnflag = false;

		if (comp) {
			returnflag = comp->UpdateTickName(nodes_key, trans_smooth);
			ImVec2 temp_pos = ImGui::GetCursorPos();

			ImGui::SetCursorPos(ImVec2(comp->AnimtransPosition()->vector_x, comp->AnimtransPosition()->vector_y));
			context(*comp->AnimtransSize(), *comp->AnimtransColor(), *comp->AnimFlagState());

			// restore cursor_position.
			ImGui::SetCursorPos(temp_pos);
		}
		return returnflag;
	}

	bool CallAnimNodesVector(
		const size_t&                                    nodes_count,
		unordered_map<string, ImAnimBaseDynamic*>&       input,
		const char*                                      comp_name,
		function<SYSDEF_FUNC_NODES_PARAM>                context,
		const float&                                     trans_smooth
	) {
		const auto& comp = FindAnimComp<ImAnimBaseDynamic>(input, comp_name);
		bool returnflag = false;

		if (comp) {
			returnflag = comp->UpdateTickCount(nodes_count, trans_smooth);
			ImVec2 temp_pos = ImGui::GetCursorPos();
			
			ImGui::SetCursorPos(ImVec2(comp->AnimtransPosition()->vector_x, comp->AnimtransPosition()->vector_y));
			context(*comp->AnimtransSize(), *comp->AnimtransColor(), *comp->AnimFlagState());

			ImGui::SetCursorPos(temp_pos);
		}
		return returnflag;
	}
}