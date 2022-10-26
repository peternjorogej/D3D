#pragma once

#include "Base.h"
#include "Bindable.h"
#include "Drawable.h"

class PointLight
{
public:
	PointLight(float Radius = 0.5f);

	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Draw() const noexcept;
	void Bind() const noexcept;

private:
	struct PointLightConstantBuffer
	{
		alignas(16) Float3 Position      = {};
		alignas(16) Float3 MaterialColor = {};
		alignas(16) Float3 AmbientColor  = {};
		alignas(16) Float3 DiffuseColor  = {};
		float  DiffuseIntensity     = 0.0f;
		float  AttenuationConstant  = 0.0f;
		float  AttenuationLinear    = 0.0f;
		float  AttenuationQuadratic = 0.0f;
	};

private:
	PointLightConstantBuffer m_Buffer = {};
	mutable SolidSphere      m_Mesh;
	mutable PixelConstantBuffer<PointLightConstantBuffer> m_CB = {};
};