#include "Render.hpp"

#include "App.hpp"
#include "Detail/AddressHashes.hpp"

#include <RED4ext/GpuApi/DeviceData.hpp>

namespace
{
bool isAttached = false;

bool GpuApi_InitializeDeviceData(void* a1);
Hook<decltype(&GpuApi_InitializeDeviceData)> GpuApi_InitializeDeviceData_fnc(Hashes::GpuApi_InitializeDeviceData,
                                                                             &GpuApi_InitializeDeviceData);
void GpuApi_ShutdownDeviceData();
Hook<decltype(&GpuApi_ShutdownDeviceData)> GpuApi_ShutdownDeviceData_fnc(Hashes::GpuApi_ShutdownDeviceData,
                                                                         &GpuApi_ShutdownDeviceData);
void GpuApi_ResizeBackbuffer(uint32_t aRenderWidth, uint32_t aRenderHeight, int32_t aMonitorIndex,
                             RED4ext::GpuApi::EHdrMode aHdrMode, uint32_t& aSwapChainId);
Hook<decltype(&GpuApi_ResizeBackbuffer)> GpuApi_ResizeBackbuffer_fnc(Hashes::GpuApi_ResizeBackbuffer,
                                                                     &GpuApi_ResizeBackbuffer);
void GpuApi_Present(uint32_t& aSwapChainId, int64_t a2, int32_t a3, int64_t a4);
Hook<decltype(&GpuApi_Present)> GpuApi_Present_fnc(Hashes::GpuApi_Present, &GpuApi_Present);

// a1 is likely reference to some struct, definitely contains a boolean and a string.
// Bool seems to be true when game runs from editor and false otherwise.
// String seems to either contain "EditorPipelineLibrary.cache" or "GamePipelineLibrary.cache".
// Struct is copied into the SDeviceData right at the begining, doesn't seem important now.
bool GpuApi_InitializeDeviceData(void* a1)
{
    return App::Get()->GetRenderSystem()->OnInitialization(GpuApi_InitializeDeviceData_fnc, a1);
}

// Shutdown never truly invalidates SDeviceData, it completely clears it and frees resources,
// but it always calls constructor right after desctrutor, dummying out the class in safe manner.
// If Shutdown is called, you need to wait for new Initialize call before data becomes usable again.
void GpuApi_ShutdownDeviceData()
{
    return App::Get()->GetRenderSystem()->OnShutdown(GpuApi_ShutdownDeviceData_fnc);
}

// aRenderWidth and aRenderHeight may be bigger or smaller than window size!
// aMonitorIndex is an index to structure which holds metadata about monitor window is displayed on. Used to query HDR
// mode when selecting DXGI format for swapchain.
void GpuApi_ResizeBackbuffer(uint32_t aRenderWidth, uint32_t aRenderHeight, int32_t aMonitorIndex,
                             RED4ext::GpuApi::EHdrMode aHdrMode, uint32_t& aSwapChainId)
{
    return App::Get()->GetRenderSystem()->OnResizeBackbuffers(GpuApi_ResizeBackbuffer_fnc, aRenderWidth, aRenderHeight,
                                                              aMonitorIndex, aHdrMode, aSwapChainId);
}

// a4 may be a pointer, check.
void GpuApi_Present(uint32_t& aSwapChainId, int64_t a2, int32_t a3, int64_t a4)
{
    return App::Get()->GetRenderSystem()->OnPresent(GpuApi_Present_fnc, aSwapChainId, a2, a3, a4);
}
} // namespace

namespace Hooks::Render
{
bool Attach()
{
    if (isAttached)
    {
        return false;
    }

    static const auto attach = [](auto& hook, const std::string_view name)
    {
        spdlog::trace("Trying to attach the hooks for the {} function at {:#x}...", name, hook.GetAddress());

        auto result = hook.Attach();
        if (result != NO_ERROR)
        {
            spdlog::error("Could not attach the hook for the {} function. Detour error code: {}", name, result);
            return false;
        }
        else
        {
            spdlog::trace("The hook for the {} function was attached", name);
            return true;
        }
    };

    auto attachedCount = 0;
    attachedCount += attach(GpuApi_InitializeDeviceData_fnc, "GpuApi_InitializeDeviceData");
    attachedCount += attach(GpuApi_ShutdownDeviceData_fnc, "GpuApi_ShutdownDeviceData");
    attachedCount += attach(GpuApi_ResizeBackbuffer_fnc, "GpuApi_ResizeBackbuffer");
    attachedCount += attach(GpuApi_Present_fnc, "GpuApi_Present");

    isAttached = attachedCount == 4;
    return isAttached;
}

bool Detach()
{
    if (!isAttached)
    {
        return false;
    }

    static const auto detach = [](auto& hook, const std::string_view name)
    {
        spdlog::trace("Trying to detach the hooks for the {} function at {:#x}...", name, hook.GetAddress());

        auto result = hook.Detach();
        if (result != NO_ERROR)
        {
            spdlog::error("Could not detach the hook for the {} function. Detour error code: {}", name, result);
            return false;
        }
        else
        {
            spdlog::trace("The hook for the {} function was detached", name);
            return true;
        }
    };

    auto detachedCount = 0;
    detachedCount += detach(GpuApi_InitializeDeviceData_fnc, "GpuApi_InitializeDeviceData");
    detachedCount += detach(GpuApi_ShutdownDeviceData_fnc, "GpuApi_ShutdownDeviceData");
    detachedCount += detach(GpuApi_ResizeBackbuffer_fnc, "GpuApi_ResizeBackbuffer");
    detachedCount += detach(GpuApi_Present_fnc, "GpuApi_Present");

    isAttached = detachedCount != 4;
    return !isAttached;
}
} // namespace Hooks::Render
