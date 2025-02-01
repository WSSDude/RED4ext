#include "Funcs.hpp"
#include "App.hpp"
#include "Utils.hpp"

namespace
{
std::shared_ptr<PluginBase> GetPluginByHandle(RED4ext::PluginHandle aHandle)
{
    auto app = App::Get();
    if (!app)
    {
        return nullptr;
    }

    auto pluginSystem = app->GetPluginSystem();
    auto plugin = pluginSystem->GetPlugin(aHandle);
    if (!plugin)
    {
        spdlog::warn("Could not find a plugin with handle {}", fmt::ptr(aHandle));
        return nullptr;
    }

    return plugin;
}
} // namespace

namespace v1 {
bool Hooking::Attach(RED4ext::PluginHandle aHandle, void* aTarget, void* aDetour, void** aOriginal)
{
    spdlog::trace("Attach request received from plugin with handle {}", fmt::ptr(aHandle));

    if (aTarget == nullptr || aDetour == nullptr)
    {
        spdlog::warn("One of the required parameters for attaching hook is NULL");
        return false;
    }

    auto app = App::Get();
    if (!app)
    {
        return false;
    }

    auto plugin = GetPluginByHandle(aHandle);
    if (!plugin)
    {
        return false;
    }

    auto hookingSystem = app->GetHookingSystem();
    return hookingSystem->Attach(plugin, aTarget, aDetour, aOriginal);
}

bool Hooking::Detach(RED4ext::PluginHandle aHandle, void* aTarget)
{
    spdlog::trace("Detach request received from plugin with handle {}", fmt::ptr(aHandle));

    if (aTarget == nullptr)
    {
        spdlog::warn("One of the required parameters for detaching hook is NULL");
        return false;
    }

    auto app = App::Get();
    if (!app)
    {
        return false;
    }

    auto plugin = GetPluginByHandle(aHandle);
    if (!plugin)
    {
        return false;
    }

    auto hookingSystem = app->GetHookingSystem();
    return hookingSystem->Detach(plugin, aTarget);
}

bool GameStates::AddHook(RED4ext::PluginHandle aHandle, RED4ext::EGameStateType aType, RED4ext::GameState* aState)
{
    spdlog::trace("Request to add a game state hooke has been received from plugin with handle {}", fmt::ptr(aHandle));

    if (!aState)
    {
        spdlog::warn("The state's address is NULL");
        return false;
    }

    auto app = App::Get();
    if (!app)
    {
        return false;
    }

    auto plugin = GetPluginByHandle(aHandle);
    if (!plugin)
    {
        return false;
    }

    auto stateSystem = app->GetStateSystem();
    return stateSystem->AddHook(plugin, aType, aState);
}

bool Scripts::Add(RED4ext::PluginHandle aHandle, const wchar_t* aPath)
{
    auto app = App::Get();
    if (!app)
    {
        return false;
    }

    auto plugin = GetPluginByHandle(aHandle);
    if (!plugin)
    {
        return false;
    }

    auto scriptCompilationSystem = app->GetScriptCompilationSystem();
    return scriptCompilationSystem->Add(plugin, aPath);
}
} // namespace v1
