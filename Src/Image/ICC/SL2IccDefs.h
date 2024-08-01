/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: ICC defines mercilessly ripped from https://www.color.org/icc32.pdf.
 */


#pragma once

#include <cstdint>


namespace sl2 {

	/*
	 * Defines used in the specification
	 */
#define icMagicNumber					0x61637370L								/* ‘acsp’ */
#define icVersionNumber					0x02000000L								/* 2.0, BCD */
	/* Screening Encodings */
#define icPrtrDefaultScreensFalse		0x00000000L								/* Bit position 0 */
#define icPrtrDefaultScreensTrue		0x00000001L								/* Bit position 0 */
#define icLinesPerInch					0x00000002L								/* Bit position 1 */
#define icLinesPerCm					0x00000000L								/* Bit position 1 */
	/*
	 * Device attributes, currently defined values correspond
	 * to the low 4 bytes of the 8 byte attribute quantity, see
	 * the header for their location.
	 */
#define icReflective					0x00000000L								/* Bit position 0 */
#define icTransparency					0x00000001L								/* Bit position 0 */
#define icGlossy						0x00000000L								/* Bit position 1 */
#define icMatte							0x00000002L								/* Bit position 1 */
	/*
	 * Profile header flags, the low 16 bits are reserved for consortium
	 * use.
	 */
#define icEmbeddedProfileFalse			0x00000000L								/* Bit position 0 */
#define icEmbeddedProfileTrue			0x00000001L								/* Bit position 0 */
#define icUseAnywhere					0x00000000L								/* Bit position 1 */
#define icUseWithEmbeddedDataOnly		0x00000002L								/* Bit position 1 */
	/* Ascii or Binary data */
#define icAsciiData						0x00000000L								/* Used in dataType */
#define icBinaryData					0x00000001L
	/*
	 * Define used to indicate that this is a variable length array
	 */
#define icAny 1

	/* public tags and sizes */
	typedef enum {
		icSigAToB0Tag = 0x41324230L,/* ‘A2B0’ */
		icSigAToB1Tag = 0x41324231L,/* ‘A2B1’ */
		icSigAToB2Tag = 0x41324232L,/* ‘A2B2’ */
		icSigBlueColorantTag = 0x6258595AL,/* ‘bXYZ’ */
		icSigBlueTRCTag = 0x62545243L,/* ‘bTRC’ */
		icSigBToA0Tag = 0x42324130L,/* ‘B2A0’ */
		icSigBToA1Tag = 0x42324131L,/* ‘B2A1’ */
		icSigBToA2Tag = 0x42324132L,/* ‘B2A2’ */
		icSigCalibrationDateTimeTag = 0x63616C74L,/* ‘calt’ */
		icSigCharTargetTag = 0x74617267L,/* ‘targ’ */
		icSigCopyrightTag = 0x63707274L,/* ‘cprt’ */
		icSigDeviceMfgDescTag = 0x646D6E64L,/* ‘dmnd’ */
		icSigDeviceModelDescTag = 0x646D6464L,/* ‘dmdd’ */
		icSigGamutTag = 0x67616D74L,/* ‘gamt ‘ */
		icSigGrayTRCTag = 0x6b545243L,/* ‘kTRC’ */
		icSigGreenColorantTag = 0x6758595AL,/* ‘gXYZ’ */
		icSigGreenTRCTag = 0x67545243L,/* ‘gTRC’ */
		icSigLuminanceTag = 0x6C756d69L,/* ‘lumi’ */
		icSigMeasurementTag = 0x6D656173L,/* ‘meas’ */
		icSigMediaBlackPointTag = 0x626B7074L,/* ‘bkpt’ */
		icSigMediaWhitePointTag = 0x77747074L,/* ‘wtpt’ */
		icSigNamedColorTag = 0x6E636f6CL,/* ‘ncol’
		* OBSOLETE, use ncl2 */
		icSigPreview0Tag = 0x70726530L,/* ‘pre0’ */
		icSigPreview1Tag = 0x70726531L,/* ‘pre1’ */
		icSigPreview2Tag = 0x70726532L,/* ‘pre2’ */
		icSigProfileDescriptionTag = 0x64657363L,/* ‘desc’ */
		icSigProfileSequenceDescTag = 0x70736571L,/* ‘pseq’ */
		icSigPs2CRD0Tag = 0x70736430L,/* ‘psd0’ */
		icSigPs2CRD1Tag = 0x70736431L,/* ‘psd1’ */
		icSigPs2CRD2Tag = 0x70736432L,/* ‘psd2’ */
		icSigPs2CRD3Tag = 0x70736433L,/* ‘psd3’ */
		icSigPs2CSATag = 0x70733273L,/* ‘ps2s’ */
		icSigPs2RenderingIntentTag = 0x70733269L,/* ‘ps2i’ */
		icSigRedColorantTag = 0x7258595AL,/* ‘rXYZ’ */
		icSigRedTRCTag = 0x72545243L,/* ‘rTRC’ */
		icSigScreeningDescTag = 0x73637264L,/* ‘scrd’ */
		icSigScreeningTag = 0x7363726EL,/* ‘scrn’ */
		icSigTechnologyTag = 0x74656368L,/* ‘tech’ */
		icSigUcrBgTag = 0x62666420L,/* ‘bfd ‘ */
		icSigViewingCondDescTag = 0x76756564L,/* ‘vued’ */
		icSigViewingConditionsTag = 0x76696577L,/* ‘view’ */
		icSigNamedColor2Tag = 0x6E636C32L,/* ‘ncl2’ */
		icMaxEnumTag = 0xFFFFFFFFL/* enum = 4 bytes max */
	} icTagSignature;

