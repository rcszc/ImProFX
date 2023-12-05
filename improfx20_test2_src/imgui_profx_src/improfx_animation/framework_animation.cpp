// framework_animation.
#include "framework_animation.h"

using namespace std;
using namespace LOGCONS;

namespace vector_transcalc {
	// vector2 float transformation.
	void TransVec2F(Vector2T<float>& input_src, const Vector2T<float>& input_tag, const float& cst, const float& speed, const float& smooth) {
		// vec2f: x += (s - x) * const * speed * smooth_scale
		input_src.vector_x += (input_tag.vector_x - input_src.vector_x) * cst * speed * smooth;
		input_src.vector_y += (input_tag.vector_y - input_src.vector_y) * cst * speed * smooth;
	}
	// vector4 float transformation.
	void TransVec4F(Vector4T<float>& input_src, const Vector4T<float>& input_tag, const float& cst, const float& speed, const float& smooth) {
		// vec4f: x += (s - x) * const * speed * smooth_scale
		input_src.vector_x += (input_tag.vector_x - input_src.vector_x) * cst * speed * smooth;
		input_src.vector_y += (input_tag.vector_y - input_src.vector_y) * cst * speed * smooth;
		input_src.vector_z += (input_tag.vector_z - input_src.vector_z) * cst * speed * smooth;
		input_src.vector_w += (input_tag.vector_w - input_src.vector_w) * cst * speed * smooth;
	}
}

namespace ImGuiProAnim {

	void ImAnimButton::UpdateTick(bool hover, bool active, float smooth_scale) {
		if (!hover && !active) {
			// normal state color & size.
			vector_transcalc::TransVec4F(anim_color, ConfigNormalColor, 0.08f, ConfigColorTransSpeed, smooth_scale);
			vector_transcalc::TransVec2F(anim_size, ConfigNormalSize, 0.08f, ConfigSizeTransSpeed, smooth_scale);
		}
		else if (hover && !active) {
			// hover state color & size.
			vector_transcalc::TransVec4F(anim_color, ConfigHoverColor, 0.05f, ConfigColorTransSpeed, smooth_scale);
			vector_transcalc::TransVec2F(anim_size, ConfigHoverSize, 0.05f, ConfigSizeTransSpeed, smooth_scale);
		}
		else if (active) {
			// active state color & size.
			vector_transcalc::TransVec4F(anim_color, ConfigActiveColor, 0.072f, ConfigColorTransSpeed, smooth_scale);
			vector_transcalc::TransVec2F(anim_size, ConfigActiveSize, 0.072f, ConfigSizeTransSpeed, smooth_scale);
		}
	}

	void ImAnimFixedWindow::UpdateTick(bool hover, bool active, float smooth_scale) {
		if (active) {
			// open state color & size.
			vector_transcalc::TransVec4F(anim_color, ConfigOpenColor, 0.048f, ConfigColorTransSpeed, smooth_scale);
			vector_transcalc::TransVec2F(anim_size, ConfigOpenSize, 0.048f, ConfigSizeTransSpeed, smooth_scale);
		}
		else {
			// close state color & size.
			vector_transcalc::TransVec4F(anim_color, ConfigCloseColor, 0.128f, ConfigColorTransSpeed, smooth_scale);
			vector_transcalc::TransVec2F(anim_size, ConfigCloseSize, 0.128f, ConfigSizeTransSpeed, smooth_scale);
		}
	}

	void ImAnimSourceComp::UpdateTick(bool hover, bool active, float smooth_scale) {
		if (active) {
			// open state color & position.
			vector_transcalc::TransVec4F(anim_color, ConfigHighColor, 0.048f, ConfigColorTransSpeed, smooth_scale);
			vector_transcalc::TransVec2F(anim_size, ConfigHighSize, 0.042f, ConfigSizeTransSpeed, smooth_scale);
			vector_transcalc::TransVec2F(anim_position, ConfigHighPosition, 0.05f, ConfigPositionTransSpeed, smooth_scale);
		}
		else {
			// close state color & position.
			vector_transcalc::TransVec4F(anim_color, ConfigLowColor, 0.128f, ConfigColorTransSpeed, smooth_scale);
			vector_transcalc::TransVec2F(anim_size, ConfigLowSize, 0.042f, ConfigSizeTransSpeed, smooth_scale);
			vector_transcalc::TransVec2F(anim_position, ConfigLowPosition, 0.128f, ConfigPositionTransSpeed, smooth_scale);
		}
	}
}
