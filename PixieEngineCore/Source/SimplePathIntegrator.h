#pragma once
#include "pch.h"
#include "Integrator.h"
#include "Random.h"

class SimplePathIntegrator : public Integrator {
public:
	bool m_sampleLights = true;
	bool m_sampleBSDF = true;

	SimplePathIntegrator(const glm::ivec2& resolution);

	virtual void SetScene(Scene* scene) override;
	virtual Spectrum Integrate(Ray ray, Sampler* sampler) override;

protected:
	UniformLightSampler m_lightSampler;
};