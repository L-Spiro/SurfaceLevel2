#ifndef ISPC_UINT_IS_DEFINED
//these are defined in ISPC version 1.13.0 and later
typedef unsigned int8 uint8;
typedef unsigned int32 uint32;
typedef unsigned int64 uint64;
#endif

export struct ColorRGBA {
    double r, g, b, a;
};

export struct ColorLABA {
    double l, a, b, alpha;
};

// Convert RGB to LAB
export void ispc_rgb2lab(const uniform ColorRGBA rgb[], uniform ColorLABA lab[], uniform uint64 n) {
    foreach (i = 0 ... n) {
        double r = rgb[i].r / 255.0d;
        double g = rgb[i].g / 255.0d;
        double b = rgb[i].b / 255.0d;

        double x = (r * 0.4124d + g * 0.3576d + b * 0.1805d) / 0.95047d;
        double y = (r * 0.2126d + g * 0.7152d + b * 0.0722d) / 1.00000d;
        double z = (r * 0.0193d + g * 0.1192d + b * 0.9505d) / 1.08883d;

        x = (x > 0.008856d) ? pow(x, (1.0d / 3.0d)) : (7.787037037037037037d * x) + 16.0d / 116.0d;
        y = (y > 0.008856d) ? pow(y, (1.0d / 3.0d)) : (7.787037037037037037d * y) + 16.0d / 116.0d;
        z = (z > 0.008856d) ? pow(z, (1.0d / 3.0d)) : (7.787037037037037037d * z) + 16.0d / 116.0d;


        lab[i].l = (116.0d * y) - 16.0d;
        lab[i].a = 500.0d * (x - y);
        lab[i].b = 200.0d * (y - z);
        lab[i].alpha = rgb[i].a; // Preserve alpha channel
    }
}

// Convert LAB to RGB
export void ispc_lab2rgb(const uniform ColorLABA lab[], uniform ColorRGBA rgb[], uniform uint64 n) {
    foreach (i = 0 ... n) {
        double y = (lab[i].l + 16.0d) / 116.0d;
        double x = lab[i].a / 500.0d + y;
        double z = y - lab[i].b / 200.0d;

        x = 0.95047d * ((x * x * x > 0.008856d) ? x * x * x : (x - 16.0d / 116.0d) / 7.787037037037037037d);
        y = 1.00000d * ((y * y * y > 0.008856d) ? y * y * y : (y - 16.0d / 116.0d) / 7.787037037037037037d);
        z = 1.08883d * ((z * z * z > 0.008856d) ? z * z * z : (z - 16.0d / 116.0d) / 7.787037037037037037d);

        double r = x * 3.240625477069124789d + y * -1.537207972210696894d + z * -0.498628598698778035d;
        double g = x * -0.968913997741625519d + y * 1.875880503887304647d + z * 0.041503284783317432d;
        double b = x * 0.055710120445510620d + y * -0.204022945751467327d + z * 1.056995942254388236d;

        rgb[i].r = clamp(r, 0.0d, 1.0d) * 255.0d;
        rgb[i].g = clamp(g, 0.0d, 1.0d) * 255.0d;
        rgb[i].b = clamp(b, 0.0d, 1.0d) * 255.0d;
        rgb[i].a = lab[i].alpha; // Preserve alpha channel
    }
}

