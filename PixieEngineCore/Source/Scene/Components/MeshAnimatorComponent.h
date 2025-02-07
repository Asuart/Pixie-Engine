#pragma once
#include "pch.h"
#include "Component.h"
#include "Animation/MeshAnimator.h"
#include "OpenGLInterface.h"
#include "EngineTime.h"

class MeshAnimatorComponent : public Component {
public:
	MeshAnimatorComponent(SceneObject* parent, const std::vector<Animation*>& animations, Mat4 globalInverseTransform);
	~MeshAnimatorComponent();

	void OnUpdate() override;
	void UpdateAnimation(Float deltaTime);
	std::vector<Mat4> GetBoneMatrices();
	void GetBoneMatrices(Float time, std::array<Mat4, MaxBonesPerModel>& transforms) const;

protected:
	Animator* m_animator;
	std::vector<Animation*> m_animations;
	int32_t m_currentAnimation = -1;
};
