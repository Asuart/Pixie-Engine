#include "pch.h"
#include "ShaderNode.h"
#include "ShaderNodeConnection.h"

ShaderNode::ShaderNode(const std::string& name) :
	m_name(name) {};

const std::string& ShaderNode::GetName() const {
	return m_name;
}

std::vector<ShaderNodeInput>& ShaderNode::GetInputs() {
	return m_inputs;
}

std::vector<ShaderNodeOutput>& ShaderNode::GetOutputs() {
	return m_outputs;
}

ShaderNodeInput* ShaderNode::GetInput(const std::string& name) {
	for (uint32_t i = 0; i < m_inputs.size(); i++) {
		if (m_inputs[i].m_name == name) {
			return &m_inputs[i];
		}
	}
	return nullptr;
}

int32_t ShaderNode::GetInputInt(const std::string& name) {
	ShaderNodeInput* input = GetInput(name);
	if (!input || !input->m_connection) {
		return 0;
	}
	return *((int32_t*)input->m_connection->from.m_value);
}

Float ShaderNode::GetInputFloat(const std::string& name) {
	ShaderNodeInput* input = GetInput(name);
	if (!input || !input->m_connection) {
		return 0.0f;
	}
	return *((Float*)input->m_connection->from.m_value);
}

Vec2 ShaderNode::GetInputVec2(const std::string& name) {
	ShaderNodeInput* input = GetInput(name);
	if (!input || !input->m_connection) {
		return Vec2();
	}
	return *((Vec2*)input->m_connection->from.m_value);
}

Vec3 ShaderNode::GetInputVec3(const std::string& name) {
	ShaderNodeInput* input = GetInput(name);
	if (!input || !input->m_connection) {
		return Vec3();
	}
	return *((Vec3*)input->m_connection->from.m_value);
}

Vec4 ShaderNode::GetInputVec4(const std::string& name) {
	ShaderNodeInput* input = GetInput(name);
	if (!input || !input->m_connection) {
		return Vec4();
	}
	return *((Vec4*)input->m_connection->from.m_value);
}

GLuint ShaderNode::GetInputTexture(const std::string& name) {
	ShaderNodeInput* input = GetInput(name);
	if (!input || !input->m_connection) {
		return 0;
	}
	return *((GLuint*)input->m_connection->from.m_value);
}

ShaderNodeOutput* ShaderNode::GetOutput(const std::string& name) {
	for (uint32_t i = 0; i < m_outputs.size(); i++) {
		if (m_outputs[i].m_name == name) {
			return &m_outputs[i];
		}
	}
	return nullptr;
}

int32_t ShaderNode::GetOutputInt(const std::string& name) {
	ShaderNodeOutput* output = GetOutput(name);
	if (!output) {
		return 0;
	}
	return *((int32_t*)output->m_value);
}

Float ShaderNode::GetOutputFloat(const std::string& name) {
	ShaderNodeOutput* output = GetOutput(name);
	if (!output) {
		return 0.0f;
	}
	return *((Float*)output->m_value);
}

Vec2 ShaderNode::GetOutputVec2(const std::string& name) {
	ShaderNodeOutput* output = GetOutput(name);
	if (!output) {
		return Vec2();
	}
	return *((Vec2*)output->m_value);
}

Vec3 ShaderNode::GetOutputVec3(const std::string& name) {
	ShaderNodeOutput* output = GetOutput(name);
	if (!output) {
		return Vec3();
	}
	return *((Vec3*)output->m_value);
}

Vec4 ShaderNode::GetOutputVec4(const std::string& name) {
	ShaderNodeOutput* output = GetOutput(name);
	if (!output) {
		return Vec4();
	}
	return *((Vec4*)output->m_value);
}

GLuint ShaderNode::GetOutputTexture(const std::string& name) {
	ShaderNodeOutput* output = GetOutput(name);
	if (!output) {
		return 0;
	}
	return *((GLuint*)output->m_value);
}
