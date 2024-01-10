// framework_animation. [2023_11_01] RCSZ.

#ifndef _FRAMEWORK_ANIMATION_H
#define _FRAMEWORK_ANIMATION_H
#include <utility>
#include "framework_ainmation_interface.h"

#define SYSMD_LB_ANIM "[IM_ANIMATION]: "

namespace ImGuiProAnim {
	/*
	* ImGuiProAnim 动画控件注册
	* @param  input (FrmDat.DataAnimation), key (组件对象Hash.key), comp (配置完成的组件对象)
	* @return state
	*/
	bool AnimCompReg(
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input, 
		const char*														key, 
		IFC_ANIM::ImAnimBaseStateful*									comp
	);
	/*
	* ImGuiProAnim 动画控件删除
	* @param  input (FrmDat.DataAnimation), key (组件对象Hash.key)
	* @return state
	*/
	bool AnimCompDelete(std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input, const char* key);

	class ImAnimButton :public IFC_ANIM::ImAnimBaseStateful {
	protected:
		Vector4T<float> AnimationColorTrans = {}; // smooth transformation color. button
		Vector2T<float> AnimationSizeTrans  = {}; // scale size.

		uint32_t ControlFrameUniqueID = NULL;
	public:
		// hover = false, active = false, mode: normal
		// hover = true,  active = false, mode: hover
		// hover = true,  active = true,  mode: active
		void UpdateTick(bool hover, bool active, float smooth_scale) override;

		void ComponentsInit() override {
			AnimationColorTrans = ConfigNormalColor;
			AnimationSizeTrans  = ConfigNormalSize;
		}

		Vector4T<float>* AnimtransColor() override { return &AnimationColorTrans; };
		Vector2T<float>* AnimtransSize() override { return &AnimationSizeTrans; };
		Vector2T<float>* AnimtransPosition() override { return nullptr; }; // [EmptyIMP]

		Vector4T<float> ConfigNormalColor = {}, ConfigHoverColor = {}, ConfigActiveColor = {};
		Vector2T<float> ConfigNormalSize = {}, ConfigHoverSize = {}, ConfigActiveSize = {};
		
		float ConfigColorTransSpeed = 1.0f;
		float ConfigSizeTransSpeed  = 1.0f;

		uint32_t* _MS_UNIQUE() { return &ControlFrameUniqueID; };
	};

	/*
	* 创建按钮动画控件对象
	* @param  copy = nullptr (用于复制已配置好的对象)
	* @return ImAnimButton* 
	*/
	ImAnimButton* CreateAnimButton(ImAnimButton* copy = nullptr);

	/*
	* 调用按钮动画控件
	* @param  button_name (按钮名称), input (FrmDat.DataAnimation), comp_name (组件对象Key), trans_smooth = 1.0f (步进缩放)
	* @return bool
	*/
	bool CallAnimButton(
		const char*                                                     button_name, 
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*                                                     comp_name, 
		const float&                                                    trans_smooth = 1.0f
	);
	/*
	* 调用图片按钮动画控件
	* @param  button_name (按钮名称), input (FrmDat.DataAnimation), comp_name (组件对象Key), texture_hd (纹理句柄),
	* @param  trans_smooth = 1.0f (步进缩放)
	* @return bool
	*/
	bool CallAnimButtonImage(
		const char*                                                     button_name,
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*														comp_name,
		uint32_t														TextureHandle,
		const float&                                                    trans_smooth = 1.0f
	);

	class ImAnimFixedWindow :public IFC_ANIM::ImAnimBaseStateful {
	protected:
		Vector4T<float> AnimationColorTrans = {}; // smooth transformation color. window background
		Vector2T<float> AnimationSizeTrans  = {}; // scale size.

		uint32_t ControlFrameUniqueID = NULL;
	public:
		// hover = false, active = false, mode: close
		// hover = false, active = true,  mode: open
		void UpdateTick(bool hover, bool active, float smooth_scale) override;

		void ComponentsInit() override {
			AnimationColorTrans = ConfigCloseColor;
			AnimationSizeTrans  = ConfigCloseSize;
		}

		Vector4T<float>* AnimtransColor() override { return &AnimationColorTrans; };
		Vector2T<float>* AnimtransSize() override { return &AnimationSizeTrans; };
		Vector2T<float>* AnimtransPosition() override { return nullptr; }; // [EmptyIMP]

		Vector4T<float> ConfigCloseColor = {}, ConfigOpenColor = {};
		Vector2T<float> ConfigCloseSize = {}, ConfigOpenSize = {};

		float ConfigColorTransSpeed = 1.0f;
		float ConfigSizeTransSpeed  = 1.0f;

		uint32_t* _MS_UNIQUE() { return &ControlFrameUniqueID; };
	};

	/*
	* 创建固定(大小)窗口动画控件对象
	* @param  copy = nullptr (用于复制已配置好的对象)
	* @return ImAnimFixedWindow*
	*/
	ImAnimFixedWindow* CreateAnimFixedWindow(ImAnimFixedWindow* copy = nullptr);

