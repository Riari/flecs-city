const std = @import("std");
const Vcpkg = @import("build/vcpkg.zig").Vcpkg;
const utils = @import("build/utils.zig");
const core = @import("src/Modules/Core/build.zig");

pub fn build(b: *std.Build) void
{
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const vcpkg = Vcpkg.init(b, target);

    const lib_core = core.build(b, target, optimize, vcpkg);

    const mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    mod.addCSourceFiles(.{
        .files = &.{
            "src/main.cpp",
            "src/Private/Application/Application.cpp",
            "src/Private/Environment/Options.cpp",
            "src/Private/Network/ClientThread.cpp",
            "src/Private/Network/NetworkThread.cpp",
            "src/Private/Network/ServerThread.cpp",
        },
        .flags = &.{"-std=c++17"},
    });

    mod.addCMacro("SPDLOG_HEADER_ONLY", "1");
    mod.addCMacro("FMT_HEADER_ONLY", "1");

    mod.addIncludePath(b.path("src/Public"));
    mod.addIncludePath(b.path("src/Private"));
    mod.addIncludePath(b.path("src/Modules"));
    mod.addIncludePath(vcpkg.inc_path);
    mod.addLibraryPath(vcpkg.lib_path);

    mod.linkSystemLibrary("c++", .{});
    mod.linkSystemLibrary("enet", .{});

    if (target.result.os.tag == .windows)
    {
        utils.Windows.linkRaylib(mod);
        mod.linkSystemLibrary("flecs.dll", .{});
        mod.linkSystemLibrary("glfw3dll", .{});

        const vcpkg_binaries = b.addInstallDirectory(.{
            .source_dir = vcpkg.bin_path,
            .install_dir = .bin,
            .install_subdir = "",
        });

        b.getInstallStep().dependOn(&vcpkg_binaries.step);
    }
    else
    {
        mod.linkSystemLibrary("flecs", .{});
        mod.linkSystemLibrary("glfw", .{});
        mod.linkSystemLibrary("raylib", .{});
    }

    mod.linkLibrary(lib_core);

    const exe = b.addExecutable(.{
        .name = "flecs_city",
        .root_module = mod,
    });

    b.installArtifact(exe);
    b.installArtifact(lib_core);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }
}
