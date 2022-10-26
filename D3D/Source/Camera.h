#pragma once

#include "Base.h"

class SceneCamera
{
public:
	SceneCamera() = default;

	Matrix4x4 GetMatrix() const noexcept;
	void      SpawnControlWindow() noexcept;
	void      Reset() noexcept;

private:
	float m_Radius = 70.0f;
	float m_Theta  = 0.0f;
	float m_Phi    = 0.0f;
	float m_Pitch  = 0.0f;
	float m_Yaw    = 0.0f;
	float m_Roll   = 0.0f;
};