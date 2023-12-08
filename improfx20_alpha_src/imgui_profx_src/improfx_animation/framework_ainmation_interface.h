// framework_ainmation_interface.
// [INTERFACE].

#ifndef _FRAMEWORK_AINMATION_INTERFACE_H
#define _FRAMEWORK_AINMATION_INTERFACE_H
#include "../improfx_log/framework_log.hpp"

// [2023_11_25](1): ImProFx��ܶ����ؼ��ӿ�.
namespace IFC_ANIM {

	// animation_base stateful(�̶��ڵ㶯���任)
	class ImAnimBaseStateful {
	public:
		virtual void UpdateTick(bool hover, bool active, float smooth_scale) = 0;
		virtual void ComponentsInit() = 0;

		virtual Vector4T<float>* AnimtransColor() = 0;
		virtual Vector2T<float>* AnimtransSize() = 0;
		virtual Vector2T<float>* AnimtransPosition() = 0;
	};

	// animation dynamic node data.
	struct ImAnimDynamicNode {
		bool RenderState;

		Vector4T<float> TransColor;
		Vector2T<float> TransSize;
		Vector2T<float> TransPosition;

		constexpr ImAnimDynamicNode() :RenderState(false), TransColor(Vector4T<float>()), TransSize(Vector2T<float>()), TransPosition(Vector2T<float>()) {}
		constexpr ImAnimDynamicNode(const bool& stat, const Vector2T<float>& pos, const Vector2T<float>& size, const Vector4T<float>& color) :
			RenderState   (stat),
			TransColor    (color),
			TransSize     (size),
			TransPosition (pos)
		{}
	};

	// animation_base dynamic(��̬�ڵ㶯���任)
	class ImAnimBaseDynamic {
	public:
		virtual bool UpdateTickName(const char* node_key, float smooth_scale) = 0;
		virtual bool UpdateTickCount(size_t count, float smooth_scale) = 0;

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