// Calculate Delta E between two LAB colors (CIE94).
export uniform double ispc_deltaE_CIE94(const uniform ColorLABA &labA, const uniform ColorLABA &labB) {
    uniform double deltaL = labA.l - labB.l;
    uniform double deltaA = labA.a - labB.a;
    uniform double deltaB = labA.b - labB.b;
    
    uniform double c1 = sqrt(labA.a * labA.a + labA.b * labA.b);
    uniform double c2 = sqrt(labB.a * labB.b + labB.b * labB.b);
    uniform double deltaC = c1 - c2;
    
    varying double deltaH2 = deltaA * deltaA + deltaB * deltaB - deltaC * deltaC;
    deltaH2 = max(0.0d, deltaH2);
    
    uniform double sc = 1.0d + 0.045d * c1;
    uniform double sh = 1.0d + 0.015d * c1;
    
    varying double deltaLKlsl = deltaL / 1.0d;
    varying double deltaCkcsc = deltaC / sc;
    varying double deltaHkhsh = sqrt(deltaH2) / sh;
    
    varying double i = deltaLKlsl * deltaLKlsl + deltaCkcsc * deltaCkcsc + deltaHkhsh * deltaHkhsh;
    
    return reduce_add(sqrt(i)) / programCount;  // Aggregate results across all program instances
}
export uniform double ispc_deltaE_CIE94_textiles(const uniform ColorLABA &labA, const uniform ColorLABA &labB) {
    uniform double deltaL = labA.l - labB.l;
    uniform double deltaA = labA.a - labB.a;
    uniform double deltaB = labA.b - labB.b;
    
    uniform double c1 = sqrt(labA.a * labA.a + labA.b * labA.b);
    uniform double c2 = sqrt(labB.a * labB.b + labB.b * labB.b);
    uniform double deltaC = c1 - c2;
    
    varying double deltaH2 = deltaA * deltaA + deltaB * deltaB - deltaC * deltaC;
    deltaH2 = max(0.0d, deltaH2);
    
    uniform double sc = 1.0d + 0.048d * c1;
    uniform double sh = 1.0d + 0.014d * c1;
    
    varying double deltaLKlsl = deltaL / 2.0d;
    varying double deltaCkcsc = deltaC / sc;
    varying double deltaHkhsh = sqrt(deltaH2) / sh;
    
    varying double i = deltaLKlsl * deltaLKlsl + deltaCkcsc * deltaCkcsc + deltaHkhsh * deltaHkhsh;
    
    return reduce_add(sqrt(i)) / programCount;  // Aggregate results across all program instances
}

export struct ColorLabAlpha {
    double L;
    double a;
    double b;
    double alpha;
};

#define M_PI 3.14159265358979323846264338327950288419716939937510d

export uniform double ispc_deltaE_CIEDE2000(
    uniform double L1, uniform double a1, uniform double b1, uniform double alpha1,
    uniform double L2, uniform double a2, uniform double b2, uniform double alpha2) {
    
    // Constants
    uniform double kL = 1.0d, kC = 1.0d, kH = 1.0d;
    uniform double kA = 1.0d; // Weight for alpha channel

    // Step 1: Calculate C1, C2, and average C
    uniform double C1 = sqrt(a1 * a1 + b1 * b1);
    uniform double C2 = sqrt(a2 * a2 + b2 * b2);
    uniform double C_bar = (C1 + C2) / 2.0d;

    // Step 2: Calculate G and adjusted a1_prime, a2_prime
    uniform double G = 0.5d * (1.0d - sqrt(pow(C_bar, 7.0d) / (pow(C_bar, 7.0d) + pow(25.0d, 7.0d))));
    uniform double a1_prime = (1.0d + G) * a1;
    uniform double a2_prime = (1.0d + G) * a2;

    // Step 3: Calculate C1_prime, C2_prime, and average C_prime
    uniform double C1_prime = sqrt(a1_prime * a1_prime + b1 * b1);
    uniform double C2_prime = sqrt(a2_prime * a2_prime + b2 * b2);
    uniform double C_bar_prime = (C1_prime + C2_prime) / 2.0d;

    // Step 4: Calculate h1_prime, h2_prime, and average H_bar_prime
    uniform double h1_prime = atan2(b1, a1_prime);
    if (h1_prime < 0.0d) h1_prime += 2.0d * M_PI;
    uniform double h2_prime = atan2(b2, a2_prime);
    if (h2_prime < 0.0d) h2_prime += 2.0d * M_PI;

    uniform double H_bar_prime = (abs(h1_prime - h2_prime) > M_PI) ? 
                                 (h1_prime + h2_prime + 2.0d * M_PI) / 2.0d : 
                                 (h1_prime + h2_prime) / 2.0d;

    // Step 5: Calculate T
    uniform double T = 1.0d - 0.17d * cos(H_bar_prime - M_PI / 6.0d) + 
                       0.24d * cos(2.0d * H_bar_prime) + 
                       0.32d * cos(3.0d * H_bar_prime + M_PI / 30.0d) - 
                       0.20d * cos(4.0d * H_bar_prime - M_PI / 3.0d);

    // Step 6: Calculate delta_h_prime
    uniform double delta_h_prime = h2_prime - h1_prime;
    if (abs(delta_h_prime) > M_PI) {
        delta_h_prime -= (delta_h_prime > 0.0d) ? 2.0d * M_PI : -2.0d * M_PI;
    }
    uniform double delta_H_prime = 2.0d * sqrt(C1_prime * C2_prime) * sin(delta_h_prime / 2.0d);

    // Step 7: Calculate delta_L_prime, delta_C_prime, SL, SC, SH
    uniform double delta_L_prime = L2 - L1;
    uniform double delta_C_prime = C2_prime - C1_prime;
    uniform double SL = 1.0d + (0.015d * pow(L1 - 50.0d, 2)) / sqrt(20.0d + pow(L1 - 50.0d, 2));
    uniform double SC = 1.0d + 0.045d * C_bar_prime;
    uniform double SH = 1.0d + 0.015d * C_bar_prime * T;

    // Step 8: Calculate delta_alpha_prime (difference in alpha)
    uniform double delta_alpha_prime = alpha2 - alpha1;

    // Step 9: Calculate RT
    uniform double delta_theta = M_PI / 6.0d * exp(-pow((H_bar_prime - 275.0d * M_PI / 180.0d) / (25.0d * M_PI / 180.0d), 2));
    uniform double RC = 2.0d * sqrt(pow(C_bar_prime, 7.0d) / (pow(C_bar_prime, 7.0d) + pow(25.0d, 7.0d)));
    uniform double RT = -RC * sin(2.0d * delta_theta);

    // Step 10: Calculate the final delta E 2000
    return sqrt(pow(delta_L_prime / (kL * SL), 2.0d) + 
                  pow(delta_C_prime / (kC * SC), 2.0d) + 
                  pow(delta_H_prime / (kH * SH), 2.0d) + 
                  pow(delta_alpha_prime / kA, 2.0d) + 
                  RT * (delta_C_prime / (kC * SC)) * (delta_H_prime / (kH * SH)));
}







