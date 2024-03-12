// framework_ainmation_interface.
// [INTERFACE].

#ifndef _FRAMEWORK_AINMATION_INTERFACE_H
#define _FRAMEWORK_AINMATION_INTERFACE_H
#include "../improfx_log/framework_log.hpp"

// [2023_11_25](1): ImProFx框架动画控件接口.
namespace IFC_ANIM {

	// animation_base stateful(固定节点动画变换)
	class ImAnimBaseStateful {
	public:
		virtual void UpdateTick(bool hover, bool active, float smooth_scale) = 0;
		virtual void ComponentsInit() = 0;

		virtual Vector4T<float>* AnimtransColor() = 0;
		virtual Vector2T<float>* AnimtransSize() = 0;
		virtual Vector2T<float>* AnimtransPosition() = 0;

		virtual uint32_t* _MS_UNIQUE() = 0;
	};

	// animation dynamic node data.
	struct ImAnimDynamicNode {
		bool RenderState;

		Vector4T<float> TransColor;
		Vector2T<float> TransSize;
		Vector2T<float> TransPosition;

		constexpr ImAnimDynamicNode() :RenderState(false), TransColor(Vector4T<float>()), TransSize(Vector2T<float>()), TransPosition(Vector2T<float>()) {}
		constexpr ImAnimDynamicNode(const bool& state, const Vector2T<float>& position, const Vector2T<float>& size, const Vector4T<float>& color) :
			RenderState   (state),
			TransColor    (color),
			TransSize     (size),
			TransPosition (position)
		{}
	};

	// animation_base dynamic(动态节点动画变换)
	class ImAnimBaseDynamic {
	public:
		virtual bool UpdateTickName(const char* node_key, const float& smooth_scale) = 0;
		virtual bool UpdateTickCount(const size_t& count, const float& smooth_scale) = 0;

		virtual void ComponentsInit() = 0;

		virtual void ConfigNodeAdd(const char* key, ImAnimDynamicNode value) = 0;
		virtual void ConfigNodeDelete(const char* key) = 0;
		virtual ImAnimDynamicNode* ConfigNodeFind(const char* key) = 0;

		virtual Vector4T<float>* AnimtransColor() = 0;
		virtual Vector2T<float>* AnimtransSize() = 0;
		virtual Vector2T<float>* AnimtransPosition() = 0;

		virtual bool* AnimFlagState() = 0;
	};
}

#endif