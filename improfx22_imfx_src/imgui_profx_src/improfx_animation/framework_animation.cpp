// framework_animation.
#include "framework_animation.h"

using namespace std;
using namespace LOGCONS;

// vector2 float interpolation transformation.
void interpolation2f(Vector2T<float>& input_src, const Vector2T<float>& input_tag, const float& cst, const float& speed, const float& smooth) {
	// vec2f: x += (s - x) * const * speed * smooth_scale
	input_src.vector_x += (input_tag.vector_x - input_src.vector_x) * cst * speed * smooth;
	input_src.vector_y += (input_tag.vector_y - input_src.vector_y) * cst * speed * smooth;
}

// vector4 float interpolation transformation.
void interpolation4f(Vector4T<float>& input_src, const Vector4T<float>& input_tag, const float& cst, const float& speed, const float& smooth) {
	// vec4f: x += (s - x) * const * speed * smooth_scale
	input_src.vector_x += (input_tag.vector_x - input_src.vector_x) * cst * speed * smooth;
	input_src.vector_y += (input_tag.vector_y - input_src.vector_y) * cst * speed * smooth;
	input_src.vector_z += (input_tag.vector_z - input_src.vector_z) * cst * speed * smooth;
	input_src.vector_w += (input_tag.vector_w - input_src.vector_w) * cst * speed * smooth;
}

namespace ImGuiProAnim {

	void ImAnimButton::UpdateTick(bool hover, bool active, float smooth_scale) {
		if (!hover && !active) {
			// normal state color & size.
			interpolation4f(AnimationColorTrans, ConfigNormalColor, 0.08f, ConfigColorTransSpeed, smooth_scale);
			interpolation2f(AnimationSizeTrans, ConfigNormalSize, 0.08f, ConfigSizeTransSpeed, smooth_scale);
		}
		else if (hover && !active) {
			// hover state color & size.
			interpolation4f(AnimationColorTrans, ConfigHoverColor, 0.05f, ConfigColorTransSpeed, smooth_scale);
			interpolation2f(AnimationSizeTrans, ConfigHoverSize, 0.05f, ConfigSizeTransSpeed, smooth_scale);
		}
		else if (hover && active) {
			// active state color & size.
			interpolation4f(AnimationColorTrans, ConfigActiveColor, 0.072f, ConfigColorTransSpeed, smooth_scale);
			interpolation2f(AnimationSizeTrans, ConfigActiveSize, 0.072f, ConfigSizeTransSpeed, smooth_scale);
		}
	}

	void ImAnimFixedWindow::UpdateTick(bool hover, bool active, float smooth_scale) {
		if (active) {
			// open state color & size.
			interpolation4f(AnimationColorTrans, ConfigOpenColor, 0.05f, ConfigColorTransSpeed, smooth_scale);
			interpolation2f(AnimationSizeTrans, ConfigOpenSize, 0.05f, ConfigSizeTransSpeed, smooth_scale);
		}
		else {
			// close state color & size.
			interpolation4f(AnimationColorTrans, ConfigCloseColor, 0.128f, ConfigColorTransSpeed, smooth_scale);
			interpolation2f(AnimationSizeTrans, ConfigCloseSize, 0.128f, ConfigSizeTransSpeed, smooth_scale);
		}
	}

	void ImAnimSourceComp::UpdateTick(bool hover, bool active, float smooth_scale) {
		if (active) {
			// open state color & size & position.
			interpolation4f(AnimationColorTrans, ConfigHighColor, 0.05f, ConfigColorTransSpeed, smooth_scale);
			interpolation2f(AnimationSizeTrans, ConfigHighSize, 0.042f, ConfigSizeTransSpeed, smooth_scale);
			interpolation2f(AnimationPositionTrans, ConfigHighPosition, 0.05f, ConfigPositionTransSpeed, smooth_scale);
		}
		else {
			// close state color & size & position.
			interpolation4f(AnimationColorTrans, ConfigLowColor, 0.128f, ConfigColorTransSpeed, smooth_scale);
			interpolation2f(AnimationSizeTrans, ConfigLowSize, 0.042f, ConfigSizeTransSpeed, smooth_scale);
			interpolation2f(AnimationPositionTrans, ConfigLowPosition, 0.128f, ConfigPositionTransSpeed, smooth_scale);
		}
	}
}

namespace ImGuiProAnimNodes {

	bool ImAnimNodesHashmap::UpdateTickName(const char* node_key, const float& smooth_scale) {
		bool returnflag = false;

		auto it = AnimationNodes.find(node_key);
		if (it != AnimationNodes.end()) {
			returnflag = true;

			AnimationColorTrans[0]    = it->second.TransColor;
			AnimationSizeTrans[0]     = it->second.TransSize;
			AnimationPositionTrans[0] = it->second.TransPosition;
			AnimationStateFlag        = it->second.RenderState;
		}
		else
			returnflag = false;
		// transformation color & size & position.
		interpolation4f(AnimationColorTrans[1], AnimationColorTrans[0], 0.05f, ConfigColorTransSpeed, smooth_scale);
		interpolation2f(AnimationSizeTrans[1], AnimationSizeTrans[0], 0.05f, ConfigSizeTransSpeed, smooth_scale);
		interpolation2f(AnimationPositionTrans[1], AnimationPositionTrans[0], 0.05f, ConfigPositionTransSpeed, smooth_scale);
		return returnflag;
	}

	bool ImAnimNodesVector::UpdateTickCount(const size_t& count, const float& smooth_scale) {
		bool returnflag = false;

		if (AnimationNodes.size() > count) {
			returnflag = true;

			AnimationColorTrans[0]    = AnimationNodes[count].second.TransColor;
			AnimationSizeTrans[0]     = AnimationNodes[count].second.TransSize;
			AnimationPositionTrans[0] = AnimationNodes[count].second.TransPosition;
			AnimationStateFlag        = AnimationNodes[count].second.RenderState;
		}
		else
			returnflag = false;
		// transformation color & size & position.
		interpolation4f(AnimationColorTrans[1], AnimationColorTrans[0], 0.05f, ConfigColorTransSpeed, smooth_scale);
		interpolation2f(AnimationSizeTrans[1], AnimationSizeTrans[0], 0.05f, ConfigSizeTransSpeed, smooth_scale);
		interpolation2f(AnimationPositionTrans[1], AnimationPositionTrans[0], 0.05f, ConfigPositionTransSpeed, smooth_scale);
		return returnflag;
	}
}