export struct Color {
    double r, g, b, a;
};

export struct ColorCube {
    int64 start, end;  // Indexes into the color array
    double rMin, rMax, gMin, gMax, bMin, bMax, aMin, aMax;
};

// Manual sorting function
inline void sortColors(uniform Color colors[], uniform int start, uniform int end, int channel) {
    for (uniform int i = start; i < end - 1; i++) {
        for (uniform int j = i + 1; j < end; j++) {
            uniform bool condition = false;
            if (channel == 0)      condition = colors[i].r > colors[j].r;
            else if (channel == 1) condition = colors[i].g > colors[j].g;
            else if (channel == 2) condition = colors[i].b > colors[j].b;
            else if (channel == 3) condition = colors[i].a > colors[j].a;

            if (condition) {
                uniform Color temp = colors[i];
                colors[i] = colors[j];
                colors[j] = temp;
            }
        }
    }
}



inline void splitCube(uniform Color colors[], uniform ColorCube cubes[], uniform int cubeIndex, uniform int &cubeCount) {
    uniform ColorCube cube = cubes[cubeIndex];
    cubeCount--;

    double rRange = cube.rMax - cube.rMin;
    double gRange = cube.gMax - cube.gMin;
    double bRange = cube.bMax - cube.bMin;
    double aRange = cube.aMax - cube.aMin;

    int channel = 0;
    if (gRange >= rRange && gRange >= bRange && gRange >= aRange) {
        channel = 1;
    } else if (bRange >= rRange && bRange >= gRange && bRange >= aRange) {
        channel = 2;
    } else if (aRange >= rRange && aRange >= gRange && aRange >= bRange) {
        channel = 3;
    }

    sortColors(colors, cube.start, cube.end, channel);

    uniform int median = (cube.start + cube.end) / 2;

    uniform ColorCube cube1, cube2;
    cube1.start = cube.start;
    cube1.end = median;
    cube1.rMin = 1.0d;
    cube1.rMax = 0.0d;
    cube1.gMin = 1.0d;
    cube1.gMax = 0.0d;
    cube1.bMin = 1.0d;
    cube1.bMax = 0.0d;
    cube1.aMin = 1.0d;
    cube1.aMax = 0.0d;

    cube2.start = median;
    cube2.end = cube.end;
    cube2.rMin = 1.0d;
    cube2.rMax = 0.0d;
    cube2.gMin = 1.0d;
    cube2.gMax = 0.0d;
    cube2.bMin = 1.0d;
    cube2.bMax = 0.0d;
    cube2.aMin = 1.0d;
    cube2.aMax = 0.0d;

    // Update color bounds for cube1
    for (uniform int c = cube1.start; c < cube1.end; ++c) {
        cube1.rMin = min(cube1.rMin, colors[c].r);
        cube1.rMax = max(cube1.rMax, colors[c].r);
        cube1.gMin = min(cube1.gMin, colors[c].g);
        cube1.gMax = max(cube1.gMax, colors[c].g);
        cube1.bMin = min(cube1.bMin, colors[c].b);
        cube1.bMax = max(cube1.bMax, colors[c].b);
        cube1.aMin = min(cube1.aMin, colors[c].a);
        cube1.aMax = max(cube1.aMax, colors[c].a);
    }

    // Update color bounds for cube2
    for (uniform int c = cube2.start; c < cube2.end; ++c) {
        cube2.rMin = min(cube2.rMin, colors[c].r);
        cube2.rMax = max(cube2.rMax, colors[c].r);
        cube2.gMin = min(cube2.gMin, colors[c].g);
        cube2.gMax = max(cube2.gMax, colors[c].g);
        cube2.bMin = min(cube2.bMin, colors[c].b);
        cube2.bMax = max(cube2.bMax, colors[c].b);
        cube2.aMin = min(cube2.aMin, colors[c].a);
        cube2.aMax = max(cube2.aMax, colors[c].a);
    }

    cubes[cubeCount++] = cube1;
    cubes[cubeCount++] = cube2;
}


