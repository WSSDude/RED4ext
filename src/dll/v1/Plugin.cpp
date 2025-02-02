#include "stdafx.hpp"

#include "Image.hpp"
#include "v1/Funcs.hpp"
#include "v1/Logger.hpp"
#include "v1/Plugin.hpp"

namespace v1
{
Plugin::Plugin(const std::filesystem::path& aPath, wil::unique_hmodule aModule)
    : PluginBase(aPath, std::move(aModule))
    , m_info{}
    , m_runtime(Image::Get()->GetProductVersion())
    , m_logger{}
    , m_hooking{}
    , m_gameStates{}
    , m_scripts{}
    , m_sdk{.runtime = m_runtime,
            .logger = m_logger,
            .hooking = m_hooking,
            .gameStates = m_gameStates,
            .scripts = m_scripts,
            .rendering = m_rendering}
{
    m_logger.Trace = Logger::Trace;
    m_logger.TraceF = Logger::TraceF;
    m_logger.TraceW = Logger::TraceW;
    m_logger.TraceWF = Logger::TraceWF;
    m_logger.Debug = Logger::Debug;
    m_logger.DebugF = Logger::DebugF;
    m_logger.DebugW = Logger::DebugW;
    m_logger.DebugWF = Logger::DebugWF;
    m_logger.Info = Logger::Info;
    m_logger.InfoF = Logger::InfoF;
    m_logger.InfoW = Logger::InfoW;
    m_logger.InfoWF = Logger::InfoWF;
    m_logger.Warn = Logger::Warn;
    m_logger.WarnF = Logger::WarnF;
    m_logger.WarnWF = Logger::WarnWF;
    m_logger.WarnW = Logger::WarnW;
    m_logger.Error = Logger::Error;
    m_logger.ErrorF = Logger::ErrorF;
    m_logger.ErrorW = Logger::ErrorW;
    m_logger.ErrorWF = Logger::ErrorWF;
    m_logger.Critical = Logger::Critical;
    m_logger.CriticalF = Logger::CriticalF;
    m_logger.CriticalW = Logger::CriticalW;
    m_logger.CriticalWF = Logger::CriticalWF;

    m_hooking.Attach = Hooking::Attach;
    m_hooking.Detach = Hooking::Detach;

    m_gameStates.AddHook = GameStates::AddHook;

    m_scripts.Add = Scripts::Add;
}

const uint32_t Plugin::GetApiVersion() const
{
    return RED4EXT_API_VERSION_1;
}

void* Plugin::GetPluginInfo()
{
    return &m_info;
}

const void* Plugin::GetSdkStruct() const
{
    return &m_sdk;
}

const std::wstring_view Plugin::GetName() const
{
    return m_info.name;
}

const std::wstring_view Plugin::GetAuthor() const
{
    return m_info.author;
}

const RED4ext::SemVer& Plugin::GetVersion() const
{
    return m_info.version;
}

const RED4ext::FileVer& Plugin::GetRuntimeVersion() const
{
    return m_info.runtime;
}

const RED4ext::SemVer& Plugin::GetSdkVersion() const
{
    return m_info.sdk;
}
} // namespace v1
