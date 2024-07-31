#pragma once
#include "pch.h"

struct FrameBuffer {
	FrameBuffer();
	~FrameBuffer();

	void Resize(uint32_t width, uint32_t height);
	void Bind();
	void Unbind();

	GLuint m_frameBuffer;
	GLuint m_texture, m_depth;
	glm::ivec2 m_resolution;
};
