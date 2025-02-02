#pragma once

#include "ISystem.hpp"

#include "Hook.hpp"
#include "PluginBase.hpp"

#include <RED4ext/Api/v1/Render.hpp>

class RenderSystem final : public ISystem
{
public:
    using GpuApi_InitializeDeviceData_t = bool (*)(void*);
    using GpuApi_ShutdownDeviceData_t = void (*)();
    using GpuApi_ResizeBackbuffer_t = void (*)(uint32_t, uint32_t, int32_t, RED4ext::GpuApi::EHdrMode, uint32_t&);
    using GpuApi_Present_t = void (*)(uint32_t&, int64_t, int32_t, int64_t);

    ESystemType GetType() final;

    void Startup() final;
    void Shutdown() final;

    bool AddHook(std::shared_ptr<PluginBase> aPlugin, const RED4ext::v1::Render& aRender);

    bool OnInitialization(Hook<GpuApi_InitializeDeviceData_t>& aHooked, void* a1);
    void OnShutdown(Hook<GpuApi_ShutdownDeviceData_t>& aHooked);
    void OnResizeBackbuffers(Hook<GpuApi_ResizeBackbuffer_t>& aHooked, uint32_t aRenderWidth, uint32_t aRenderHeight,
                             int32_t aMonitorIndex, RED4ext::GpuApi::EHdrMode aHdrMode, uint32_t& aSwapChainId);
    void OnPresent(Hook<GpuApi_Present_t>& aHooked, uint32_t& aSwapChainId, int64_t a2, int32_t a3, int64_t a4);

private:
    template<typename Func_t>
    struct RenderItem
    {
        std::shared_ptr<PluginBase> plugin;
        Func_t func;
    };

    template<typename Func_t>
    struct Render
    {
        std::wstring_view GetName() const
        {
            return name;
        }

        void Shutdown()
        {
            onBefore.clear();
            onAfter.clear();
        }

        std::wstring_view name;
        std::vector<RenderItem<Func_t>> onBefore;
        std::vector<RenderItem<Func_t>> onAfter;
    };

    template<typename Func_t, typename... Args>
    void Run(std::wstring_view aAction, std::vector<RenderItem<Func_t>>& aList, Args&&... args);

    Render<bool (*)(RED4ext::GpuApi::SDeviceData&)> m_initialization = {.name = L"InitializeDevice"};
    Render<bool (*)(RED4ext::GpuApi::SDeviceData&)> m_shutdown = {.name = L"ShutdownDevice"};
    Render<bool (*)(uint32_t, uint32_t, int32_t, RED4ext::GpuApi::EHdrMode, RED4ext::GpuApi::SSwapChainData&)>
        m_resizeBackbuffer = {.name = L"ResizeBackbuffer"};
    Render<bool (*)(RED4ext::GpuApi::SSwapChainData&)> m_present = {.name = L"Present"};
};

template<typename Func_t, typename... Args>
void RenderSystem::Run(std::wstring_view aAction, std::vector<RenderItem<Func_t>>& aList, Args&&... args)
{
    for (auto it = aList.begin(); it != aList.end();)
    {
        const std::wstring_view pluginName = it->plugin ? it->plugin->GetName() : L"RED4ext";

        try
        {
            if (it->func(std::forward<Args>(args)...))
            {
                spdlog::trace(L"Finished running '{}' registered by '{}'", aAction, pluginName);
                it = aList.erase(it);
            }
            else
            {
                ++it;
            }
        }
        catch (const std::exception& e)
        {
            spdlog::warn(L"An exception occured while executing '{}' registered by '{}'", aAction, pluginName);
            spdlog::warn(e.what());
        }
        catch (...)
        {
            spdlog::warn(L"An unknown exception occured while executing '{}' registered by '{}'", aAction, pluginName);
        }
    }
}
