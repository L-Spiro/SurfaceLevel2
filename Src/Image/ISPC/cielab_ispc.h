//
// C:\My Projects\SurfaceLevel2\Src\Image\Ispc\cielab_ispc.h
// (Header automatically generated by the ispc compiler.)
// DO NOT EDIT THIS FILE.
//

#pragma once
#include <stdint.h>



#ifdef __cplusplus
namespace ispc { /* namespace */
#endif // __cplusplus


#ifndef __ISPC_ALIGN__
#if defined(__clang__) || !defined(_MSC_VER)
// Clang, GCC, ICC
#define __ISPC_ALIGN__(s) __attribute__((aligned(s)))
#define __ISPC_ALIGNED_STRUCT__(s) struct __ISPC_ALIGN__(s)
#else
// Visual Studio
#define __ISPC_ALIGN__(s) __declspec(align(s))
#define __ISPC_ALIGNED_STRUCT__(s) __ISPC_ALIGN__(s) struct
#endif
#endif

#ifndef __ISPC_STRUCT_ColorLABA__
#define __ISPC_STRUCT_ColorLABA__
struct ColorLABA {
    double l;
    double a;
    double b;
    double alpha;
};
#endif

#ifndef __ISPC_STRUCT_ColorRGBA__
#define __ISPC_STRUCT_ColorRGBA__
struct ColorRGBA {
    double r;
    double g;
    double b;
    double a;
};
#endif

#ifndef __ISPC_STRUCT_Color__
#define __ISPC_STRUCT_Color__
struct Color {
    double r;
    double g;
    double b;
    double a;
};
#endif


///////////////////////////////////////////////////////////////////////////
// Functions exported from ispc code
///////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus) && (! defined(__ISPC_NO_EXTERN_C) || !__ISPC_NO_EXTERN_C )
extern "C" {
#endif // __cplusplus
#if defined(__cplusplus)
    extern double ispc_deltaE_CIE94(const struct ColorLABA &labA, const struct ColorLABA &labB);
#else
    extern double ispc_deltaE_CIE94(const struct ColorLABA *labA, const struct ColorLABA *labB);
#endif // ispc_deltaE_CIE94 function declaraion
#if defined(__cplusplus)
    extern double ispc_deltaE_CIE94_textiles(const struct ColorLABA &labA, const struct ColorLABA &labB);
#else
    extern double ispc_deltaE_CIE94_textiles(const struct ColorLABA *labA, const struct ColorLABA *labB);
#endif // ispc_deltaE_CIE94_textiles function declaraion
    extern double ispc_deltaE_CIEDE2000(double L1, double a1, double b1, double alpha1, double L2, double a2, double b2, double alpha2);
    extern void ispc_lab2rgb(const struct ColorLABA * lab, struct ColorRGBA * rgb, uint32_t n);
    extern void ispc_medianCutQuantization(struct Color * colors, int32_t imageSize, int32_t paletteSize, struct Color * palette);
    extern void ispc_rgb2lab(const struct ColorRGBA * rgb, struct ColorLABA * lab, uint32_t n);
#if defined(__cplusplus) && (! defined(__ISPC_NO_EXTERN_C) || !__ISPC_NO_EXTERN_C )
} /* end extern C */
#endif // __cplusplus


#ifndef __ISPC_ALIGN__
#if defined(__clang__) || !defined(_MSC_VER)
// Clang, GCC, ICC
#define __ISPC_ALIGN__(s) __attribute__((aligned(s)))
#define __ISPC_ALIGNED_STRUCT__(s) struct __ISPC_ALIGN__(s)
#else
// Visual Studio
#define __ISPC_ALIGN__(s) __declspec(align(s))
#define __ISPC_ALIGNED_STRUCT__(s) __ISPC_ALIGN__(s) struct
#endif
#endif



#ifdef __cplusplus
} /* namespace */
#endif // __cplusplus