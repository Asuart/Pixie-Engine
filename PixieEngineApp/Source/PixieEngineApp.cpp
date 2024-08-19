#include "PixieEngineApp.h"

PixieEngineApp::PixieEngineApp() {
	if (!gladLoadGL()) {
		std::cout << "GLEW initialization failed.\n";
		exit(1);
	}

	glEnable(GL_DEPTH_TEST);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_window.GetGLFWWindow(), true);
	ImGui_ImplOpenGL3_Init();

	SceneLoader sceneLoader;
	m_scene = sceneLoader.LoadScene(m_scenePath);
	m_scene->MakeGeometrySnapshot();

	m_sceneRenderer = new SceneRenderer(glm::ivec2(1280, 720), m_scene);

	m_rayTracingRenderer = new RayTracingRenderer(this, glm::ivec2(1280, 720), m_scene);
	if (m_rayTracingViewport) {
		m_rayTracingRenderer->StartRender();
	}

	m_viewportFrameBuffer = new FrameBuffer(1280, 720);
}

PixieEngineApp::~PixieEngineApp() {
	delete m_rayTracingRenderer;
	delete m_viewportFrameBuffer;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void PixieEngineApp::Start() {
	while (!m_window.IsShouldClose()) {
		glfwPollEvents();
		HandleUserInput();
		UserInput::Reset();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_viewportFrameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_viewportFrameBuffer->m_resolution.x, m_viewportFrameBuffer->m_resolution.y);
		if (m_rayTracingViewport) {
			m_rayTracingRenderer->DrawFrame();
		}
		else {
			m_sceneRenderer->DrawFrame();
		}
		m_viewportFrameBuffer->Unbind();
		glViewport(0, 0, m_window.GetWindowSize().x, m_window.GetWindowSize().y);

		DrawUI();

		glfwSwapBuffers(m_window.GetGLFWWindow());
	}

	m_rayTracingRenderer->StopRender();
}

GLFWwindow* PixieEngineApp::GetGLFWWindow() {
	return m_window.GetGLFWWindow();
}

void PixieEngineApp::HandleResize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

void PixieEngineApp::DrawUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static bool dockingOpen = true;
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) {
		windowFlags |= ImGuiWindowFlags_NoBackground;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockingOpen, windowFlags);

	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Close")) glfwSetWindowShouldClose(m_window.GetGLFWWindow(), true);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	//ImGui::ShowDemoWindow();
	DrawSettingsWindow();
	DrawViewportWindow();
	DrawSceneWindow();
	DrawMaterialsWindow();
	DrawInspectorWindow();

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void PixieEngineApp::DrawSettingsWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Settings", 0);

	ImGui::PushItemWidth(-FLT_MIN);

	if (ImGui::Checkbox("RayTracing viewport", &m_rayTracingViewport)) {
		if (m_rayTracingViewport) {
			m_rayTracingRenderer->Reset();
			m_rayTracingRenderer->StartRender();
		}
		else {
			m_rayTracingRenderer->StopRender();
		}
	}

	ImGui::Text("Scene Path");
	ImGui::InputText("##scene_path", m_scenePath, m_maxScenePathLength);
	if (ImGui::Button("Reload Scene")) {
		ReloadScene();
	}
	ImGui::Spacing();

	m_rayTracingRenderer->DrawUI();

	ImGui::PopItemWidth();

	ImGui::End();
}

void PixieEngineApp::DrawViewportWindow() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport");
	ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
	ImGui::SetNextWindowSize(viewportResolution);
	ImGui::Image((void*)(uint64_t)m_viewportFrameBuffer->m_texture, viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
	glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };
	if (glmViewportResolution != m_viewportResolution) {
		UpdateViewportResolution(glmViewportResolution);
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void PixieEngineApp::DrawSceneWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Scene", 0);
	DrawSceneTree(m_scene->GetRootObject());
	ImGui::End();
}

void PixieEngineApp::DrawSceneTree(SceneObject* object) {
	ImGuiTreeNodeFlags flags = 0;
	if (object == m_scene->GetRootObject()) flags |= ImGuiTreeNodeFlags_DefaultOpen;
	if (object->children.size() == 0) flags |= ImGuiTreeNodeFlags_Leaf;
	if (object == m_selectedObject) flags |= ImGuiTreeNodeFlags_Selected;
	if (ImGui::TreeNodeEx(object->name.c_str(), flags)) {
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
			m_selectedObject = object;
		}
		for (size_t i = 0; i < object->children.size(); i++) {
			DrawSceneTree(object->children[i]);
		}
		ImGui::TreePop();
	}
}

void PixieEngineApp::DrawMaterialsWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Materials", 0);

	std::vector<Material*> materials = m_scene->GetMaterialsList();
	for (Material* material : materials) {
		if (ImGui::CollapsingHeader(material->m_name.c_str())) {
			if (ImGui::ColorEdit3("Albedo", (float*)&material->m_albedo.GetRGB())) {
				m_rayTracingRenderer->Reset();
			}
			if (ImGui::ColorEdit3("Emission Color", (float*)&material->m_emissionColor.GetRGB())) {
				m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragFloat("Emission Strength", (float*)&material->m_emissionStrength)) {
				m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragFloat("Roughness", &material->m_roughness, 0.01f, 0.0f, 1.0f)) {
				m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragFloat("Metallic", &material->m_metallic, 0.01f, 0.0f, 1.0f)) {
				m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragFloat("Transparency", &material->m_transparency, 0.01f, 0.0f, 1.0f)) {
				m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragFloat("Refraction", &material->m_refraction, 0.01f, 0.0f, 10.0f)) {
				m_rayTracingRenderer->Reset();
			}
		}
	}

	ImGui::End();
}

