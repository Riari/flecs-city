const std = @import("std");

pub const Windows = struct
{
    // Links Raylib and Windows dependencies with some features disabled.
    // Mainly for compatibility with ENet.
    pub fn linkRaylib(mod: *std.Build.Module) void
    {
        mod.linkSystemLibrary("winmm", .{});
        mod.linkSystemLibrary("ws2_32", .{});
        mod.addCMacro("WIN32_LEAN_AND_MEAN", "1");
        mod.addCMacro("NOGDI", "1");
        mod.addCMacro("NOUSER", "1");

        mod.linkSystemLibrary("raylib.dll", .{});
    }
}
