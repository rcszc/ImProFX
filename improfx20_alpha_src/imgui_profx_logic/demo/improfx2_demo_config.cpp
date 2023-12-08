// improfx2_demo_config.
#include "improfx2_demo.h"

using namespace std;
using namespace ProFxFramework;

void ImprofxDemoClass::ConfigAnimation(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat) {
	// -------------------------------- ImGuiProAnim --------------------------------

	auto MainPlayButtonA = ImGuiProAnim::CreateAnimButton();

	MainPlayButtonA->ConfigNormalSize = Vector2T<float>(384.0f, 32.0f);
	MainPlayButtonA->ConfigHoverSize = Vector2T<float>(384.0f, 36.0f);
	MainPlayButtonA->ConfigActiveSize = Vector2T<float>(384.0f, 28.0f);

	MainPlayButtonA->ConfigNormalColor = Vector4T<float>(0.42f, 0.0f, 1.0f, 0.32f);
	MainPlayButtonA->ConfigHoverColor = Vector4T<float>(1.0f, 0.32f, 0.0f, 0.58f);
	MainPlayButtonA->ConfigActiveColor = Vector4T<float>(1.0f, 0.32f, 0.0f, 0.85f);

	auto MainPlayButtonB = ImGuiProAnim::CreateAnimButton(MainPlayButtonA);
	auto MainPlayParticle = ImGuiProAnim::CreateAnimButton(MainPlayButtonA);

	MainPlayParticle->ConfigNormalSize = Vector2T<float>(512.0f, 32.0f);
	MainPlayParticle->ConfigHoverSize = Vector2T<float>(384.0f, 32.0f);
	MainPlayParticle->ConfigActiveSize = Vector2T<float>(480.0f, 32.0f);

	auto MainPlayNext = ImGuiProAnim::CreateAnimButton(MainPlayParticle);

	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "PalyButA", MainPlayButtonA);
	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "PlayButB", MainPlayButtonB);
	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "PlayButPTC", MainPlayParticle);
	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "PlayButNEXT", MainPlayNext);

	auto SecWinButtonA = ImGuiProAnim::CreateAnimButton();

	SecWinButtonA->ConfigNormalSize = Vector2T<float>(84.0f, 84.0f);
	SecWinButtonA->ConfigHoverSize = Vector2T<float>(88.0f, 88.0f);
	SecWinButtonA->ConfigActiveSize = Vector2T<float>(80.0f, 80.0f);

	SecWinButtonA->ConfigNormalColor = Vector4T<float>(0.55f, 0.0f, 1.0f, 0.2f);
	SecWinButtonA->ConfigHoverColor = Vector4T<float>(0.55f, 0.0f, 1.0f, 0.72f);
	SecWinButtonA->ConfigActiveColor = Vector4T<float>(0.55f, 0.0f, 1.0f, 0.72f);

	// øΩ±¥≈‰÷√.
	auto SecWinButtonB = ImGuiProAnim::CreateAnimButton(SecWinButtonA);
	auto SecWinButtonC = ImGuiProAnim::CreateAnimButton(SecWinButtonA);

	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "SecWinButA", SecWinButtonA);
	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "SecWinButB", SecWinButtonB);
	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "SecWinButC", SecWinButtonC);

	auto WindowSettings = ImGuiProAnim::CreateAnimFixedWindow();

	WindowSettings->ConfigCloseSize = Vector2T<float>(8.0f, 8.0f);
	WindowSettings->ConfigOpenSize = Vector2T<float>(256.0f, 288.0f);

	WindowSettings->ConfigCloseColor = Vector4T<float>(0.0f, 0.0f, 0.0f, 0.32f);
	WindowSettings->ConfigOpenColor = Vector4T<float>(0.24f, 0.0f, 0.5f, 0.98f);

	auto WindowArch = ImGuiProAnim::CreateAnimFixedWindow(WindowSettings);
	WindowArch->ConfigOpenSize = Vector2T<float>(256.0f, 280.0f);

	auto WindowInfo = ImGuiProAnim::CreateAnimFixedWindow(WindowSettings);
	WindowInfo->ConfigOpenSize = Vector2T<float>(256.0f, 390.0f);

	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "WindowA", WindowSettings);
	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "WindowB", WindowArch);
	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "WindowC", WindowInfo);

	// -------------------------------- ImGuiProAnimNodes --------------------------------

	auto AnimNodesArch = ImGuiProAnimNodes::CreateAnimNodes(ImGuiProAnimNodes::NodesVector, nullptr, Vector3T<float>(0.58f, 1.0f, 0.32f));
	auto TempSize = Vector2T<float>(700.0f, 700.0f);

	AnimNodesArch->ConfigNodeAdd("T1", ImGuiProAnimNodes::ConfigAnimNode(true, Vector2T<float>(536.0f, -200.0f), TempSize, Vector4T<float>()));
	AnimNodesArch->ConfigNodeAdd("T2", ImGuiProAnimNodes::ConfigAnimNode(true, Vector2T<float>(536.0f, 200.0f), TempSize, Vector4T<float>(1.0f, 1.0f, 1.0f, 1.0f)));
	AnimNodesArch->ConfigNodeAdd("T3", ImGuiProAnimNodes::ConfigAnimNode(true, Vector2T<float>(536.0f, -200.0f), TempSize, Vector4T<float>()));

	ImGuiProAnimNodes::AnimNodesCompReg(FrmDat.DataAnimationNodes, "NodesArch", AnimNodesArch);

	auto AnimNodesIcon1 = ImGuiProAnimNodes::CreateAnimNodes(ImGuiProAnimNodes::NodesVector, nullptr, Vector3T<float>(0.58f, 0.42f, 0.28f));
	TempSize = Vector2T<float>(72.0f, 72.0f);

	AnimNodesIcon1->ConfigNodeAdd("TI1", ImGuiProAnimNodes::ConfigAnimNode(true, Vector2T<float>(0.0f, -200.0f), TempSize, Vector4T<float>()));
	AnimNodesIcon1->ConfigNodeAdd("TI2", ImGuiProAnimNodes::ConfigAnimNode(true, Vector2T<float>(0.0f, -200.0f), TempSize, Vector4T<float>()));
	AnimNodesIcon1->ConfigNodeAdd("TI3", ImGuiProAnimNodes::ConfigAnimNode(true, Vector2T<float>(238.0f, 400.0f), Vector2T<float>(320.0f, 320.0f), Vector4T<float>(1.0f, 1.0f, 1.0f, 1.0f)));
	AnimNodesIcon1->ConfigNodeAdd("TI4", ImGuiProAnimNodes::ConfigAnimNode(true, Vector2T<float>(0.0f, -200.0f), TempSize, Vector4T<float>()));

	// øΩ±¥≈‰÷√ + æ÷≤ø–ﬁ∏ƒ.
	auto AnimNodesIcon2 = ImGuiProAnimNodes::CreateAnimNodes(ImGuiProAnimNodes::NodesVector, AnimNodesIcon1, Vector3T<float>(0.58f, 0.42f, 0.32f));
	AnimNodesIcon2->ConfigNodeFind("2")->TransPosition = Vector2T<float>(566.0f, 400.0f);

	auto AnimNodesIcon3 = ImGuiProAnimNodes::CreateAnimNodes(ImGuiProAnimNodes::NodesVector, AnimNodesIcon1, Vector3T<float>(0.58f, 0.42f, 0.32f));
	AnimNodesIcon3->ConfigNodeFind("2")->TransPosition = Vector2T<float>(894.0f, 400.0f);

	auto AnimNodesIcon4 = ImGuiProAnimNodes::CreateAnimNodes(ImGuiProAnimNodes::NodesVector, AnimNodesIcon1, Vector3T<float>(0.58f, 0.42f, 0.32f));
	AnimNodesIcon4->ConfigNodeFind("2")->TransPosition = Vector2T<float>(1222.0f, 400.0f);

	ImGuiProAnimNodes::AnimNodesCompReg(FrmDat.DataAnimationNodes, "NodesIcon1", AnimNodesIcon1);
	ImGuiProAnimNodes::AnimNodesCompReg(FrmDat.DataAnimationNodes, "NodesIcon2", AnimNodesIcon2);
	ImGuiProAnimNodes::AnimNodesCompReg(FrmDat.DataAnimationNodes, "NodesIcon3", AnimNodesIcon3);
	ImGuiProAnimNodes::AnimNodesCompReg(FrmDat.DataAnimationNodes, "NodesIcon4", AnimNodesIcon4);

	auto AnimNodesNext = ImGuiProAnimNodes::CreateAnimNodes(ImGuiProAnimNodes::NodesVector, nullptr, Vector3T<float>(0.58f, 1.0f, 0.32f));
	auto TempPos = Vector2T<float>(618.0f, 945.0f);
	TempSize = Vector2T<float>(512.0f, 116.0f);

	AnimNodesNext->ConfigNodeAdd("TX1", ImGuiProAnimNodes::ConfigAnimNode(true, TempPos, TempSize, Vector4T<float>()));
	AnimNodesNext->ConfigNodeAdd("TX2", ImGuiProAnimNodes::ConfigAnimNode(true, TempPos, TempSize, Vector4T<float>()));
	AnimNodesNext->ConfigNodeAdd("TX3", ImGuiProAnimNodes::ConfigAnimNode(true, TempPos, TempSize, Vector4T<float>()));
	AnimNodesNext->ConfigNodeAdd("TX4", ImGuiProAnimNodes::ConfigAnimNode(true, Vector2T<float>(618.0f, 512.0f), TempSize, Vector4T<float>(1.0f, 1.0f, 1.0f, 1.0f)));

	ImGuiProAnimNodes::AnimNodesCompReg(FrmDat.DataAnimationNodes, "NodesNext", AnimNodesNext);
}