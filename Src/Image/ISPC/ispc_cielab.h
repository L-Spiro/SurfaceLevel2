#pragma once

extern "C" void ispc_rgb2lab( ColorRGBA * rgb, ColorLABA * lab, int N );
extern "C" void ispc_lab2rgb( ColorLABA * lab, ColorRGBA * rgb, int N );
extern "C" double ispc_deltaE_CIE94( const ColorLABA &labA, const ColorLABA &labB );
extern "C" double ispc_deltaE_CIE94_textiles( const ColorLABA &labA, const ColorLABA &labB );
extern "C" double ispc_deltaE_CIEDE2000(
    double L1, double a1, double b1, double alpha1,
    double L2, double a2, double b2, double alpha2 );
extern "C" void ispc_medianCutQuantization( Color * colors, int imageSize, int paletteSize, Color * palette );