	typedef int ImGuiWindowFlagsAm;
	/*
	* 调用固定(大小)窗口动画控件.
	* @param  window_name (窗口名称), input (FrmDat.DataAnimation), comp_name (组件对象Key), window_ui (子窗口函数对象),
	* @param  open_flag (imgui窗口"p_open"标志), flags = 0 (imgui窗口标志"ImGuiWindowFlags"), trans_smooth = 1.0f (步进缩放)
	* @return void
	*/
	void CallAnimFixedWindow(
		const char*														window_name,
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*														comp_name,
		bool&															open_flag,
		std::function<void()>											window_ui,
		ImGuiWindowFlagsAm												flags        = 0,
		const float&                                                    trans_smooth = 1.0f
	);
	
	class ImAnimSourceComp :public IFC_ANIM::ImAnimBaseStateful {
	protected:
		Vector4T<float> AnimationColorTrans    = {}; // smooth transformation color.
		Vector2T<float> AnimationSizeTrans     = {}; // scale size.
		Vector2T<float> AnimationPositionTrans = {}; // move position.

		uint32_t ControlFrameUniqueID = NULL;
	public:
		// hover = false, active = false, mode: low
		// hover = false, active = true,  mode: high
		void UpdateTick(bool hover, bool active, float smooth_scale) override;

		void ComponentsInit() override {
			AnimationColorTrans    = ConfigLowColor;
			AnimationSizeTrans     = ConfigLowSize;
			AnimationPositionTrans = ConfigLowPosition;
		}

		Vector4T<float>* AnimtransColor() override { return &AnimationColorTrans; };
		Vector2T<float>* AnimtransSize() override { return &AnimationSizeTrans; };
		Vector2T<float>* AnimtransPosition() override { return &AnimationPositionTrans; };

		Vector4T<float> ConfigLowColor = {}, ConfigHighColor = {};
		Vector2T<float> ConfigLowSize = {}, ConfigHighSize = {};
		Vector2T<float> ConfigLowPosition = {}, ConfigHighPosition = {};

		float ConfigColorTransSpeed    = 1.0f;
		float ConfigSizeTransSpeed     = 1.0f;
		float ConfigPositionTransSpeed = 1.0f;

		uint32_t* _MS_UNIQUE() { return &ControlFrameUniqueID; };
	};

	/*
	* 创建源动画控件对象 [移动 & 变色]
	* @param  copy = nullptr (用于复制已配置好的对象)
	* @return ImAnimSrcMoveColor*
	*/
	ImAnimSourceComp* CreateAnimSourceComp(ImAnimSourceComp* copy = nullptr);
	/*
	* 调用源动画控件 [移动 & 变色].
	* @param  input (FrmDat.DataAnimation), comp_name (组件对象Key), state_flag (状态切换),
	* @param  context (绘制函数对象), trans_smooth = 1.0f (步进缩放)
	* @return void
	*/
#define SYSDEF_FUNC_PARAM void(const Vector2T<float>&, const Vector4T<float>&) // update:[20231206]
	void CallAnimSourceComp(
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*														comp_name,
		const bool&                                                     state_flag,
		std::function<SYSDEF_FUNC_PARAM>			                    context,
		const float&                                                    trans_smooth = 1.0f
	);

	// 设置位置 (用于控件快速归位).
	void CallAnimSourceCompRES(
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*														comp_name,
		Vector4T<float>													setcolor,
		Vector2T<float>													setsize,
		Vector2T<float>													setpoition
	);
}

#define NODESFUNC [&](const Vector2T<float>& size, const Vector4T<float>& color, const bool& state)
namespace ImGuiProAnimNodes {
	/*
	* ImGuiProAnimNodes 动画节点控件注册
	* @param  input (FrmDat.DataAnimationNodes), key (组件对象Hash.key), comp (配置完成的组件对象)
	* @return state
	*/
	bool AnimNodesCompReg(
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseDynamic*>& input,
		const char*                                                    key,
		IFC_ANIM::ImAnimBaseDynamic*                                   comp
	);
	/*
	* ImGuiProAnimNodes 动画节点控件删除
	* @param  input (FrmDat.DataAnimationNodes), key (组件对象Hash.key)
	* @return state
	*/
	bool AnimNodesCompDelete(std::unordered_map<std::string, IFC_ANIM::ImAnimBaseDynamic*>& input, const char* key);

	class ImAnimNodesHashmap :public IFC_ANIM::ImAnimBaseDynamic {
	protected:
		std::unordered_map<std::string, IFC_ANIM::ImAnimDynamicNode> AnimationNodes = {};

		// smooth transformation.
		Vector4T<float> AnimationColorTrans[2]    = {};
		Vector2T<float> AnimationSizeTrans[2]     = {};
		Vector2T<float> AnimationPositionTrans[2] = {};
		// nodes flag.
		bool AnimationStateFlag = false;
	public:
		std::unordered_map<std::string, IFC_ANIM::ImAnimDynamicNode>* getnodes_refe() { return &AnimationNodes; }

