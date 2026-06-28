#include "includes.h"
#include "include/GameHooks.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
bool showMenu = true;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io.IniFilename = nullptr;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

static void RenderHookMenu()
{
	if (!ImGui::Begin("Solo Leveling Hook", &showMenu, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("Reset All Features"))
	{
		GameHooks::ResetFeatures();
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Combat Hacks", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("God Mode", &GameHooks::g_features.godMode);
		ImGui::Checkbox("One Hit Kill", &GameHooks::g_features.oneHitKill);
		ImGui::Checkbox("Infinite Damage", &GameHooks::g_features.infiniteDamage);
	ImGui::SliderInt("Damage Value", &GameHooks::g_features.damageValue, 1, 999999999);
	ImGui::Checkbox("Super Crit", &GameHooks::g_features.superCrit);
	ImGui::SliderInt("Crit Rate (%)", &GameHooks::g_features.critRateValue, 0, 100);
	ImGui::Checkbox("Instant Skill", &GameHooks::g_features.instantSkill);
}

if (ImGui::CollapsingHeader("Cooldown / Speed", ImGuiTreeNodeFlags_DefaultOpen))
{
	ImGui::Checkbox("No Cooldown", &GameHooks::g_features.noCooldown);
	ImGui::Checkbox("Max Speed", &GameHooks::g_features.maxSpeed);
	ImGui::SliderFloat("Speed Multiplier", &GameHooks::g_features.speedMultiplier, 1.0f, 50.0f, "%.1f");
}

if (ImGui::CollapsingHeader("Stats", ImGuiTreeNodeFlags_DefaultOpen))
{
	ImGui::Checkbox("Infinite HP", &GameHooks::g_features.infiniteHP);
	ImGui::SliderFloat("Max HP Value", &GameHooks::g_features.maxHPValue, 1.0f, 9999999.0f, "%.0f");
	ImGui::Checkbox("Infinite MP", &GameHooks::g_features.infiniteMP);
	ImGui::SliderFloat("Max MP Value", &GameHooks::g_features.maxMPValue, 1.0f, 9999999.0f, "%.0f");
	ImGui::Checkbox("Max Attack", &GameHooks::g_features.maxAttack);
	ImGui::SliderInt("Max Attack Value", &GameHooks::g_features.maxAttackValue, 1, 999999);
	ImGui::Checkbox("Max Defense", &GameHooks::g_features.maxDefense);
	ImGui::SliderInt("Max Defense Value", &GameHooks::g_features.maxDefenseValue, 1, 999999);
		ImGui::Checkbox("Unlock All Hunters", &GameHooks::g_features.unlockAllHunters);
		ImGui::Checkbox("Max Shadow Army", &GameHooks::g_features.maxShadowArmy);
	}

	if (ImGui::CollapsingHeader("Currency / Items", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Infinite Gold", &GameHooks::g_features.infiniteGold);
		ImGui::Checkbox("Infinite Gems", &GameHooks::g_features.infiniteGems);
		ImGui::Checkbox("Infinite AP", &GameHooks::g_features.infiniteAP);
	}

	if (ImGui::CollapsingHeader("Misc", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Bypass Anti Cheat", &GameHooks::g_features.bypassAntiCheat);
		ImGui::Checkbox("Unlock All Content", &GameHooks::g_features.unlockAllContent);
		ImGui::Checkbox("Instant Quest Complete", &GameHooks::g_features.instantQuestComplete);
	}

	ImGui::TextWrapped("Press INSERT to toggle the menu.");
	ImGui::End();
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_KEYDOWN && wParam == VK_INSERT)
	{
		showMenu = !showMenu;
		return true;
	}

	if (showMenu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			HRESULT hr = pSwapChain->GetDesc(&sd);
			if (FAILED(hr))
			{
				pContext->Release();
				pContext = NULL;
				pDevice->Release();
				pDevice = NULL;
				return oPresent(pSwapChain, SyncInterval, Flags);
			}
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer = NULL;
			hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			if (FAILED(hr) || !pBackBuffer)
			{
				pContext->Release();
				pContext = NULL;
				pDevice->Release();
				pDevice = NULL;
				return oPresent(pSwapChain, SyncInterval, Flags);
			}
			hr = pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			if (FAILED(hr))
			{
				pContext->Release();
				pContext = NULL;
				pDevice->Release();
				pDevice = NULL;
				return oPresent(pSwapChain, SyncInterval, Flags);
			}
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}
		else
		{
			return oPresent(pSwapChain, SyncInterval, Flags);
		}
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (showMenu)
	{
		RenderHookMenu();
	}

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

static constexpr int MAX_INIT_RETRIES = 50;
static constexpr DWORD RETRY_DELAY_MS = 200;

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	int retries = 0;
	do
	{
		kiero::Status::Enum status = kiero::init(kiero::RenderType::D3D11);
		if (status == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
		else if (status == kiero::Status::AlreadyInitializedError)
		{
			init_hook = true;
		}
		else
		{
			retries++;
			if (retries >= MAX_INIT_RETRIES)
			{
				return FALSE;
			}
			Sleep(RETRY_DELAY_MS);
		}
	} while (!init_hook);

	if (!GameHooks::Initialize())
	{
		kiero::shutdown();
		return FALSE;
	}
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hMod);
		HANDLE hThread = CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		if (!hThread)
			return FALSE;
		CloseHandle(hThread);
		break;
	}
	case DLL_PROCESS_DETACH:
		GameHooks::Cleanup();
		kiero::shutdown();
		break;
	}
	return TRUE;
}