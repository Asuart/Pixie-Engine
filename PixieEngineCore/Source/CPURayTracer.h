#pragma once
#include "Film.h"
#include "RTScene.h"
#include "Light.h"
#include "VisibleSurface.h"

enum class CPURayTracerMode {
	Normals = 0,
	TraceRay,
	TracePath,
	LiRandomWalk,
	LiSimplePath,
	LiPath,
	_COUNT_,
};

std::string to_string(CPURayTracerMode mode);

class CPURayTracer {
public:
	CPURayTracerMode mode = CPURayTracerMode::TraceRay;
	int32_t maxDepth = 128;
	glm::ivec2 resolution;
	const glm::ivec2 tileSize = glm::ivec2(64, 64);
	int32_t maxThreads = 1;
	bool sampleLights = true;
	bool sampleBSDF = true;
	bool regularize = true;

	Film film;
	RTScene* scene = nullptr;
	UniformLightSampler* lightSampler = nullptr;

	bool isRendering = false;
	std::vector<std::thread*> renderThreads;
	std::vector<Bounds2i> quads;
	std::queue<int32_t> quadQueue;
	std::mutex quadQueueMutex;

	int32_t sample = 0;
	int32_t threads = 0;
	Float renderTime = 0;
	Float lastSampleTime = 0;
	std::chrono::microseconds startTime;
	std::chrono::microseconds sampleStartTime;

	CPURayTracer(glm::ivec2 resolution, int32_t _maxDepth = 1024);
	~CPURayTracer();

	void Resize(glm::ivec2 resolution);
	void Reset();
	void StartRender();
	void EndRender();
	void SetScene(RTScene* s);

private:
	bool Unoccluded(const RTInteraction& p0, const RTInteraction& p1) const;
	void GenerateQuads();
	void PerPixel(uint32_t x, uint32_t y);
	Vec3 SampleLd(const RTInteraction& intr, const BSDF* bsdf) const;
	Vec3 TraceNormals(Ray ray) const;
	Vec3 TraceRay(Ray ray) const;
	Vec3 TracePath(Ray ray) const;
	Vec3 LiRandomWalk(Ray ray, int depth = 0) const;
	Vec3 LiSimplePath(Ray ray) const;
	Vec3 LiPath(Ray ray, VisibleSurface* visibleSurf) const;
};
