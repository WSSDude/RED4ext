#include "Addresses.hpp"
#include "App.hpp"
#include "Hook.hpp"
#include "RedmodCompilation.hpp"
#include "Systems/ScriptSystem.hpp"
#include "stdafx.hpp"

namespace
{
bool isAttached = false;

void* _Scripts_RedmodCompile(RED4ext::CBaseEngine* engine, unsigned __int8 a2, unsigned __int16 a3);
Hook<decltype(&_Scripts_RedmodCompile)> Scripts_RedmodCompile(Addresses::Scripts_RedmodCompile,
                                                              &_Scripts_RedmodCompile);

void* _Scripts_RedmodCompile(RED4ext::CBaseEngine* engine, unsigned __int8 a2, unsigned __int16 a3)
{
    auto scriptSystem = App::Get()->GetScriptSystem();
    if (engine->scriptsBlobPath.Length())
    {
        spdlog::info("Found scriptsBlobPath: '{}'", engine->scriptsBlobPath.c_str());
        scriptSystem->usingRedmod = true;
        scriptSystem->engine = engine;
        scriptSystem->scriptsBlobPath = engine->scriptsBlobPath;
        engine->scriptsBlobPath = "";
    }

    auto result = Scripts_RedmodCompile(engine, a2, a3);

    return result;
}
} // namespace

bool Hooks::RedmodCompilation::Attach()
{
    spdlog::trace("Trying to attach the hook for the redscript compilation at {}...",
                  RED4EXT_OFFSET_TO_ADDR(Addresses::Scripts_RedmodCompile));

    auto result = Scripts_RedmodCompile.Attach();
    if (result != NO_ERROR)
    {
        spdlog::error("Could not attach the hook for the redscript compilation. Detour error code: {}", result);
    }
    else
    {
        spdlog::trace("The hook for the redscript compilation was attached");
    }

    isAttached = result == NO_ERROR;
    return isAttached;
}

bool Hooks::RedmodCompilation::Detach()
{
    if (!isAttached)
    {
        return false;
    }

    spdlog::trace("Trying to detach the hook for the redscript compilation at {}...",
                  RED4EXT_OFFSET_TO_ADDR(Addresses::Scripts_RedmodCompile));

    auto result = Scripts_RedmodCompile.Detach();
    if (result != NO_ERROR)
    {
        spdlog::error("Could not detach the hook for the redscript compilation. Detour error code: {}", result);
    }
    else
    {
        spdlog::trace("The hook for the redscript compilation was detached");
    }

    isAttached = result != NO_ERROR;
    return !isAttached;
}
