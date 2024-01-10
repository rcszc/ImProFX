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
		t_class* RepeatComp = FindAnimComp<t_class>(input, key);
		if (!RepeatComp) {
			comp->ComponentsInit();
			input[key] = comp;

			const type_info& OBJECT_INFO = typeid(*comp);
			PushLogger(LOG_TRACE, SYSMD_LB_ANIM, "register comp[%s]: %s", name, key);
			PushLogger(LOG_INFO, SYSMD_LB_ANIM, "register object: %s", OBJECT_INFO.name());
			return true;
		}
		else {
			PushLogger(LOG_ERROR, SYSMD_LB_ANIM, "failed register comp[%s] duplicate_key.", name);
			return false;
		}
	}
	else {
		PushLogger(LOG_ERROR, SYSMD_LB_ANIM, "failed register comp[%s] nullptr.", name);
		return false;
	}
}

template <typename t_class>
bool TemplateCompDelete(unordered_map<string, t_class*>& input, const char* key, const char* name) {
	t_class* DeleteComp = FindAnimComp<t_class>(input, key);
	if (DeleteComp) {
		delete DeleteComp;
		PushLogger(LOG_TRACE, SYSMD_LB_ANIM, "delete comp[%s]: %s", name, key);
		return true;
	}
	else {
		PushLogger(LOG_WARNING, SYSMD_LB_ANIM, "failed delete comp[%s], not found key.", name);
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
		const char*                                 button_name, 
		unordered_map<string, ImAnimBaseStateful*>& input, 
		const char*									comp_name, 
		const float&                                trans_smooth
	) {
		const auto& BaseComp = FindAnimComp<ImAnimBaseStateful>(input, comp_name);
		bool FlagTemp = false;

		if (BaseComp) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(BaseComp->AnimtransColor()->vector_x, BaseComp->AnimtransColor()->vector_y, BaseComp->AnimtransColor()->vector_z, BaseComp->AnimtransColor()->vector_w));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(BaseComp->AnimtransColor()->vector_x, BaseComp->AnimtransColor()->vector_y, BaseComp->AnimtransColor()->vector_z, BaseComp->AnimtransColor()->vector_w));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(BaseComp->AnimtransColor()->vector_x, BaseComp->AnimtransColor()->vector_y, BaseComp->AnimtransColor()->vector_z, BaseComp->AnimtransColor()->vector_w));

			ImGui::PushID(*BaseComp->_MS_UNIQUE());
			{
				const auto& ClassComp = dynamic_cast<ImAnimButton*>(BaseComp);
				// 重叠绘制, InvisibleButton & Button.
				ImVec2 ReposTemp = ImGui::GetCursorPos();
				{
					FlagTemp = ImGui::InvisibleButton(button_name, ImVec2(ClassComp->ConfigNormalSize.vector_x, ClassComp->ConfigNormalSize.vector_y));
					BaseComp->UpdateTick(ImGui::IsItemHovered(), ImGui::IsItemHovered() && ImGui::IsMouseDown(0), trans_smooth);
				}
				ImGui::SetCursorPos(ReposTemp);
				// 固定中心位置.
				ImGui::SetCursorPos(ImVec2(
					ImGui::GetCursorPos().x + (ClassComp->ConfigNormalSize.vector_x - BaseComp->AnimtransSize()->vector_x) / 2.0f,
					ImGui::GetCursorPos().y + (ClassComp->ConfigNormalSize.vector_y - BaseComp->AnimtransSize()->vector_y) / 2.0f
				));
				ImGui::Button(button_name, ImVec2(BaseComp->AnimtransSize()->vector_x, BaseComp->AnimtransSize()->vector_y));
			}
			ImGui::PopID();

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}
		return FlagTemp;
	}

	bool CallAnimButtonImage(
		const char*                                 button_name,
		unordered_map<string, ImAnimBaseStateful*>& input,
		const char*                                 comp_name,
		uint32_t                                    TextureHandle,
		const float&                                trans_smooth
	) {
		const auto& BaseComp = FindAnimComp<ImAnimBaseStateful>(input, comp_name);
		bool FlagTemp = false;

		if (BaseComp) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(BaseComp->AnimtransColor()->vector_x, BaseComp->AnimtransColor()->vector_y, BaseComp->AnimtransColor()->vector_z, BaseComp->AnimtransColor()->vector_w));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(BaseComp->AnimtransColor()->vector_x, BaseComp->AnimtransColor()->vector_y, BaseComp->AnimtransColor()->vector_z, BaseComp->AnimtransColor()->vector_w));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(BaseComp->AnimtransColor()->vector_x, BaseComp->AnimtransColor()->vector_y, BaseComp->AnimtransColor()->vector_z, BaseComp->AnimtransColor()->vector_w));

			ImGui::PushID(*BaseComp->_MS_UNIQUE());
			{
				const auto& ClassComp = dynamic_cast<ImAnimButton*>(BaseComp);
				// 重叠绘制, InvisibleButton & Button.
				ImVec2 ReposTemp = ImGui::GetCursorPos();
				{
					FlagTemp = ImGui::InvisibleButton(button_name, ImVec2(ClassComp->ConfigNormalSize.vector_x, ClassComp->ConfigNormalSize.vector_y));
					BaseComp->UpdateTick(ImGui::IsItemHovered(), ImGui::IsItemHovered() && ImGui::IsMouseDown(0), trans_smooth);
				}
				ImGui::SetCursorPos(ReposTemp);
				// 固定中心位置.
				ImGui::SetCursorPos(ImVec2(
					ImGui::GetCursorPos().x + (ClassComp->ConfigNormalSize.vector_x - BaseComp->AnimtransSize()->vector_x) / 2.0f,
					ImGui::GetCursorPos().y + (ClassComp->ConfigNormalSize.vector_y - BaseComp->AnimtransSize()->vector_y) / 2.0f
				));
				// ImageButton 比普通按钮尺寸大(3.0f*2).
				ImGui::ImageButton(
					CVT_IMPTR(TextureHandle),
					ImVec2(BaseComp->AnimtransSize()->vector_x - 6.0f, BaseComp->AnimtransSize()->vector_y - 6.0f)
				);
			}
			ImGui::PopID();

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}
		return FlagTemp;
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
		const char*                                 window_name,
		unordered_map<string, ImAnimBaseStateful*>& input,
		const char*                                 comp_name,
		bool&                                       open_flag,
		function<void()>							window_ui,
		ImGuiWindowFlagsAm							flags,
		const float&                                trans_smooth
	) {
		const auto& BaseComp = FindAnimComp<ImAnimBaseStateful>(input, comp_name);

		ImGui::PushID(*BaseComp->_MS_UNIQUE());
		if (BaseComp) {
			ImGui::PushStyleColor(
				ImGuiCol_WindowBg,
				ImVec4(BaseComp->AnimtransColor()->vector_x, BaseComp->AnimtransColor()->vector_y, BaseComp->AnimtransColor()->vector_z, BaseComp->AnimtransColor()->vector_w)
			);
			ImGui::SetNextWindowSize(ImVec2(BaseComp->AnimtransSize()->vector_x, BaseComp->AnimtransSize()->vector_y));

			BaseComp->UpdateTick(false, open_flag, trans_smooth);
			if (open_flag) {
				ImGui::Begin(window_name, &open_flag, flags | ImGuiWindowFlags_NoResize);
				window_ui();
				ImGui::End();
			}
			ImGui::PopStyleColor();
		}
		ImGui::PopID();
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
		unordered_map<string, ImAnimBaseStateful*>&      input,
		const char*                                      comp_name,
		const bool&                                      state_flag,
		function<SYSDEF_FUNC_PARAM>                      context,
		const float&                                     trans_smooth
	) {
		const auto& BaseComp = FindAnimComp<ImAnimBaseStateful>(input, comp_name);

		ImGui::PushID(*BaseComp->_MS_UNIQUE());
		if (BaseComp) {
			BaseComp->UpdateTick(false, state_flag, trans_smooth);
			ImVec2 ReposTemp = ImGui::GetCursorPos();

			ImGui::SetCursorPos(ImVec2(BaseComp->AnimtransPosition()->vector_x, BaseComp->AnimtransPosition()->vector_y));
			context(*BaseComp->AnimtransSize(), *BaseComp->AnimtransColor());

			// restore cursor_position.
			ImGui::SetCursorPos(ReposTemp);
		}
		ImGui::PopID();
	}

	void CallAnimSourceCompRES(
		unordered_map<string, ImAnimBaseStateful*>& input,
		const char*                                 comp_name,
		Vector4T<float>								setcolor,
		Vector2T<float>								setsize,
		Vector2T<float>								setpoition
	) {
		const auto& BaseComp = FindAnimComp(input, comp_name);

		*BaseComp->AnimtransColor() = setcolor;
		*BaseComp->AnimtransSize() = setsize;
		*BaseComp->AnimtransPosition() = setpoition;
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
		auto it = AnimationNodes.find(key);
		if (it != AnimationNodes.end())
			PushLogger(LOG_WARNING, SYSMD_LB_ANIM, "failed add node_hashmap duplicate_key: %s", key);
		else {
			AnimationNodes[key] = value;
			PushLogger(LOG_INFO, SYSMD_LB_ANIM, "add node_hashmap key: %s", key);
		}
	}

	void ImAnimNodesHashmap::ConfigNodeDelete(const char* key) {
		auto it = AnimationNodes.find(key);
		if (it != AnimationNodes.end()) {
			AnimationNodes.erase(it);
			PushLogger(LOG_INFO, SYSMD_LB_ANIM, "delete node_hashmap key: %s", key);
		}
		else
			PushLogger(LOG_WARNING, SYSMD_LB_ANIM, "failed delete node_hashmap, not found key.");
	}

	IFC_ANIM::ImAnimDynamicNode* ImAnimNodesHashmap::ConfigNodeFind(const char* key) {
		auto it = AnimationNodes.find(key);
		if (it != AnimationNodes.end())
			return &it->second;
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_ANIM, "failed find node_hashmap, not found key.");
			return nullptr;
		}
	}

	void ImAnimNodesVector::ConfigNodeAdd(const char* key, ImAnimDynamicNode value) {
		AnimationNodes.push_back(make_pair(key, value));
		PushLogger(LOG_INFO, SYSMD_LB_ANIM, "add node_vector key: %s", key);
	}

	void ImAnimNodesVector::ConfigNodeDelete(const char* key) {
		size_t EraseCount = NULL;
		for (auto it = AnimationNodes.begin(); it != AnimationNodes.end();) {
			if (it->first == key) {
				AnimationNodes.erase(it);
				++EraseCount;
				PushLogger(LOG_INFO, SYSMD_LB_ANIM, "delete node_vector key: %s count: %u", key, EraseCount);
			}
			else
				++it;
		}
	}

	IFC_ANIM::ImAnimDynamicNode* ImAnimNodesVector::ConfigNodeFind(const char* key) {
		size_t FindCount = stoul(key);
		if (AnimationNodes.size() > FindCount)
			return &AnimationNodes[FindCount].second;
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_ANIM, "failed find node_vector, not found count.");
			return nullptr;
		}
	}

	ImAnimBaseDynamic* CreateAnimNodes(AnimCompType type, ImAnimBaseDynamic* copy, const Vector3T<float>& speed) {
		ImAnimBaseDynamic* ReturnBaseComp = nullptr;

		switch (type) {
		case(NodesHashmap): {
			ImAnimNodesHashmap* CompCreate = new ImAnimNodesHashmap();

			CompCreate->ConfigPositionTransSpeed = speed.vector_x;
			CompCreate->ConfigSizeTransSpeed     = speed.vector_y;
			CompCreate->ConfigColorTransSpeed    = speed.vector_z;
			if (copy) {
				if (typeid(*copy) == typeid(ImAnimNodesHashmap)) {
					// convert object & copy config.
					ImAnimNodesHashmap* ObjectTemp = dynamic_cast<ImAnimNodesHashmap*>(copy);
					*CompCreate->getnodes_refe() = *ObjectTemp->getnodes_refe();
				}
				else
					PushLogger(LOG_WARNING, SYSMD_LB_ANIM, "configure failed, copy object not'ImAnimNodesHashmap'.");
			}
			ReturnBaseComp = CompCreate;
			break;
		}
		case(NodesVector): {
			ImAnimNodesVector* CompCreate = new ImAnimNodesVector();

			CompCreate->ConfigPositionTransSpeed = speed.vector_x;
			CompCreate->ConfigSizeTransSpeed     = speed.vector_y;
			CompCreate->ConfigColorTransSpeed    = speed.vector_z;
			if (copy) {
				if (typeid(*copy) == typeid(ImAnimNodesVector)) {
					// convert object & copy config.
					ImAnimNodesVector* ObjectTemp = dynamic_cast<ImAnimNodesVector*>(copy);
					*CompCreate->getnodes_refe() = *ObjectTemp->getnodes_refe();
				}
				else
					PushLogger(LOG_WARNING, SYSMD_LB_ANIM, "configure failed, copy object not'ImAnimNodesVector'.");
			}
			ReturnBaseComp = CompCreate;
			break;
		}
		}
		return ReturnBaseComp;
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
		const auto& BaseComp = FindAnimComp<ImAnimBaseDynamic>(input, comp_name);
		bool ReturnFlag = false;

		if (BaseComp) {
			ReturnFlag = BaseComp->UpdateTickName(nodes_key, trans_smooth);
			ImVec2 temp_pos = ImGui::GetCursorPos();

			ImGui::SetCursorPos(ImVec2(BaseComp->AnimtransPosition()->vector_x, BaseComp->AnimtransPosition()->vector_y));
			context(*BaseComp->AnimtransSize(), *BaseComp->AnimtransColor(), *BaseComp->AnimFlagState());

			// restore cursor_position.
			ImGui::SetCursorPos(temp_pos);
		}
		return ReturnFlag;
	}

	bool CallAnimNodesVector(
		const size_t&                                    nodes_count,
		unordered_map<string, ImAnimBaseDynamic*>&       input,
		const char*                                      comp_name,
		function<SYSDEF_FUNC_NODES_PARAM>                context,
		const float&                                     trans_smooth
	) {
		const auto& BaseComp = FindAnimComp<ImAnimBaseDynamic>(input, comp_name);
		bool ReturnFlag = false;

		if (BaseComp) {
			ReturnFlag = BaseComp->UpdateTickCount(nodes_count, trans_smooth);
			ImVec2 ReposTemp = ImGui::GetCursorPos();
			
			ImGui::SetCursorPos(ImVec2(BaseComp->AnimtransPosition()->vector_x, BaseComp->AnimtransPosition()->vector_y));
			context(*BaseComp->AnimtransSize(), *BaseComp->AnimtransColor(), *BaseComp->AnimFlagState());

			ImGui::SetCursorPos(ReposTemp);
		}
		return ReturnFlag;
	}
}