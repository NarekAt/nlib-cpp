NLIB_GCC_EXCEPTIONS_FLAGS = [
    "-fexceptions",
]

NLIB_GCC_FLAGS = [
    "-Wall",
    "-Wextra",
    "-Wcast-qual",
    "-Wconversion-null",
    "-Wmissing-declarations",
    "-Woverlength-strings",
    "-Wpointer-arith",
    "-Wunused-local-typedefs",
    "-Wunused-result",
    "-Wvarargs",
    "-Wvla",
    "-Wwrite-strings",
    "-Wno-missing-field-initializers",
    "-Wno-sign-compare",
    "-std=c++17",
]

NLIB_GCC_TEST_FLAGS = [
    "-Wno-conversion-null",
    "-Wno-deprecated-declarations",
    "-Wno-missing-declarations",
    "-Wno-sign-compare",
    "-Wno-unused-function",
    "-Wno-unused-parameter",
    "-Wno-unused-private-field",
    "-std=c++17",
]

NLIB_MSVC_EXCEPTIONS_FLAGS = [
    "/U_HAS_EXCEPTIONS",
    "/D_HAS_EXCEPTIONS=1",
    "/EHsc",
]

NLIB_MSVC_FLAGS = [
    "/W3",
    "/DNOMINMAX",
    "/DWIN32_LEAN_AND_MEAN",
    "/D_CRT_SECURE_NO_WARNINGS",
    "/D_SCL_SECURE_NO_WARNINGS",
    "/D_ENABLE_EXTENDED_ALIGNED_STORAGE",
    "/wd4005",
    "/wd4068",
    "/wd4180",
    "/wd4244",
    "/wd4267",
    "/wd4503",
    "/wd4800",
]

NLIB_MSVC_LINKOPTS = [
    "-ignore:4221",
]

NLIB_MSVC_TEST_FLAGS = [
    "/wd4018",
    "/wd4101",
    "/wd4503",
    "/wd4996",
    "/DNOMINMAX",
]