	typedef enum {
		icSigDigitalCamera = 0x6463616DL,/* ‘dcam’ */
		icSigFilmScanner = 0x6673636EL,/* ‘fscn’ */
		icSigReflectiveScanner = 0x7273636EL,/* ‘rscn’ */
		icSigInkJetPrinter = 0x696A6574L,/* ‘ijet’ */
		icSigThermalWaxPrinter = 0x74776178L,/* ‘twax’ */
		icSigElectrophotographicPrinter = 0x6570686FL,/* ‘epho’ */
		icSigElectrostaticPrinter = 0x65737461L,/* ‘esta’ */
		icSigDyeSublimationPrinter = 0x64737562L,/* ‘dsub’ */
		icSigPhotographicPaperPrinter = 0x7270686FL,/* ‘rpho’ */
		icSigFilmWriter = 0x6670726EL,/* ‘fprn’ */
		icSigVideoMonitor = 0x7669646DL,/* ‘vidm’ */
		icSigVideoCamera = 0x76696463L,/* ‘vidc’ */
		icSigProjectionTelevision = 0x706A7476L,/* ‘pjtv’ */
		icSigCRTDisplay = 0x43525420L,/* ‘CRT ‘ */
		icSigPMDisplay = 0x504D4420L,/* ‘PMD ‘ */
		icSigAMDisplay = 0x414D4420L,/* ‘AMD ‘ */
		icSigPhotoCD = 0x4B504344L,/* ‘KPCD’ */
		icSigPhotoImageSetter = 0x696D6773L,/* ‘imgs’ */
		icSigGravure = 0x67726176L,/* ‘grav’ */
		icSigOffsetLithography = 0x6F666673L,/* ‘offs’ */
		icSigSilkscreen = 0x73696C6BL, /* ‘silk’ */
		icSigFlexography = 0x666C6578L,/* ‘flex’ */
		icMaxEnumTechnology = 0xFFFFFFFFL/* enum = 4 bytes max */
	} icTechnologySignature;
	/* type signatures */
	typedef enum {
		icSigCurveType = 0x63757276L,/* ‘curv’ */
		icSigDataType = 0x64617461L,/* ‘data’ */
		icSigDateTimeType = 0x6474696DL,/* ‘dtim’ */
		icSigLut16Type = 0x6d667432L,/* ‘mft2’ */
		icSigLut8Type = 0x6d667431L,/* ‘mft1’ */
		icSigMeasurementType = 0x6D656173L,/* ‘meas’ */
		icSigNamedColorType = 0x6E636f6CL,/* ‘ncol’
		* OBSOLETE, use ncl2 */
		icSigParaType = 0x70617261L,/* ‘para’ */
		icSigProfileSequenceDescType = 0x70736571L,/* ‘pseq’ */
		icSigS15Fixed16ArrayType = 0x73663332L,/* ‘sf32’ */
		icSigScreeningType = 0x7363726EL,/* ‘scrn’ */
		icSigSignatureType = 0x73696720L,/* ‘sig ‘ */
		icSigTextType = 0x74657874L,/* ‘text’ */
		icSigTextDescriptionType = 0x64657363L,/* ‘desc’ */
		icSigU16Fixed16ArrayType = 0x75663332L,/* ‘uf32’ */
		icSigUcrBgType = 0x62666420L,/* ‘bfd ‘ */
		icSigUInt16ArrayType = 0x75693136L,/* ‘ui16’ */
		icSigUInt32ArrayType = 0x75693332L,/* ‘ui32’ */
		icSigUInt64ArrayType = 0x75693634L,/* ‘ui64’ */
		icSigUInt8ArrayType = 0x75693038L,/* ‘ui08’ */
		icSigViewingConditionsType = 0x76696577L,/* ‘view’ */
		icSigXYZType = 0x58595A20L,/* ‘XYZ ‘ */
		icSigXYZArrayType = 0x58595A20L,/* ‘XYZ ‘ */
		icSigNamedColor2Type = 0x6E636C32L,/* ‘ncl2’ */
		icMaxEnumType = 0xFFFFFFFFL/* enum = 4 bytes max */
	} icTagTypeSignature;

