// framework_animation. [2023_11_01] RCSZ.

#ifndef _FRAMEWORK_ANIMATION_H
#define _FRAMEWORK_ANIMATION_H
#include "framework_ainmation_interface.h"

#define SYSMD_LB_ANIM "[im_animation]: "

namespace vector_transcalc {
	void TransVec2F(Vector2T<float>& input_src, const Vector2T<float>& input_tag, const float& cst, const float& speed, const float& smooth);
	void TransVec4F(Vector4T<float>& input_src, const Vector4T<float>& input_tag, const float& cst, const float& speed, const float& smooth);
}

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
		Vector4T<float> anim_color = {}; // smooth transformation color. button
		Vector2T<float> anim_size  = {}; // scale size.
		
	public:
		// hover = false, active = false, mode: normal
		// hover = true,  active = false, mode: hover
		// hover = true,  active = true,  mode: active
		void UpdateTick(bool hover, bool active, float smooth_scale) override;

		void ComponentsInit() override {
			anim_color = ConfigNormalColor;
			anim_size  = ConfigNormalSize;
		}

		Vector4T<float>* AnimtransColor() override { return &anim_color; };
		Vector2T<float>* AnimtransSize() override { return &anim_size; };
		Vector2T<float>* AnimtransPosition() override { return nullptr; };

		Vector4T<float> ConfigNormalColor = {}, ConfigHoverColor = {}, ConfigActiveColor = {};
		Vector2T<float> ConfigNormalSize = {}, ConfigHoverSize = {}, ConfigActiveSize = {};
		
		float ConfigColorTransSpeed = 1.0f;
		float ConfigSizeTransSpeed  = 1.0f;
	};

	/*
	* 创建按钮动画控件对象
	* @param  copy = nullptr (用于复制已配置好的对象)
	* @return ImAnimButton* 
	*/
	ImAnimButton* CreateAnimButton(ImAnimButton* copy = nullptr);

	/*
	* 调用按钮动画控件
	* @param  unique_name (按钮文本[Unique]), input (FrmDat.DataAnimation), comp_name (组件对象Key), trans_smooth = 1.0f (步进缩放)
	* @return bool
	*/
	bool CallAnimButton(
		const char*                                                     unique_name, 
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*                                                     comp_name, 
		float                                                           trans_smooth = 1.0f
	);

	/*
	* 调用图片按钮动画控件
	* @param  unique_name (按钮文本[Unique]), input (FrmDat.DataAnimation), comp_name (组件对象Key), texture_hd (纹理句柄),
	* @param  trans_smooth = 1.0f (步进缩放)
	* @return bool
	*/
	bool CallAnimButtonImage(
		const char*														unique_name,
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*														comp_name,
		uint32_t														texture_hd,
		float															trans_smooth = 1.0f
	);

	class ImAnimFixedWindow :public IFC_ANIM::ImAnimBaseStateful {
	protected:
		Vector4T<float> anim_color = {}; // smooth transformation color. window background
		Vector2T<float> anim_size  = {}; // scale size.

	public:
		// hover = false, active = false, mode: close
		// hover = false, active = true,  mode: open
		void UpdateTick(bool hover, bool active, float smooth_scale) override;

		void ComponentsInit() override {
			anim_color = ConfigCloseColor;
			anim_size  = ConfigCloseSize;
		}

		Vector4T<float>* AnimtransColor() override { return &anim_color; };
		Vector2T<float>* AnimtransSize() override { return &anim_size; };
		Vector2T<float>* AnimtransPosition() override { return nullptr; };

		Vector4T<float> ConfigCloseColor = {}, ConfigOpenColor = {};
		Vector2T<float> ConfigCloseSize = {}, ConfigOpenSize = {};

		float ConfigColorTransSpeed = 1.0f;
		float ConfigSizeTransSpeed  = 1.0f;
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
	* @param  unique_name (按钮文本[Unique]), input (FrmDat.DataAnimation), comp_name (组件对象Key), window_ui (子窗口函数对象),
	* @param  open_flag (imgui窗口"p_open"标志), flags = 0 (imgui窗口标志"ImGuiWindowFlags"), trans_smooth = 1.0f (步进缩放)
	* @return void
	*/
	void CallAnimFixedWindow(
		const char*														unique_name,
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*														comp_name,
		bool&															open_flag,
		std::function<void()>											window_ui,
		ImGuiWindowFlagsAm												flags        = 0,
		float															trans_smooth = 1.0f
	);
	
	class ImAnimSourceComp :public IFC_ANIM::ImAnimBaseStateful {
	protected:
		Vector4T<float> anim_color    = {}; // smooth transformation color.
		Vector2T<float> anim_size     = {}; // scale size.
		Vector2T<float> anim_position = {}; // move position.

	public:
		// hover = false, active = false, mode: low
		// hover = false, active = true,  mode: high
		void UpdateTick(bool hover, bool active, float smooth_scale) override;

		void ComponentsInit() override {
			anim_color    = ConfigLowColor;
			anim_size     = ConfigLowSize;
			anim_position = ConfigLowPosition;
		}

		Vector4T<float>* AnimtransColor() override { return &anim_color; };
		Vector2T<float>* AnimtransSize() override { return &anim_size; };
		Vector2T<float>* AnimtransPosition() override { return &anim_position; };

		Vector4T<float> ConfigLowColor = {}, ConfigHighColor = {};
		Vector2T<float> ConfigLowSize = {}, ConfigHighSize = {};
		Vector2T<float> ConfigLowPosition = {}, ConfigHighPosition = {};

		float ConfigColorTransSpeed    = 1.0f;
		float ConfigSizeTransSpeed     = 1.0f;
		float ConfigPositionTransSpeed = 1.0f;
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
	* @param  context (颜色变换&大小变换 通过参数传入, 函数山下文内绘制位置变换), trans_smooth = 1.0f (步进缩放)
	* @return void
	*/
	void CallAnimSourceComp(
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input,
		const char*														comp_name,
		bool															state_flag,
		std::function<void(Vector4T<float>, Vector2T<float>)>			context,
		float															trans_smooth = 1.0f
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

#endif