void PixieEngineApp::DrawInspectorWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Inspector", 0);

	if (Camera* mainCamera = m_scene->GetMainCamera()) {
		ImGui::Text("Main Camera");
		ImGui::Spacing();
		DrawTransform(mainCamera->m_transform);
	}

	if (m_selectedObject) {
		ImGui::Text(m_selectedObject->name.c_str());
		ImGui::Spacing();

		DrawTransform(m_selectedObject->transform);
		ImGui::Spacing();

		ImGui::Text("Components");
		if (m_selectedObject->components.size() > 0) {
			for (size_t i = 0; i < m_selectedObject->components.size(); i++) {
				ImGui::Text(m_selectedObject->components[i]->name.c_str());
			}
		}
		ImGui::Spacing();
	}

	ImGui::End();
}

void PixieEngineApp::DrawTransform(Transform& transform) {
	ImGui::Text("Transform");
	if (ImGui::DragFloat3(("Position##" + transform.id.ToString()).c_str(), (float*)&transform.GetPosition(), 0.01f, -10000.0f, 10000.0f)) {
		transform.UpdateMatrices();
	}
	if (ImGui::DragFloat3(("Rotation##" + transform.id.ToString()).c_str(), (float*)&transform.GetRotation(), 0.1f, -720.0f, 720.0f)) {
		transform.UpdateMatrices();
	}
	if (ImGui::DragFloat3(("Scale##" + transform.id.ToString()).c_str(), (float*)&transform.GetScale(), 0.01f, -10000.0f, 10000.0f)) {
		transform.UpdateMatrices();
	}
}

void PixieEngineApp::UpdateViewportResolution(glm::ivec2 resolution) {
	m_viewportResolution = resolution;
	m_sceneRenderer->SetResolution(resolution);
	m_rayTracingRenderer->SetViewportSize(resolution);
	m_viewportFrameBuffer->Resize(resolution.x, resolution.y);
}

void PixieEngineApp::ReloadScene() {
	m_rayTracingRenderer->StopRender();
	if (m_scene) {
		delete m_scene;
		m_scene = nullptr;
	}

	SceneLoader sceneLoader;
	m_scene = sceneLoader.LoadScene(m_scenePath);
	m_scene->MakeGeometrySnapshot();

	m_sceneRenderer->SetScene(m_scene);

	m_rayTracingRenderer->SetScene(m_scene);
	m_rayTracingRenderer->StartRender();
}

void PixieEngineApp::HandleUserInput() {
	const Float speed = 10.0f;
	const Float rotationSpeed = 0.1f;

	if (UserInput::GetKey(GLFW_KEY_W)) {
		if (m_scene) {
			Camera* camera = m_scene->GetMainCamera();
			if (camera) {
				camera->m_transform.MoveForward(speed * Timer::fixedDeltaTime);
			}
		}
		if (m_rayTracingViewport) m_rayTracingRenderer->Reset();
	}
	if (UserInput::GetKey(GLFW_KEY_S)) {
		if (m_scene) {
			Camera* camera = m_scene->GetMainCamera();
			if (camera) {
				camera->m_transform.MoveForward(-speed * Timer::fixedDeltaTime);
			}
		}
		if (m_rayTracingViewport)m_rayTracingRenderer->Reset();
	}
	if (UserInput::GetKey(GLFW_KEY_D)) {
		if (m_scene) {
			Camera* camera = m_scene->GetMainCamera();
			if (camera) {
				camera->m_transform.MoveRight(speed * Timer::fixedDeltaTime);
			}
		}
		if (m_rayTracingViewport)m_rayTracingRenderer->Reset();
	}
	if (UserInput::GetKey(GLFW_KEY_A)) {
		if (m_scene) {
			Camera* camera = m_scene->GetMainCamera();
			if (camera) {
				camera->m_transform.MoveRight(-speed * Timer::fixedDeltaTime);
			}
		}
		if (m_rayTracingViewport)m_rayTracingRenderer->Reset();
	}
	if (UserInput::GetKey(GLFW_KEY_SPACE)) {
		if (m_scene) {
			Camera* camera = m_scene->GetMainCamera();
			if (camera) {
				camera->m_transform.MoveUp(speed * Timer::fixedDeltaTime);
			}
		}
		if (m_rayTracingViewport)m_rayTracingRenderer->Reset();
	}
	if (UserInput::GetKey(GLFW_KEY_LEFT_CONTROL)) {
		if (m_scene) {
			Camera* camera = m_scene->GetMainCamera();
			if (camera) {
				camera->m_transform.MoveUp(-speed * Timer::fixedDeltaTime);
			}
		}
		if (m_rayTracingViewport)m_rayTracingRenderer->Reset();
	}
	if (UserInput::GetMouseButton(GLFW_MOUSE_BUTTON_2)) {
		if (UserInput::mouseDeltaX) {
			if (UserInput::mouseDeltaX) {
				if (m_scene) {
					Camera* camera = m_scene->GetMainCamera();
					if (camera) {
						camera->m_transform.AddRotationY(rotationSpeed * (Float)UserInput::mouseDeltaX);
					}
				}
			}
			if (m_rayTracingViewport)m_rayTracingRenderer->Reset();
		}
	}
}