	/*
	 * Color Space Signatures
	 * Note that only icSigXYZData and icSigLabData are valid
	 * Profile Connection Spaces (PCSs)
	 */
	typedef enum {
		icSigXYZData = 0x58595A20L,/* ‘XYZ ‘ */
		icSigLabData = 0x4C616220L,/* ‘Lab ‘ */
		icSigLuvData = 0x4C757620L,/* ‘Luv ‘ */
		icSigYCbCrData = 0x59436272L,/* ‘YCbr’ */
		icSigYxyData = 0x59787920L,/* ‘Yxy ‘ */
		icSigRgbData = 0x52474220L,/* ‘RGB ‘ */
		icSigGrayData = 0x47524159L,/* ‘GRAY’ */
		icSigHsvData = 0x48535620L,/* ‘HSV ‘ */
		icSigHlsData = 0x484C5320L,/* ‘HLS ‘ */
		icSigCmykData = 0x434D594BL,/* ‘CMYK’ */
		icSigCmyData = 0x434D5920L,/* ‘CMY ‘ */
		icMaxEnumData = 0xFFFFFFFFL/* enum = 4 bytes max */
	} icColorSpaceSignature;
	/* profileClass enumerations */
	typedef enum {
		icSigInputClass = 0x73636E72L,/* ‘scnr’ */
		icSigDisplayClass = 0x6D6E7472L,/* ‘mntr’ */
		icSigOutputClass = 0x70727472L,/* ‘prtr’ */
		icSigLinkClass = 0x6C696E6BL,/* ‘link’ */
		icSigAbstractClass = 0x61627374L,/* ‘abst’ */
		icSigColorSpaceClass = 0x73706163L,/* ‘spac’ */
		icSigNamedColorClass = 0x6e6d636cL,/* ‘nmcl’ */
		icMaxEnumClass = 0xFFFFFFFFL/* enum = 4 bytes max */
	} icProfileClassSignature;
	/* Platform Signatures */
	typedef enum {
		icSigMacintosh = 0x4150504CL,/* ‘APPL’ */
		icSigMicrosoft = 0x4D534654L,/* ‘MSFT’ */
		icSigSolaris = 0x53554E57L,/* ‘SUNW’ */
		icSigSGI = 0x53474920L,/* ‘SGI ‘ */
		icSigTaligent = 0x54474E54L,/* ‘TGNT’ */
		icMaxEnumPlatform = 0xFFFFFFFFL/* enum = 4 bytes max */
	} icPlatformSignature;