export void ispc_medianCutQuantization(uniform Color colors[], uniform int64 imageSize, uniform Color palette[], uniform int paletteSize) {
    uniform ColorCube cubes[0x10000];
    uniform int cubeCount = 0;

    // Initialize the initialCube manually
    uniform ColorCube initialCube;
    initialCube.start = 0;
    initialCube.end = imageSize;
    initialCube.rMin = 1.0d;
    initialCube.rMax = 0.0d;
    initialCube.gMin = 1.0d;
    initialCube.gMax = 0.0d;
    initialCube.bMin = 1.0d;
    initialCube.bMax = 0.0d;
    initialCube.aMin = 1.0d;
    initialCube.aMax = 0.0d;

    for (uniform int c = initialCube.start; c < initialCube.end; ++c) {
        initialCube.rMin = min(initialCube.rMin, colors[c].r);
        initialCube.rMax = max(initialCube.rMax, colors[c].r);
        initialCube.gMin = min(initialCube.gMin, colors[c].g);
        initialCube.gMax = max(initialCube.gMax, colors[c].g);
        initialCube.bMin = min(initialCube.bMin, colors[c].b);
        initialCube.bMax = max(initialCube.bMax, colors[c].b);
        initialCube.aMin = min(initialCube.aMin, colors[c].a);
        initialCube.aMax = max(initialCube.aMax, colors[c].a);
    }

    cubes[cubeCount++] = initialCube;

    while (cubeCount < paletteSize) {
        splitCube(colors, cubes, cubeCount - 1, cubeCount);
    }

    for (uniform int i = 0; i < paletteSize; ++i) {
        uniform ColorCube cube = cubes[i];
        varying double rSum = 0.0d, gSum = 0.0d, bSum = 0.0d, aSum = 0.0d;

        foreach (c = cube.start ... cube.end) {
            rSum += colors[c].r;
            gSum += colors[c].g;
            bSum += colors[c].b;
            aSum += colors[c].a;
        }

        uniform double rSumUniform = reduce_add(rSum);
        uniform double gSumUniform = reduce_add(gSum);
        uniform double bSumUniform = reduce_add(bSum);
        uniform double aSumUniform = reduce_add(aSum);

        uniform double colorCountUniform = (double)(cube.end - cube.start);

        palette[i].r = rSumUniform / colorCountUniform;
        palette[i].g = gSumUniform / colorCountUniform;
        palette[i].b = bSumUniform / colorCountUniform;
        palette[i].a = aSumUniform / colorCountUniform;
    }
}

