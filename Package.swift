// swift-tools-version:5.3
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

#if os(Windows)
#if arch(i386) || arch(x86_64)
    var cSettings: [CSetting] = [
        .define("_X86_", .when(platforms: [.windows])),
    ]
#elseif arch(arm) || arch(arm64)
    var cSettings: [CSetting] = [
        .define("_ARM_", .when(platforms: [.windows])),
    ]
#else
#error("unsupported architecture")
#endif
    // When building the library on Windows, we do not have proper control of
    // whether it is being built statically or dynamically.  However, given the
    // current default of static, we will assume that we are building
    // statically.  More importantly, should this not hold, this will fail at
    // link time.
    cSettings += [
        .define("CMARK_GFM_STATIC_DEFINE", .when(platforms: [.windows])),
        .define("CMARK_GFM_EXTENSIONS_STATIC_DEFINE", .when(platforms: [.windows])),
    ]
#else
    let cSettings: [CSetting] = []
#endif

let package = Package(
    name: "cmark-gfm",
    products: [
        // Products define the executables and libraries a package produces, and make them visible to other packages.
        .library(
            name: "cmark-gfm",
            targets: ["cmark-gfm"]),
        .library(
            name: "cmark-gfm-extensions",
            targets: ["cmark-gfm-extensions"]),
        .executable(
            name: "cmark-gfm-bin",
            targets: ["cmark-gfm-bin"]),
        .executable(name: "api_test",
            targets: ["api_test"])
    ],
    targets: [
        .target(name: "cmark-gfm",
          path: "src",
          exclude: [
            "scanners.re",
            "libcmark-gfm.pc.in",
            "config.h.in",
            "CMakeLists.txt",
            "cmark-gfm_version.h.in",
            "case_fold_switch.inc",
            "entities.inc",
          ],
          cSettings: cSettings
        ),
        .target(name: "cmark-gfm-extensions",
          dependencies: [
            "cmark-gfm",
          ],
          path: "extensions",
          exclude: [
            "CMakeLists.txt",
            "ext_scanners.re",
          ],
          cSettings: cSettings
        ),
        .target(name: "cmark-gfm-bin",
          dependencies: [
            "cmark-gfm",
            "cmark-gfm-extensions",
          ],
          path: "bin",
          sources: [
            "main.c",
          ]
        ),
        .target(name: "api_test",
          dependencies: [
            "cmark-gfm",
            "cmark-gfm-extensions",
          ],
          path: "api_test",
          exclude: [
            "CMakeLists.txt",
          ]
        )
    ]
)
