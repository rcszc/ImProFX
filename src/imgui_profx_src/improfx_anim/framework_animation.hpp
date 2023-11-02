// framework_animation. [2023_11_01] RCSZ.

#ifndef _FRAMEWORK_ANIMATION_H
#define _FRAMEWORK_ANIMATION_H
#include "../improfx_log/framework_log.hpp"

#define SYSTEM_MODULE_LABLE_animation std::string("[im_animation]: ")

namespace ImGuiProAnim {

	class ImAnimationBase {
	public:
		virtual void update_tick(bool hover, bool active, float smooth_scale) = 0;
		virtual void comp_init() = 0;

		virtual Vector4T<float> animtrans_color() = 0;
		virtual Vector2T<float> animtrans_size() = 0;
		virtual Vector2T<float> animtrans_position() = 0;
	};

	/*
	* ImGuiProAnim 动画控件注册
	* @param  input (DataCompAnim), key (组件对象Hash.key), comp (配置完成的组件对象)
	* @return void
	*/
	void AnimCompReg(std::unordered_map<std::string, ImAnimationBase*>& input, const char* key, ImAnimationBase* comp);

	class ImAnimButton :public ImAnimationBase {
	protected:
		Vector4T<float> anim_color = {}; // smooth transformation color. button
		Vector2T<float> anim_size = {};  // scale size.
		
	public:
		// hover = false, active = false, mode: normal
		// hover = true,  active = false, mode: hover
		// hover = true,  active = true,  mode: active
		void update_tick(bool hover, bool active, float smooth_scale) override;

		void comp_init() override {
			anim_color = config_normal_color;
			anim_size  = config_normal_size;
		}

		Vector4T<float> animtrans_color() override { return anim_color; };
		Vector2T<float> animtrans_size() override { return anim_size; };
		Vector2T<float> animtrans_position() override { return Vector2T<float>(); };

		Vector4T<float> config_normal_color = {}, config_hover_color = {}, config_active_color = {};
		Vector2T<float> config_normal_size = {}, config_hover_size = {}, config_active_size = {};
		
		float config_color_transspeed = 1.0f;
		float config_size_transspeed  = 1.0f;
	};

	/*
	* 创建按钮动画控件对象
	* @param  copy = nullptr (用于复制已配置好的对象)
	* @return ImAnimButton* 
	*/
	ImAnimButton* CreateAnimButton(ImAnimButton* copy = nullptr);

	/*
	* 调用按钮动画控件
	* @param  unique_name (按钮文本[Unique]), input (DataCompAnim), comp_name (组件对象Key), trans_smooth = 1.0f (FPS平滑缩放参数)
	* @return bool
	*/
	bool CallAnimButton(
		const char*                                        unique_name, 
		std::unordered_map<std::string, ImAnimationBase*>& input, 
		const char*                                        comp_name, 
		float                                              trans_smooth = 1.0f
	);

	/*
	* 调用图片按钮动画控件
	* @param  unique_name (按钮文本[Unique]), input (DataCompAnim), comp_name (组件对象Key), texture_hd (纹理句柄),
	* @param  trans_smooth = 1.0f (FPS平滑缩放参数)
	* @return bool
	*/
	bool CallAnimButtonImage(
		const char*                                        unique_name,
		std::unordered_map<std::string, ImAnimationBase*>& input,
		const char*                                        comp_name,
		uint32_t                                           texture_hd,
		float                                              trans_smooth = 1.0f
	);

	class ImAnimFixedWindow :public ImAnimationBase {
	protected:
		Vector4T<float> anim_color = {}; // smooth transformation color. window background
		Vector2T<float> anim_size = {};  // scale size.

	public:
		// hover = false, active = false, mode: close
		// hover = false, active = true,  mode: open
		void update_tick(bool hover, bool active, float smooth_scale) override;

		void comp_init() override {
			anim_color = config_close_color;
			anim_size = config_close_size;
		}

		Vector4T<float> animtrans_color() override { return anim_color; };
		Vector2T<float> animtrans_size() override { return anim_size; };
		Vector2T<float> animtrans_position() override { return Vector2T<float>(); };

		Vector4T<float> config_close_color = {}, config_open_color = {};
		Vector2T<float> config_close_size = {}, config_open_size = {};

		float config_color_transspeed = 1.0f;
		float config_size_transspeed = 1.0f;
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
	* @param  unique_name (按钮文本[Unique]), input (DataCompAnim), comp_name (组件对象Key), window_ui (子窗口包装),
	* @param  open_flag (imgui窗口p_open标志), flags = 0 (imgui窗口标志[ImGuiWindowFlags]), trans_smooth = 1.0f (FPS平滑缩放参数)
	* @return void
	*/
	void CallAnimFixedWindow(
		const char*                                        unique_name,
		std::unordered_map<std::string, ImAnimationBase*>& input,
		const char*                                        comp_name,
		bool&                                              open_flag,
		std::function<void()>                              window_ui,
		ImGuiWindowFlagsAm                                 flags = 0,
		float                                              trans_smooth = 1.0f
	);
}

#endif