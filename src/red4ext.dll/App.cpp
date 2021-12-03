#include "stdafx.hpp"
#include "App.hpp"
#include "DetourTransaction.hpp"
#include "Image.hpp"
#include "Utils.hpp"
#include "Version.hpp"

#include "Hooks/CGameApplication.hpp"
#include "Hooks/Main_Hooks.hpp"

namespace
{
std::unique_ptr<App> g_app;
}

App::App()
    : m_config(m_paths)
    , m_devConsole(m_config.GetDev())
    , m_pluginSystem(m_config.GetPlugins(), m_paths)
{
    Utils::CreateLogger(m_paths, m_config, m_devConsole);

    spdlog::info("RED4ext ({}) is initializing...", RED4EXT_GIT_TAG);

    spdlog::debug("Using the following paths:");
    spdlog::debug(L"  Root: {}", m_paths.GetRootDir());
    spdlog::debug(L"  RED4ext: {}", m_paths.GetRED4extDir());
    spdlog::debug(L"  Logs: {}", m_paths.GetLogsDir());
    spdlog::debug(L"  Config: {}", m_paths.GetConfigFile());
    spdlog::debug(L"  Plugins: {}", m_paths.GetPluginsDir());

    spdlog::debug("Using the following configuration:");
    spdlog::debug("  version: {}", m_config.GetVersion());

    const auto& dev = m_config.GetDev();
    spdlog::debug("  dev.console: {}", dev.hasConsole);

    const auto& loggingConfig = m_config.GetLogging();
    spdlog::debug("  logging.level: {}", spdlog::level::to_string_view(loggingConfig.level));
    spdlog::debug("  logging.flush_on: {}", spdlog::level::to_string_view(loggingConfig.flushOn));
    spdlog::debug("  logging.max_files: {}", loggingConfig.maxFiles);
    spdlog::debug("  logging.max_file_size: {} MB", loggingConfig.maxFileSize);

    const auto& pluginsConfig = m_config.GetPlugins();
    spdlog::debug("  plugins.enabled: {}", pluginsConfig.isEnabled);

    const auto& blacklist = pluginsConfig.blacklist;
    if (blacklist.empty())
    {
        spdlog::debug(L"  plugins.blacklist: []");
    }
    else
    {
        spdlog::debug(L"  plugins.blacklist: [ {} ]", fmt::join(blacklist, L", "));
    }

    spdlog::debug("Base address is {}", reinterpret_cast<void*>(GetModuleHandle(nullptr)));

    const auto image = Image::Get();
    const auto& ver = image->GetVersion();
    spdlog::info("Game version is {}.{}{}", ver.major, ver.minor, ver.patch);

    if (!image->IsSupported())
    {
        spdlog::error("This game version ({}.{}{}) is not supported.", ver.major, ver.minor, ver.patch);

        const auto supportedVer = image->GetSupportedVersion();
        fmt::memory_buffer out;
        fmt::format_to(std::back_inserter(out), "The current version of the mod supports only version {}.{}{}, ",
                       supportedVer.major, supportedVer.minor, supportedVer.patch);

        if (ver < supportedVer)
        {
            fmt::format_to(std::back_inserter(out), "try downgrading the mod or updating the game.");
        }
        else
        {
            fmt::format_to(std::back_inserter(out), "try updating the mod or downgrading the game.");
        }

        spdlog::error(fmt::to_string(out));
        return;
    }

    if (AttachHooks())
    {
        spdlog::info("RED4ext has been successfully initialized");
    }
    else
    {
        spdlog::error("RED4ext did not initialize properly");
    }
}

void App::Construct()
{
    g_app.reset(new App());
}

void App::Destruct()
{
    spdlog::info("RED4ext is terminating...");

    // Detaching hooks here and not in dtor, since the dtor can be called by CRT when the processes exists. We don't
    // really care if this will be called or not when the game exist ungracefully.
    const auto image = Image::Get();
    if (image->IsSupported())
    {
        spdlog::trace("Detaching the hooks...");

        DetourTransaction transaction;
        if (transaction.IsValid())
        {
            auto success = Hooks::CGameApplication::Detach() && Hooks::Main::Detach();
            if (success)
            {
                transaction.Commit();
            }
        }
    }

    g_app.reset(nullptr);
    spdlog::info("RED4ext has been terminated");

    spdlog::details::registry::instance().flush_all();
    spdlog::shutdown();
}

App* App::Get()
{
    return g_app.get();
}

void App::Startup()
{
    spdlog::info("RED4ext is starting up...");

    m_pluginSystem.Startup();

    spdlog::info("RED4ext has been started");
}

void App::Shutdown()
{
    spdlog::info("RED4ext is shutting down...");

    m_pluginSystem.Shutdown();
    m_stateSystem.Shutdown();
    m_hookingSystem.Shutdown();

    spdlog::info("RED4ext has been shut down");

    // Flushing the log here, since it is called in the main function, not when DLL is unloaded.
    spdlog::details::registry::instance().flush_all();
}

PluginSystem* App::GetPluginSystem()
{
    return &m_pluginSystem;
}

HookingSystem* App::GetHookingSystem()
{
    return &m_hookingSystem;
}

StateSystem* App::GetStateSystem()
{
    return &m_stateSystem;
}

bool App::AttachHooks() const
{
    spdlog::trace("Attaching hooks...");

    DetourTransaction transaction;
    if (!transaction.IsValid())
    {
        return false;
    }

    auto success = Hooks::Main::Attach() && Hooks::CGameApplication::Attach();
    if (success)
    {
        return transaction.Commit();
    }

    return false;
}
