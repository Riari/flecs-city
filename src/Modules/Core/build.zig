const std = @import("std");

pub fn build(b: *std.Build) void
{
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const vcpkg_root = b.option([]const u8, "vcpkg_root", "") orelse "vcpkg_installed";
    const vcpkg_triplet = b.option([]const u8, "vcpkg_triplet", "") orelse "x64-mingw-dynamic";
    const vcpkg_inc = b.fmt("{s}/{s}/include", .{ vcpkg_root, vcpkg_triplet });

    mod.addCSourceFiles(.{
        .files = &.{
            "Core.cpp",
            "ECS/Phases.cpp",
        },
        .flags = &.{"-std=c++17"},
    });

    mod.addIncludePath(.{ .cwd_relative = "src/Public" });
    mod.addIncludePath(b.path(".."));
    mod.addIncludePath(.{ .cwd_relative = vcpkg_inc });

    mod.addCMacro("EXPORTS", "");
    mod.addCMacro("SPDLOG_HEADER_ONLY", "1");
    mod.addCMacro("FMT_HEADER_ONLY", "1");

    mod.linkSystemLibrary("c++", .{});

    const lib = b.addLibrary(.{ .name = "Core", .root_module = mod, .linkage = .dynamic });

    b.installArtifact(lib);
}
