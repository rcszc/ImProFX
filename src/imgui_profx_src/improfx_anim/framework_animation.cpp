// framework_animation.
#include "framework_animation.hpp"

using namespace std;
using namespace LOGCONS;

void TransVec4F(Vector4T<float>& input_src, const Vector4T<float> input_tag, float speed, float smooth) {
	// vec4f: x += (s - x) * const * speed * smooth
	input_src.vector_x += (input_tag.vector_x - input_src.vector_x) * 0.05f * speed * smooth;
	input_src.vector_y += (input_tag.vector_y - input_src.vector_y) * 0.05f * speed * smooth;
	input_src.vector_z += (input_tag.vector_z - input_src.vector_z) * 0.05f * speed * smooth;
	input_src.vector_w += (input_tag.vector_w - input_src.vector_w) * 0.05f * speed * smooth;
}

void TransVec2F(Vector2T<float>& input_src, const Vector2T<float> input_tag, float speed, float smooth) {
	// vec2f: x += (s - x) * const * speed * smooth
	input_src.vector_x += (input_tag.vector_x - input_src.vector_x) * 0.05f * speed * smooth;
	input_src.vector_y += (input_tag.vector_y - input_src.vector_y) * 0.05f * speed * smooth;
}

namespace ImGuiProAnim {

	void ImAnimButton::update_tick(bool hover, bool active, float smooth_scale) {
		if (!hover && !active) {
			// normal state color.
			TransVec4F(anim_color, config_normal_color, config_color_transspeed, smooth_scale);
			// normal state size.
			TransVec2F(anim_size, config_normal_size, config_size_transspeed, smooth_scale);
		}
		else if (hover && !active) {
			// hover state color.
			TransVec4F(anim_color, config_hover_color, config_color_transspeed, smooth_scale);
			// hover state size.
			TransVec2F(anim_size, config_hover_size, config_size_transspeed, smooth_scale);
		}
		else if (active) {
			// active state color.
			TransVec4F(anim_color, config_active_color, config_color_transspeed, smooth_scale);
			// active state size.
			TransVec2F(anim_size, config_active_size, config_size_transspeed, smooth_scale);
		}
	}

	void ImAnimFixedWindow::update_tick(bool hover, bool active, float smooth_scale) {
		if (active) {
			// open state color.
			TransVec4F(anim_color, config_open_color, config_color_transspeed, smooth_scale);
			// open state size.
			TransVec2F(anim_size, config_open_size, config_size_transspeed, smooth_scale);
		}
		else {
			// close state color.
			TransVec4F(anim_color, config_close_color, config_color_transspeed, smooth_scale);
			// close state size.
			TransVec2F(anim_size, config_close_size, config_size_transspeed, smooth_scale);
		}
	}
}