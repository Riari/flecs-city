const std = @import("std");

pub fn build(b: *std.Build) void
{
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const vcpkg_root = b.option([]const u8, "vcpkg_root", "") orelse "vcpkg_installed";
    const vcpkg_triplet = b.option([]const u8, "vcpkg_triplet", "") orelse "x64-mingw-dynamic";
    const vcpkg_inc = b.fmt("{s}/{s}/include", .{ vcpkg_root, vcpkg_triplet });
    const vcpkg_lib = b.fmt("{s}/{s}/lib", .{ vcpkg_root, vcpkg_triplet });
    const vcpkg_bin = b.fmt("{s}/{s}/bin", .{ vcpkg_root, vcpkg_triplet });

    const core_dep = b.dependency("Core", .{
        .target = target,
        .optimize = optimize,
    });
    const core_lib = core_dep.artifact("Core");

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

    if (target.result.os.tag == .windows)
    {
        mod.linkSystemLibrary("winmm", .{});
        mod.linkSystemLibrary("ws2_32", .{});
        mod.addCMacro("WIN32_LEAN_AND_MEAN", "1");
        mod.addCMacro("NOGDI", "1");
        mod.addCMacro("NOUSER", "1");
    }

    mod.addCMacro("FMT_HEADER_ONLY", "1");

    mod.addIncludePath(b.path("src/Public"));
    mod.addIncludePath(b.path("src/Private"));
    mod.addIncludePath(b.path(vcpkg_inc));
    mod.addIncludePath(core_dep.path("."));

    mod.addLibraryPath(b.path(vcpkg_lib));

    mod.linkSystemLibrary("c++", .{});
    mod.addObjectFile(b.path(b.fmt("{s}/libspdlog.dll.a", .{vcpkg_lib})));
    mod.addObjectFile(b.path(b.fmt("{s}/libfmt.dll.a", .{vcpkg_lib})));
    mod.addObjectFile(b.path(b.fmt("{s}/libflecs.dll.a", .{vcpkg_lib})));
    mod.addObjectFile(b.path(b.fmt("{s}/libglfw3dll.a", .{vcpkg_lib})));
    mod.addObjectFile(b.path(b.fmt("{s}/libraylib.dll.a", .{vcpkg_lib})));
    mod.addObjectFile(b.path(b.fmt("{s}/libenet.a", .{vcpkg_lib})));

    mod.linkLibrary(core_lib);

    const exe = b.addExecutable(.{
        .name = "flecs_city",
        .root_module = mod,
    });

    b.installArtifact(exe);
    b.installArtifact(core_lib);

    const vcpkg_binaries = b.addInstallDirectory(.{
        .source_dir = b.path(vcpkg_bin),
        .install_dir = .bin,
        .install_subdir = "",
    });

    b.getInstallStep().dependOn(&vcpkg_binaries.step);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }
}