		bool UpdateTickName(const char* node_key, const float& smooth_scale) override;
		bool UpdateTickCount(const size_t& count, const float& smooth_scale) override { return false; }; // [EmptyIMP]

		void ComponentsInit() override {
			auto it = AnimationNodes.begin();
			if (it != AnimationNodes.end()) {

				AnimationColorTrans[1]    = it->second.TransColor;
				AnimationSizeTrans[1]     = it->second.TransSize;
				AnimationPositionTrans[1] = it->second.TransPosition;
			}
			else
				LOGCONS::PushLogger(LOG_WARNING, SYSMD_LB_ANIM, "nodes_anim init hashmap.size == 0.");
		};

		void ConfigNodeAdd(const char* key, IFC_ANIM::ImAnimDynamicNode value) override;
		void ConfigNodeDelete(const char* key) override;
		IFC_ANIM::ImAnimDynamicNode* ConfigNodeFind(const char* key) override;

		Vector4T<float>* AnimtransColor() override { return &AnimationColorTrans[1]; };
		Vector2T<float>* AnimtransSize() override { return &AnimationSizeTrans[1]; };
		Vector2T<float>* AnimtransPosition() override { return &AnimationPositionTrans[1]; };

		bool* AnimFlagState() { return &AnimationStateFlag; };

		float ConfigColorTransSpeed    = 1.0f;
		float ConfigSizeTransSpeed     = 1.0f;
		float ConfigPositionTransSpeed = 1.0f;
	};

	class ImAnimNodesVector :public IFC_ANIM::ImAnimBaseDynamic {
	protected:
		// no unique key.
		std::vector<std::pair<std::string, IFC_ANIM::ImAnimDynamicNode>> AnimationNodes = {};

		// smooth transformation.
		Vector4T<float> AnimationColorTrans[2]    = {};
		Vector2T<float> AnimationSizeTrans[2]     = {};
		Vector2T<float> AnimationPositionTrans[2] = {};
		// nodes flag.
		bool AnimationStateFlag = false;
	public:
		std::vector<std::pair<std::string, IFC_ANIM::ImAnimDynamicNode>>* getnodes_refe() { return &AnimationNodes; }

		bool UpdateTickName(const char* node_key, const float& smooth_scale) override { return false; }; // [EmptyIMP]
		bool UpdateTickCount(const size_t& count, const float& smooth_scale) override;

		void ComponentsInit() override {
			if (AnimationNodes.size() > NULL) {

				AnimationColorTrans[1]    = AnimationNodes[0].second.TransColor;
				AnimationSizeTrans[1]     = AnimationNodes[0].second.TransSize;
				AnimationPositionTrans[1] = AnimationNodes[0].second.TransPosition;
			}
			else
				LOGCONS::PushLogger(LOG_WARNING, SYSMD_LB_ANIM, "nodes_anim init vector.size == 0.");
		};

		void ConfigNodeAdd(const char* key, IFC_ANIM::ImAnimDynamicNode value) override;
		void ConfigNodeDelete(const char* key) override;
		IFC_ANIM::ImAnimDynamicNode* ConfigNodeFind(const char* key) override;

		Vector4T<float>* AnimtransColor() override { return &AnimationColorTrans[1]; };
		Vector2T<float>* AnimtransSize() override { return &AnimationSizeTrans[1]; };
		Vector2T<float>* AnimtransPosition() override { return &AnimationPositionTrans[1]; };

		bool* AnimFlagState() { return &AnimationStateFlag; };

		float ConfigColorTransSpeed    = 1.0f;
		float ConfigSizeTransSpeed     = 1.0f;
		float ConfigPositionTransSpeed = 1.0f;
	};

	enum AnimCompType {
		NodesHashmap = 1 << 1,
		NodesVector  = 1 << 2
	};
	IFC_ANIM::ImAnimBaseDynamic* CreateAnimNodes(
		AnimCompType                 type,
		IFC_ANIM::ImAnimBaseDynamic* copy  = nullptr,
		const Vector3T<float>&       speed = Vector3T<float>(1.0f, 1.0f, 1.0f)
	);

	IFC_ANIM::ImAnimDynamicNode ConfigAnimNode(
		const bool&            state,
		const Vector2T<float>& pos,
		const Vector2T<float>& size,
		const Vector4T<float>& color = Vector4T<float>(1.0f, 1.0f, 1.0f, 1.0f)
	);

#define SYSDEF_FUNC_NODES_PARAM void(const Vector2T<float>&, const Vector4T<float>&, const bool&)
	bool CallAnimNodesHashmap(
		const char*                                                    nodes_key,
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseDynamic*>& input,
		const char*                                                    comp_name,
		std::function<SYSDEF_FUNC_NODES_PARAM>		                   context,
		const float&												   trans_smooth = 1.0f
	);

	bool CallAnimNodesVector(
		const size_t&                                                      nodes_count,
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseDynamic*>&     input,
		const char*                                                        comp_name,
		std::function<SYSDEF_FUNC_NODES_PARAM>                             context,
		const float&												       trans_smooth = 1.0f
	);
}

#endif