	/*------------------------------------------------------------------------*/
	/*
	 * Other enums
	 */
	/* Measurement Flare, used in the measurmentType tag */
	typedef enum {
		icFlare0 = 0x00000000L,/* 0% flare */
		icFlare100 = 0x00000001L,/* 100% flare */
		icMaxFlare = 0xFFFFFFFFL/* enum = 4 bytes max */
	} icMeasurementFlare;
	/* Measurement Geometry, used in the measurmentType tag */
	typedef enum {
		icGeometryUnknown = 0x00000000L,/* Unknown geometry */
		icGeometry045or450 = 0x00000001L,/* 0/45 or 45/0 */
		icGeometry0dord0 = 0x00000002L,/* 0/d or d/0 */
		icMaxGeometry = 0xFFFFFFFFL/* enum = 4 bytes max */
	} icMeasurementGeometry;
	/* Rendering Intents, used in the profile header */
	typedef enum {
		icPerceptual = 0,
		icRelativeColorimetric = 1,
		icSaturation = 2,
		icAbsoluteColorimetric = 3,
		icMaxEnumIntent = 0xFFFFFFFFL/* enum = 4 bytes max */
	} icRenderingIntent;
	/* Different Spot Shapes currently defined, used for screeningType */
	typedef enum {
		icSpotShapeUnknown = 0,
		icSpotShapePrinterDefault = 1,
		icSpotShapeRound = 2,
		icSpotShapeDiamond = 3,
		icSpotShapeEllipse = 4,
		icSpotShapeLine = 5,
		icSpotShapeSquare = 6,
		icSpotShapeCross = 7,
		icMaxEnumSpot = 0xFFFFFFFFL/* enum = 4 bytes max */
	} icSpotShape;
	/* Standard Observer, used in the measurmentType tag */
	typedef enum {
		icStdObsUnknown = 0x00000000L,/* Unknown observer */
		icStdObs1931TwoDegrees = 0x00000001L,/* 1931 two degrees */
		icStdObs1964TenDegrees = 0x00000002L,/* 1961 ten degrees */
		icMaxStdObs = 0xFFFFFFFFL/* enum = 4 bytes max */
	} icStandardObserver;
	/* Pre-defined illuminants, used in measurement and viewing conditions type */
	typedef enum {
		icIlluminantUnknown = 0x00000000L,
		icIlluminantD50 = 0x00000001L,
		icIlluminantD65 = 0x00000002L,
		icIlluminantD93 = 0x00000003L,
		icIlluminantF2 = 0x00000004L,
		icIlluminantD55 = 0x00000005L,
		icIlluminantA = 0x00000006L,
		icIlluminantEquiPowerE = 0x00000007L,/* Equi-Power (E) */
		icIlluminantF8 = 0x00000008L,
		icMaxEnumIluminant = 0xFFFFFFFFL/* enum = 4 bytes max */
	} icIlluminant;
	/*------------------------------------------------------------------------*/
	/*
	 * Arrays of numbers
	 */
	/* Int8 Array */
	typedef struct {
		int8_t data[icAny];/* Variable array of values */
	} icInt8Array;
	/* UInt8 Array */
	typedef struct {
		uint8_t data[icAny];/* Variable array of values */
	} icUInt8Array;
	/* uInt16 Array */
	typedef struct {
		uint16_t data[icAny];/* Variable array of values */
	} icUInt16Array;
	/* Int16 Array */
	typedef struct {
		int16_t data[icAny];/* Variable array of values */
	} icInt16Array;
	/* uInt32 Array */
	typedef struct {
		uint32_t data[icAny];/* Variable array of values */
	} icUInt32Array;
	/* Int32 Array */
	typedef struct {
		int32_t data[icAny];/* Variable array of values */
	} icInt32Array;
	/* UInt64 Array */
	typedef struct {
		uint64_t data[icAny];/* Variable array of values */
	} icUInt64Array;
	/* Int64 Array */
	typedef struct {
		int64_t data[icAny];/* Variable array of values */
	} icInt64Array;
	/* u16Fixed16 Array */
	typedef struct {
		uint32_t data[icAny];/* Variable array of values */
	} icU16Fixed16Array;
	/* s15Fixed16 Array */
	typedef struct {
		int32_t data[icAny];/* Variable array of values */
	} icS15Fixed16Array;
	/* The base date time number */
	typedef struct {
		uint16_t year;
		uint16_t month;
		uint16_t day;
		uint16_t hours;
		uint16_t minutes;
		uint16_t seconds;
	} icDateTimeNumber;
	/* XYZ Number */
	typedef struct {
		int32_t X;
		int32_t Y;
		int32_t Z;
	} icXYZNumber;
	/* XYZ Array */
	typedef struct {
		icXYZNumber data[icAny];/* Variable array of XYZ numbers */
	} icXYZArray;
	/* Curve */
	typedef struct {
		 uint32_t count; /* Number of entries */
		 uint16_t data[icAny];/* The actual table data, real
		 * number is determined by count
		 * Interpretation depends on how
		 * data is used with a given tag.
		 */
	} icCurve;
	typedef struct {
		uint16_t funcType;  /* Function type */
		uint16_t reserved;  /* Reserved for future use */
		uint32_t parameters[icAny]; /* Variable array of parameter values */
	} icPara;
	/* Data */
	typedef struct {
		 uint32_t dataFlag;/* 0 = ascii, 1 = binary */
		 int8_t data[icAny];/* Data, size determined from tag */
	} icData;
	/* lut16 */
	typedef struct {
		uint8_t inputChan;/* Number of input channels */
		uint8_t outputChan;/* Number of output channels */
		uint8_t clutPoints;/* Number of clutTable grid points */
		int8_t pad; /* Padding for byte alignment */
		int32_t e00; /* e00 in the 3 * 3 */
		int32_t e01; /* e01 in the 3 * 3 */
		int32_t e02; /* e02 in the 3 * 3 */
		int32_t e10; /* e10 in the 3 * 3 */
		int32_t e11; /* e11 in the 3 * 3 */
		int32_t e12; /* e12 in the 3 * 3 */
		int32_t e20; /* e20 in the 3 * 3 */
		int32_t e21; /* e21 in the 3 * 3 */
		int32_t e22; /* e22 in the 3 * 3 */
		uint16_t inputEnt;/* Number of input table entries */
		uint16_t outputEnt;/* Number of output table entries */
		uint16_t data[icAny];/* Data follows see spec for size */
		/*
		 * Data that follows is of this form
		 *
		 * uint16_t inputTable[inputChan][icAny];* The input table
		 * uint16_t clutTable[icAny];* The clut table
		 * uint16_t outputTable[outputChan][icAny];* The output table
		 */
	} icLut16;
	/* lut8, input & output tables are always 256 bytes in length */
	typedef struct {
		uint8_t inputChan;/* Number of input channels */
		uint8_t outputChan;/* Number of output channels */
		uint8_t clutPoints;/* Number of clutTable grid points */
		int8_t pad;
		int32_t e00; /* e00 in the 3 * 3 */
		int32_t e01; /* e01 in the 3 * 3 */
		int32_t e02; /* e02 in the 3 * 3 */
		int32_t e10; /* e10 in the 3 * 3 */
		int32_t e11; /* e11 in the 3 * 3 */
		int32_t e12; /* e12 in the 3 * 3 */
		int32_t e20; /* e20 in the 3 * 3 */
		int32_t e21; /* e21 in the 3 * 3 */
		int32_t e22; /* e22 in the 3 * 3 */
		uint8_t data[icAny];/* Data follows see spec for size */
		/*
		 * Data that follows is of this form
		 *
		 * uint8_t inputTable[inputChan][256];* The input table
		 * uint8_t clutTable[icAny];* The clut table
		 * uint8_t outputTable[outputChan][256];* The output table
		 */
	} icLut8;
	/* Measurement Data */
	typedef struct {
		icStandardObserver stdObserver;/* Standard observer */
		icXYZNumber backing;/* XYZ for backing material */
		icMeasurementGeometry geometry;/* Measurement geometry */
		icMeasurementFlare flare; /* Measurement flare */
		icIlluminant illuminant;/* Illuminant */
	} icMeasurement;
	/* Named color */
	/*
	 * icNamedColor2 takes the place of icNamedColor, approved at the
	 * SIGGRAPH 95, ICC meeting.
	 */
	typedef struct {
		uint32_t vendorFlag;/* Bottom 16 bits for IC use */
		uint32_t count; /* Count of named colors */
		uint32_t nDeviceCoords;/* Number of device coordinates */
		int8_t prefix[32];/* Prefix for each color name */
		int8_t suffix[32];/* Suffix for each color name */
		int8_t data[icAny];/* Named color data follows */
		/*
		 * Data that follows is of this form
		 *
		 * int8_t root1[32];* Root name for first color
		 * uint16_t pcsCoords1[icAny];* PCS coordinates of first color
		 * uint16_t deviceCoords1[icAny];* Device coordinates of first
		color
		 * int8_t root2[32];* Root name for second color
		 * uint16_t pcsCoords2[icAny];* PCS coordinates of first color
		 * uint16_t deviceCoords2[icAny];* Device coordinates of first
		color
		 * :
		 * :
		 * Repeat for name and PCS and device color coordinates up to (count-1)
		 *
		 * NOTES:
		 * PCS and device space can be determined from the header.
		 *
		 * PCS coordinates are icUInt16 numbers and are described in
		 * the ICC spec. Only 16 bit L*a*b* and XYZ are allowed. The number of
		 * coordinates is consistent with the headers PCS.
		 *
		 * Device coordinates are icUInt16 numbers where 0x0000 represents
		 * the minimum value and 0xFFFF represents the maximum value.
		 * If the nDeviceCoords value is 0 this field is not given.
		 */
	} icNamedColor2;
	/* Profile sequence structure */
	typedef struct {
		uint32_t deviceMfg;/* Device Manufacturer */
		uint32_t deviceModel;/* Decvice Model */
		uint64_t attributes;/* Device attributes */
		icTechnologySignature technology;/* Technology signature */
		int8_t data[icAny];/* Descriptions text follows */
		/*
		 * Data that follows is of this form, this is an int8_t
		 * to avoid problems with a compiler generating bad code as
		 * these arrays are variable in length.
		 *
		 * icTextDescription deviceMfgDesc;* Manufacturer text
		 * icTextDescription modelDesc;* Model text
		 */
	} icDescStruct;
	/* Profile sequence description */
	typedef struct {
		uint32_t count; /* Number of descriptions */
		uint8_t data[icAny];/* Array of description struct */
	} icProfileSequenceDesc;
	/* textDescription */
	typedef struct {
		uint32_t count; /* Description length */
		int8_t data[icAny];/* Descriptions follow */
		/*
		 * Data that follows is of this form
		 *
		 * int8_t desc[count]* NULL terminated ascii string
		 * uint32_t ucLangCode;* UniCode language code
		 * uint32_t ucCount;* UniCode description length
		 * int16_t ucDesc[ucCount];* The UniCode description
		 * uint16_t scCode; * ScriptCode code
		 * uint8_t scCount;* ScriptCode count
		 * int8_t scDesc[67];* ScriptCode Description
		 */
	} icTextDescription;
	/* Screening Data */
	typedef struct {
		int32_t frequency;/* Frequency */
		int32_t angle; /* Screen angle */
		icSpotShape spotShape;/* Spot Shape encodings below */
	} icScreeningData;
	typedef struct {
		uint32_t screeningFlag;/* Screening flag */
		uint32_t channels;/* Number of channels */
		icScreeningData data[icAny];/* Array of screening data */
	} icScreening;
	/* Text Data */
	typedef struct {
		int8_t data[icAny];/* Variable array of characters */
	} icText;
	/* Structure describing either a UCR or BG curve */
	typedef struct {
		uint32_t count; /* Curve length */
		uint16_t curve[icAny];/* The array of curve values */
	} icUcrBgCurve;
	/* Under color removal, black generation */
	typedef struct {
		int8_t data[icAny];/* The Ucr BG data */
		/*
		 * Data that follows is of this form, this is a int8_t
		 * to avoid problems with a compiler generating bad code as
		 * these arrays are variable in length.
		 *
		 * icUcrBgCurve ucr; * Ucr curve
		 * icUcrBgCurve bg; * Bg curve
		 * int8_t string; * UcrBg description
		 */
	} icUcrBg;
	/* viewingConditionsType */
	typedef struct {
		icXYZNumber illuminant;/* In candelas per metre sq’d */
		icXYZNumber surround;/* In candelas per metre sq’d */
		icIlluminant stdIluminant;/* See icIlluminant defines */
	} icViewingCondition;
	/*------------------------------------------------------------------------*/
	/*
	 * Tag Type definitions
	 */
	/*
	 * Many of the structures contain variable length arrays. This
	 * is represented by the use of the convention.
	 *
	 * typedata[icAny];
	 */
	/* The base part of each tag */
	typedef struct {
		icTagTypeSignature sig; /* Signature */
		int8_t reserved[4];/* Reserved, set to 0 */
	} icTagBase;
	/* curveType */
	typedef struct {
		icTagBase base; /* Signature, “curv” */
		icCurve curve; /* The curve data */
	} icCurveType;
	/* dataType */
	typedef struct {
		icTagBase base; /* Signature, “data” */
		icData data; /* The data structure */
	} icDataType;
	/* dateTimeType */
	typedef struct {
		icTagBase base; /* Signature, “dtim” */
		icDateTimeNumber date; /* The date */
	} icDateTimeType;
	/* lut16Type */
	typedef struct {
		icTagBase base; /* Signature, “mft2” */
		icLut16 lut; /* Lut16 data */
	} icLut16Type;
	/* lut8Type, input & output tables are always 256 bytes in length */
	typedef struct {
		icTagBase base; /* Signature, “mft1” */
		icLut8 lut; /* Lut8 data */
	} icLut8Type;
	/* Measurement Type */
	typedef struct {
		icTagBase base; /* Signature, “meas” */
		icMeasurement measurement;/* Measurement data */
	} icMeasurementType;
	/* Named color type */
	/* icNamedColor2Type, replaces icNamedColorType */
	typedef struct {
		icTagBase base; /* Signature, “ncl2” */
		icNamedColor2 ncolor; /* Named color data */
	} icNamedColor2Type;
	/* Profile sequence description type */
	typedef struct {
		icTagBase base;/* Signature, “pseq” */
		icProfileSequenceDesc desc;/* The seq description */
	} icProfileSequenceDescType;
	/* textDescriptionType */
	typedef struct {
		icTagBase base;/* Signature, “desc” */
		icTextDescription desc; /* The description */
	} icTextDescriptionType;
	/* s15Fixed16Type */
	typedef struct {
		icTagBase base; /* Signature, “sf32” */
		icS15Fixed16Array data; /* Array of values */
	} icS15Fixed16ArrayType;
	typedef struct {
		icTagBase base; /* Signature, “scrn” */
		icScreening screen; /* Screening structure */
	} icScreeningType;
	/* sigType */
	typedef struct {
		icTagBase base; /* Signature, “sig” */
		uint32_t signature;/* The signature data */
	} icSignatureType;
	/* textType */
	typedef struct {
		icTagBase base; /* Signature, “text” */
		icText data; /* Variable array of characters */
	} icTextType;
	/* u16Fixed16Type */
	typedef struct {
		icTagBase base; /* Signature, “uf32” */
		icU16Fixed16Array data; /* Variable array of values */
	} icU16Fixed16ArrayType;
	/* Under color removal, black generation type */
	typedef struct {
		icTagBase base; /* Signature, “bfd “ */
		icUcrBg data; /* ucrBg structure */
	} icUcrBgType;
	/* uInt16Type */
	typedef struct {
		icTagBase base; /* Signature, “ui16” */
		icUInt16Array data; /* Variable array of values */
	} icUInt16ArrayType;
	/* uInt32Type */
	typedef struct {
		icTagBase base; /* Signature, “ui32” */
		icUInt32Array data; /* Variable array of values */
	} icUInt32ArrayType;
	/* uInt64Type */
	typedef struct {
		icTagBase base; /* Signature, “ui64” */
		icUInt64Array data; /* Variable array of values */
	} icUInt64ArrayType;
	/* uInt8Type */
	typedef struct {
		icTagBase base; /* Signature, “ui08” */
		icUInt8Array data; /* Variable array of values */
	} icUInt8ArrayType;
	/* viewingConditionsType */
	typedef struct {
		icTagBase base; /* Signature, “view” */
		icViewingCondition view; /* Viewing conditions */
	} icViewingConditionType;
	/* XYZ Type */
	typedef struct {
		icTagBase base; /* Signature, “XYZ” */
		icXYZArray data; /* Variable array of XYZ numbers */
	} icXYZType;
	/*------------------------------------------------------------------------*/
	/*
	 * Lists of tags, tags, profile header and profile strcuture
	 */
	/* A tag */
	typedef struct {
		icTagSignature sig; /* The tag signature */
		uint32_t offset; /* Start of tag relative to start of header */
		uint32_t size; /* Size in bytes */
	} icTag;
	/* A Structure that may be used independently for a list of tags */
	typedef struct {
		uint32_t count; /* Number of tags in the profile */
		icTag tags[icAny];/* Variable array of tags */
	} icTagList;
	/* The Profile header */
	typedef struct {
		uint32_t size; /* Profile size in bytes */
		uint32_t cmmId; /* CMM for this profile */
		uint32_t version;/* Format version number */
		icProfileClassSignature deviceClass;/* Type of profile */
		icColorSpaceSignature colorSpace;/* Color space of data */
		icColorSpaceSignature pcs; /* PCS, XYZ or Lab only */
		icDateTimeNumber date; /* Date profile was created */
		uint32_t magic; /* icMagicNumber */
		icPlatformSignature platform;/* Primary Platform */
		uint32_t flags; /* Various bit settings */
		uint32_t manufacturer;/* Device manufacturer */
		uint32_t model; /* Device model number */
		uint64_t attributes;/* Device attributes */
		uint32_t renderingIntent;/* Rendering intent */
		icXYZNumber illuminant;/* Profile illuminant */
		uint32_t creator;/* Profile creator */
		int8_t reserved[44];/* Reserved for future use */
	} icHeader;
	/*
	 * A profile,
	 * we can’t use icTagList here because its not at the end of the structure
	 */
	typedef struct {
		icHeader header; /* The header */
		uint32_t count; /* Number of tags in the profile */
		int8_t data[icAny];/* The tagTable and tagData */
		/*
		 * Data that follows is of the form
		 *
		 * icTag tagTable[icAny];* The tag table
		 * int8_t tagData[icAny];* The tag data
		 */
	} icProfile;
	/*------------------------------------------------------------------------*/
	/* Obsolete entries */
	/* icNamedColor was replaced with icNamedColor2 */
	typedef struct {
		uint32_t vendorFlag;/* Bottom 16 bits for IC use */
		uint32_t count; /* Count of named colors */
		int8_t data[icAny];/* Named color data follows */
		/*
		 * Data that follows is of this form
		 *
		 * int8_t prefix[icAny];* Prefix for the color name, max = 32
		 * int8_t suffix[icAny];* Suffix for the color name, max = 32
		 * int8_t root1[icAny];* Root name for first color, max = 32
		 * int8_t coords1[icAny];* Color coordinates of first color
		 * int8_t root2[icAny];* Root name for first color, max = 32
		 * int8_t coords2[icAny];* Color coordinates of first color
		 * :
		 * :
		 * Repeat for root name and color coordinates up to (count-1)
		 */
	} icNamedColor;
	/* icNamedColorType was replaced by icNamedColor2Type */
	typedef struct {
		icTagBase base; /* Signature, “ncol” */
		icNamedColor ncolor; /* Named color data */
	} icNamedColorType;

}	// namespace sl2
