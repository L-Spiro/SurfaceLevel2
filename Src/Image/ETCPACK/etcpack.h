#pragma once

typedef unsigned char uint8;
extern void compressBlockETC2ExhaustivePerceptual(uint8 *img, uint8 *imgdec,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);
extern void compressBlockETC2FastPerceptual(uint8 *img, uint8 *imgdec,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);
extern void compressBlockETC2Exhaustive(uint8 *img, uint8 *imgdec,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);
extern void compressBlockETC2Fast(uint8 *img, uint8* alphaimg, uint8 *imgdec,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);
extern void compressBlockAlphaSlow(uint8* data, int ix, int iy, int width, int /*height*/, uint8* returnData);
extern void compressBlockAlphaFast(uint8 * data, int ix, int iy, int width, int /*height*/, uint8* returnData);
extern void compressBlockAlpha16(uint8* data, int ix, int iy, int width, int height, uint8* returnData);
extern int format;
extern int formatSigned_;
extern bool readCompressParams(void);
extern void setupAlphaTableAndValtab();
enum {ETC1_RGB_NO_MIPMAPS,ETC2PACKAGE_RGB_NO_MIPMAPS,ETC2PACKAGE_RGBA_NO_MIPMAPS_OLD,ETC2PACKAGE_RGBA_NO_MIPMAPS,ETC2PACKAGE_RGBA1_NO_MIPMAPS,ETC2PACKAGE_R_NO_MIPMAPS,ETC2PACKAGE_RG_NO_MIPMAPS,ETC2PACKAGE_R_SIGNED_NO_MIPMAPS,ETC2PACKAGE_RG_SIGNED_NO_MIPMAPS,ETC2PACKAGE_sRGB_NO_MIPMAPS,ETC2PACKAGE_sRGBA_NO_MIPMAPS,ETC2PACKAGE_sRGBA1_NO_MIPMAPS};
enum {SPEED_SLOW, SPEED_FAST, SPEED_MEDIUM};
enum {METRIC_PERCEPTUAL, METRIC_NONPERCEPTUAL};
enum {CODEC_ETC, CODEC_ETC2};