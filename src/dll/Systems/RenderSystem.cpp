#include "stdafx.hpp"

#include "RenderSystem.hpp"

#include <RED4ext/GpuApi/DeviceData.hpp>

ESystemType RenderSystem::GetType()
{
    return ESystemType::Render;
}

void RenderSystem::Startup()
{
}

void RenderSystem::Shutdown()
{
    spdlog::trace("Removing all render hooks...");

    m_initialization.Shutdown();
    m_shutdown.Shutdown();
    m_resizeBackbuffer.Shutdown();
    m_present.Shutdown();

    spdlog::trace("All render hooks were removed successfully");
}

bool RenderSystem::AddHook(std::shared_ptr<PluginBase> aPlugin, const RED4ext::v1::Render& aRender)
{
    if (aRender.OnBeforeInitialization)
    {
        m_initialization.onBefore.emplace_back(aPlugin, aRender.OnBeforeInitialization);
    }

    if (aRender.OnAfterInitialization)
    {
        m_initialization.onAfter.emplace_back(aPlugin, aRender.OnAfterInitialization);
    }

    if (aRender.OnBeforeShutdown)
    {
        m_shutdown.onBefore.emplace_back(aPlugin, aRender.OnBeforeShutdown);
    }

    if (aRender.OnAfterShutdown)
    {
        m_shutdown.onAfter.emplace_back(aPlugin, aRender.OnAfterShutdown);
    }

    if (aRender.OnBeforeResizeBuffers)
    {
        m_resizeBackbuffer.onBefore.emplace_back(aPlugin, aRender.OnBeforeResizeBuffers);
    }

    if (aRender.OnAfterResizeBuffers)
    {
        m_resizeBackbuffer.onAfter.emplace_back(aPlugin, aRender.OnAfterResizeBuffers);
    }

    if (aRender.OnBeforePresent)
    {
        m_present.onBefore.emplace_back(aPlugin, aRender.OnBeforePresent);
    }

    if (aRender.OnAfterPresent)
    {
        m_present.onAfter.emplace_back(aPlugin, aRender.OnAfterPresent);
    }

    spdlog::trace(L"The request to add a render hook for '{}' has been successfully completed", aPlugin->GetName());
    return true;
}

bool RenderSystem::OnInitialization(Hook<GpuApi_InitializeDeviceData_t>& aHooked, void* a1)
{
    Run(fmt::format(L"GpuApi::OnBefore{}", m_initialization.GetName()), m_initialization.onBefore,
        *RED4ext::GpuApi::GetDeviceData());
    const auto result = aHooked(a1);
    Run(fmt::format(L"GpuApi::OnAfter{}", m_initialization.GetName()), m_initialization.onAfter,
        *RED4ext::GpuApi::GetDeviceData());

    return result;
}

void RenderSystem::OnShutdown(Hook<GpuApi_ShutdownDeviceData_t>& aHooked)
{
    Run(fmt::format(L"GpuApi::OnBefore{}", m_shutdown.GetName()), m_shutdown.onBefore,
        *RED4ext::GpuApi::GetDeviceData());
    aHooked();
    Run(fmt::format(L"GpuApi::OnAfter{}", m_shutdown.GetName()), m_shutdown.onAfter, *RED4ext::GpuApi::GetDeviceData());
}

void RenderSystem::OnResizeBackbuffers(Hook<GpuApi_ResizeBackbuffer_t>& aHooked, uint32_t aRenderWidth,
                                       uint32_t aRenderHeight, int32_t aMonitorIndex,
                                       RED4ext::GpuApi::EHdrMode aHdrMode, uint32_t& aSwapChainId)
{
    Run(fmt::format(L"GpuApi::OnBefore{}", m_resizeBackbuffer.GetName()), m_resizeBackbuffer.onBefore, aRenderWidth,
        aRenderHeight, aMonitorIndex, aHdrMode, RED4ext::GpuApi::GetDeviceData()->swapChains.GetData(aSwapChainId));
    aHooked(aRenderWidth, aRenderHeight, aMonitorIndex, aHdrMode, aSwapChainId);
    Run(fmt::format(L"GpuApi::OnAfter{}", m_resizeBackbuffer.GetName()), m_resizeBackbuffer.onAfter, aRenderWidth,
        aRenderHeight, aMonitorIndex, aHdrMode, RED4ext::GpuApi::GetDeviceData()->swapChains.GetData(aSwapChainId));
}

void RenderSystem::OnPresent(Hook<GpuApi_Present_t>& aHooked, uint32_t& aSwapChainId, int64_t a2, int32_t a3,
                             int64_t a4)
{
    Run(fmt::format(L"GpuApi::OnBefore{}", m_present.GetName()), m_present.onBefore,
        RED4ext::GpuApi::GetDeviceData()->swapChains.GetData(aSwapChainId));
    aHooked(aSwapChainId, a2, a3, a4);
    Run(fmt::format(L"GpuApi::OnAfter{}", m_present.GetName()), m_present.onAfter,
        RED4ext::GpuApi::GetDeviceData()->swapChains.GetData(aSwapChainId));
}
