cc_library(
    name = "ext_sort",
    hdrs = [
        "sorter.h",
    ],
    srcs = [
        "traits.h",
        "iterators.h",
    ],
    copts = [
        "-std=c++11",
        "-Wall",
    ],
    linkopts = [
        "-lstdc++",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "ut",
    srcs = ["ext_sort_ut.cpp"],
    copts = [
        "-Iexternal/gtest/include",
    ],
    deps = [
        ":ext_sort",
        "@gtest//:main",
    ],
)
