#include "pch.h"
#include "GameManager.h"
#include "SystemManager.h"
#include "ComponentManager.h"

#include "Core/Window.h"
#include "Core/Transform.h"
#include "Render/RenderEngine.h"
#include "InputManager.h"

GameManager* GameManager::m_instance = nullptr;

GameManager* GameManager::GInstance()
{
	return (m_instance == nullptr) ? m_instance = new GameManager() : m_instance;
}

Vector2i GameManager::GMousePosition()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(m_window->GHWND(), &p);
	return Vector2i(p.x, p.y);
}

Vector3f GameManager::GMouseWorldPosition()
{
	return m_renderer->ScreenToWorld(GMousePosition());
}
void GameManager::SMousePosition(Vector2i _newPos)
{
	POINT p = { _newPos.x, _newPos.y };
	ClientToScreen(m_window->GHWND(), &p);
	SetCursorPos(p.x, p.y);
}
GameManager::GameManager()
{
	bool r;
#if defined(DEBUG) | defined(_DEBUG)
	r = AllocConsole();
	FILE* pfile = nullptr;
	freopen_s(&pfile, "CONOUT$", "w", stdout);
	freopen_s(&pfile, "CONOUT$", "w", stderr);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	bool r2 = GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
	Log(!r && !r2, "Console Initializing");
#endif
	m_window = new Window;
	r = m_window->Init(1980, 1080, L"ENETRE");
	Log(r, "Initializing Window");
	m_renderer = new RenderEngine();
	r = m_renderer->Init(m_window->GCWidth(), m_window->GCHeight(), m_window->GHWND());
	Log(r, "Initializing RenderEngine");

	m_systemManager = new SystemManager(this);
	m_componentManager = new ComponentManager(m_systemManager);
	m_inputManager = InputManager::GInstance();
	m_inputManager->Init(m_window);
	m_systemManager->Init();
}
GameManager::~GameManager()
{
#if defined(DEBUG) | defined(_DEBUG)
	fclose(stdout);
	fclose(stderr);
	FreeConsole();
#endif
}

void GameManager::Run()
{
	MSG msg = { 0 };
	m_lastTime = std::chrono::steady_clock::now();
	float rotation = 0;	

	while (m_splashTimer >0)
	{
		Texture* splashTexture = m_renderer->LoadTexture(L"../../res/Render/splash.dds");
		m_renderer->BeginDraw();
		m_renderer->BeginDrawUI();
		m_renderer->DrawUI(splashTexture, { 0,0 }, { (float)m_window->GCWidth(), (float)m_window->GCHeight() }, { 1,1,1,1 });
		m_renderer->CloseDraw();
		auto currentTime = std::chrono::steady_clock::now();
		std::chrono::duration<float> deltaTime = currentTime - m_lastTime;
		m_deltaTime = deltaTime.count();
		m_lastTime = currentTime;
		m_splashTimer -= deltaTime.count();
	}	

	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			auto currentTime = std::chrono::steady_clock::now();
			std::chrono::duration<float> deltaTime = currentTime - m_lastTime;
			m_deltaTime = deltaTime.count();
			m_lastTime = currentTime;
			if (m_timer > 0)
			{
				m_timer -= deltaTime.count();
				m_fps += 1;
			}
			else
			{
				WString fpsString = L"ENETRE | FPS:" + std::to_wstring((int)m_fps);
				// 3. Mettre � jour le titre de la fen�tre
				SetWindowText(m_window->GHWND(), fpsString.c_str());
				m_timer = 1;
				m_fps = 0;
			}

			m_entityIds;
			int y = 0;
			m_renderer->Update(deltaTime.count());
			m_systemManager->Update();
			m_inputManager->Update();

		}
		m_time += m_deltaTime;
	}
}
uint32 GameManager::GNewEntityId()
{
	uint32 newId = 0;
	size_t position = -1;

	for (int i = 0; i < m_entityIds.size(); i++)
	{
		if (m_entityIds[i] == MAX_VALUE)
		{
			position = i;
			break;
		}
		if (newId < m_entityIds[i])
			newId = m_entityIds[i];
	}
	newId++;
	for (int i = 0; i < m_freeEntityIds.size(); i++)
		if (m_freeEntityIds[i] != MAX_VALUE)
		{
			newId = m_freeEntityIds[i];
			m_freeEntityIds[i] = MAX_VALUE;
			break;
		}

	if (position == -1)
		m_entityIds.push_back(newId);
	else
		m_entityIds[position] = newId;

	return newId;
}

void GameManager::DestroyEntity(uint32 _entityId)
{
	for (size_t i = 0; i < m_entityIds.size(); i++)
		if (m_entityIds[i] == _entityId)
		{
			m_entityIds[i] = MAX_VALUE;
			int position = -1;
			for (size_t i = 0; i < m_freeEntityIds.size(); i++)
				if (m_freeEntityIds[i] == MAX_VALUE)
				{
					position = i;
					break;
				}
			if (position == -1)
				m_freeEntityIds.push_back(_entityId);
			else
				m_freeEntityIds[position] = _entityId;

			m_componentManager->RemoveEntity(_entityId);
			break;
		}
}
