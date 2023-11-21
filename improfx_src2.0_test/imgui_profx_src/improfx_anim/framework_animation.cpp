// framework_animation.
#include "framework_animation.hpp"

using namespace std;
using namespace LOGCONS;

void TransVec4F(Vector4T<float>& input_src, const Vector4T<float> input_tag, float cst, float speed, float smooth) {
	// vec4f: x += (s - x) * const * speed * smooth_scale
	input_src.vector_x += (input_tag.vector_x - input_src.vector_x) * cst * speed * smooth;
	input_src.vector_y += (input_tag.vector_y - input_src.vector_y) * cst * speed * smooth;
	input_src.vector_z += (input_tag.vector_z - input_src.vector_z) * cst * speed * smooth;
	input_src.vector_w += (input_tag.vector_w - input_src.vector_w) * cst * speed * smooth;
}

void TransVec2F(Vector2T<float>& input_src, const Vector2T<float> input_tag, float cst, float speed, float smooth) {
	// vec2f: x += (s - x) * const * speed * smooth_scale
	input_src.vector_x += (input_tag.vector_x - input_src.vector_x) * cst * speed * smooth;
	input_src.vector_y += (input_tag.vector_y - input_src.vector_y) * cst * speed * smooth;
}

namespace ImGuiProAnim {

	void ImAnimButton::update_tick(bool hover, bool active, float smooth_scale) {
		if (!hover && !active) {
			// normal state color & size.
			TransVec4F(anim_color, config_normal_color, 0.08f, config_color_transspeed, smooth_scale);
			TransVec2F(anim_size, config_normal_size, 0.08f, config_size_transspeed, smooth_scale);
		}
		else if (hover && !active) {
			// hover state color & size.
			TransVec4F(anim_color, config_hover_color, 0.05f, config_color_transspeed, smooth_scale);
			TransVec2F(anim_size, config_hover_size, 0.05f, config_size_transspeed, smooth_scale);
		}
		else if (active) {
			// active state color & size.
			TransVec4F(anim_color, config_active_color, 0.072f, config_color_transspeed, smooth_scale);
			TransVec2F(anim_size, config_active_size, 0.072f, config_size_transspeed, smooth_scale);
		}
	}

	void ImAnimFixedWindow::update_tick(bool hover, bool active, float smooth_scale) {
		if (active) {
			// open state color & size.
			TransVec4F(anim_color, config_open_color, 0.048f, config_color_transspeed, smooth_scale);
			TransVec2F(anim_size, config_open_size, 0.048f, config_size_transspeed, smooth_scale);
		}
		else {
			// close state color & size.
			TransVec4F(anim_color, config_close_color, 0.128f, config_color_transspeed, smooth_scale);
			TransVec2F(anim_size, config_close_size, 0.128f, config_size_transspeed, smooth_scale);
		}
	}

	void ImAnimSrcMoveColor::update_tick(bool hover, bool active, float smooth_scale) {
		if (active) {
			// open state color & position.
			TransVec4F(anim_color, config_high_color, 0.048f, config_color_transspeed, smooth_scale);
			TransVec2F(anim_position, config_high_position, 0.05f, config_position_transspeed, smooth_scale);
		}
		else {
			// close state color & position.
			TransVec4F(anim_color, config_low_color, 0.128f, config_color_transspeed, smooth_scale);
			TransVec2F(anim_position, config_low_position, 0.128f, config_position_transspeed, smooth_scale);
		}
	}
}