const std = @import("std");

pub const Vcpkg = struct
{
    root: []const u8,
    triplet: []const u8,
    inc_path: std.Build.LazyPath,
    lib_path: std.Build.LazyPath,
    bin_path: std.Build.LazyPath,

    pub fn init(b: *std.Build, target: std.Build.ResolvedTarget) Vcpkg
    {
        const root = b.option([]const u8, "vcpkg_root", "") orelse "vcpkg_installed";
        const default_triplet = if (target.result.os.tag == .windows) "x64-mingw-dynamic" else "x64-linux-dynamic";
        const triplet = b.option([]const u8, "vcpkg_triplet", "") orelse default_triplet;

        return Vcpkg
        {
            .root = root,
            .triplet = triplet,
            .inc_path = b.path(b.fmt("{s}/{s}/include", .{ root, triplet })),
            .lib_path = b.path(b.fmt("{s}/{s}/lib", .{ root, triplet })),
            .bin_path = b.path(b.fmt("{s}/{s}/bin", .{ root, triplet })),
        };
    }
};
