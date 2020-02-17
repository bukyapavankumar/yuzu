// Copyright 2018 yuzu Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <atomic>
#include <functional>
#include "common/common_types.h"
#include "video_core/engines/fermi_2d.h"
#include "video_core/gpu.h"
#include "video_core/guest_driver.h"

namespace Tegra {
class MemoryManager;
}

namespace VideoCore {

enum class LoadCallbackStage {
    Prepare,
    Decompile,
    Build,
    Complete,
};
using DiskResourceLoadCallback = std::function<void(LoadCallbackStage, std::size_t, std::size_t)>;

class RasterizerInterface {
public:
    virtual ~RasterizerInterface() {}

    /// Draw the current batch of vertex arrays
    virtual bool DrawBatch(bool is_indexed) = 0;

    /// Draw the current batch of multiple instances of vertex arrays
    virtual bool DrawMultiBatch(bool is_indexed) = 0;

    /// Clear the current framebuffer
    virtual void Clear() = 0;

    /// Dispatches a compute shader invocation
    virtual void DispatchCompute(GPUVAddr code_addr) = 0;

    /// Notify rasterizer that all caches should be flushed to Switch memory
    virtual void FlushAll() = 0;

    /// Notify rasterizer that any caches of the specified region should be flushed to Switch memory
    virtual void FlushRegion(CacheAddr addr, u64 size) = 0;

    /// Notify rasterizer that any caches of the specified region should be flushed to Switch memory
    virtual bool MustFlushRegion(CacheAddr addr, u64 size) = 0;

    /// Notify rasterizer that any caches of the specified region should be invalidated
    virtual void InvalidateRegion(CacheAddr addr, u64 size) = 0;

    /// Notify rasterizer that any caches of the specified region should be flushed to Switch memory
    /// and invalidated
    virtual void FlushAndInvalidateRegion(CacheAddr addr, u64 size) = 0;

    /// Notify the rasterizer to send all written commands to the host GPU.
    virtual void FlushCommands() = 0;

    /// Notify rasterizer that a frame is about to finish
    virtual void TickFrame() = 0;

    /// Attempt to use a faster method to perform a surface copy
    virtual bool AccelerateSurfaceCopy(const Tegra::Engines::Fermi2D::Regs::Surface& src,
                                       const Tegra::Engines::Fermi2D::Regs::Surface& dst,
                                       const Tegra::Engines::Fermi2D::Config& copy_config) {
        return false;
    }

    /// Attempt to use a faster method to display the framebuffer to screen
    virtual bool AccelerateDisplay(const Tegra::FramebufferConfig& config, VAddr framebuffer_addr,
                                   u32 pixel_stride) {
        return false;
    }

    /// Increase/decrease the number of object in pages touching the specified region
    virtual void UpdatePagesCachedCount(VAddr addr, u64 size, int delta) {}

    /// Initialize disk cached resources for the game being emulated
    virtual void LoadDiskResources(const std::atomic_bool& stop_loading = false,
                                   const DiskResourceLoadCallback& callback = {}) {}

    /// Grant access to the Guest Driver Profile for recording/obtaining info on the guest driver.
    GuestDriverProfile& AccessGuestDriverProfile() {
        return guest_driver_profile;
    }

    /// Grant access to the Guest Driver Profile for recording/obtaining info on the guest driver.
    const GuestDriverProfile& AccessGuestDriverProfile() const {
        return guest_driver_profile;
    }

private:
    GuestDriverProfile guest_driver_profile{};
};
} // namespace VideoCore
