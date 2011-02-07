/* 
 * File:   noise.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on March 9, 2010, 11:14 AM
 */

#ifndef _NOISE_H
#define	_NOISE_H

#include "ctrl.h"
#include <cmath>

/**
 * Encapsulates functions and algorithms for
 * noise transformation and distortion.
 */
struct cDistortion {
    /// [-inf,+inf] => [0,+1]
    static inline float sig(float x) {
        return (1.0f / (1.0f + (float) exp(-x)));
    }

    /// [-inf,+inf] => [0,+1]
    static inline float sigdbl(float x) {
        return copysign( 1.0f - (float) exp(-x*x), x );
    }

    /// [0,+1] => [0,+1] in n steps
    static inline float posterize(float x, int n) {
        return (floor(n * x) / n);
    }

    /// [0,+inf] => [0,+1], limiting to 1, ~5 => 0.993
    static inline float exposure(float x) {
        return (1.0f - (float) exp(-x));
    }

    /// [-inf,value-e] => 0, [value,+inf] => 1
    static inline float threshold(float x, float value) {
        return ((x < value) ? 0.0f : 1.0f);
    }

    // [-inf,+inf] => [-1,+1] _/\_
    static inline float rampUpDn(float min, float max, float value) {
        float slope = 2.0f / (max - min);
        // Ramp up to 1.
        float y = (value - min) * slope;
        // Ramp down from 1.
        y = (y < 1.0f) ? (y) : (2.0f - y);
        // Limit Bottom -1
        y = (y >= -1.0f) ? (y) : (-1.0f);
        return y;
    }

    // [-inf,+inf]^2 => [-1,+1] _/\_
    static inline float rampPyramid(float minx, float miny, float maxx, float maxy, float valuex, float valuey) {
        float a = rampUpDn(minx, maxx, valuex);
        float b = rampUpDn(miny, maxy, valuey);
        return fmin(a, b);
    }

    /// [0,+1] => [~min(a,b,c,d), ~max(a,b,c,d)]
    static inline float spline4(float x, float p0, float p1, float p2, float p3) {
        // x [0,+1] => u [-1,+2]
        float u = (x * 3.0f - 1.0f);
        float b0 = (u + 0.0f) * (u - 1.0f) * (u - 2.0f) * -0.166666f;
        float b1 = (u + 1.0f) * (u - 1.0f) * (u - 2.0f) * +0.500000f;
        float b2 = (u + 1.0f) * (u + 0.0f) * (u - 2.0f) * -0.500000f;
        float b3 = (u + 1.0f) * (u + 0.0f) * (u - 1.0f) * +0.166666f;
        float y = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
        return y;
    }

    /// [0,1] => [0,1], f(x) = max( (1-abs(x-0.57))^11, min((2.05*x)^12, min(0.61*x+0.197, 0.51) ) )
    static inline float continentalRidge(float x) {
        float a = pow(x*2.05f, 12);//1.1f * x;
        //float b = 0.5f * x + 0.26f;
        float b = 0.61f * x + 0.197f;
        float c = 0.51f;
        float d = pow(1.0f-fabs(x-0.57), 11);
        float h = fmax(d, fmin(a, fmin(b, c)));
        return h;
    }

    static inline void mix4(float alpha, float* zero4fv, float beta, float* one4fv, float* out4fv) {
        out4fv[0] = zero4fv[0] * alpha + one4fv[0] * beta;
        out4fv[1] = zero4fv[1] * alpha + one4fv[1] * beta;
        out4fv[2] = zero4fv[2] * alpha + one4fv[2] * beta;
        out4fv[3] = zero4fv[3] * alpha + one4fv[3] * beta;
    }

    static inline void mix3(float alpha, float* zero3fv, float beta, float* one3fv, float* out3fv) {
        out3fv[0] = zero3fv[0] * alpha + one3fv[0] * beta;
        out3fv[1] = zero3fv[1] * alpha + one3fv[1] * beta;
        out3fv[2] = zero3fv[2] * alpha + one3fv[2] * beta;
    }

    static inline void fade4(float alpha, float* zero4fv, float* one4fv, float* out4fv) {
        mix4(1.0f - alpha, zero4fv, alpha, one4fv, out4fv);
    }

    static inline void fade3(float alpha, float* zero3fv, float* one3fv, float* out3fv) {
        mix3(1.0f - alpha, zero3fv, alpha, one3fv, out3fv);
    }

    static inline float nm_to_R(int nm) {
        if (nm <= 390) return 1.0f;
        if (nm >= 440 && nm <= 510) return 0.0f;
        if (nm >= 580) return 1.0f;
        if (nm < 440) return (float(nm - 390) / float(440 - 390));
        return (float(nm - 510) / float(580 - 510));
    }

    static inline float nm_to_G(int nm) {
        if (nm <= 440) return 0.0f;
        if (nm >= 640) return 1.0f;
        if (nm >= 490 && nm <= 580) return 1.0f;
        if (nm < 490) return (float(nm - 440) / float(490 - 440));
        return (float(nm - 580) / float(640 - 580));
    }

    static inline float nm_to_B(int nm) {
        if (nm < 490) return 1.0f;
        if (nm > 510) return 0.0f;
        return (float(nm - 490) / float(510 - 490));
    }

#if 0
    // Approximate 1/sqrt(x) - put it here for now.
    static inline float InvSqrt(float x) {
        float xhalf = 0.5f * x;
        int i = *(int*) &x;
        i = 0x5f3759df - (i >> 1);
        x = *(float*) &i;
        x = x * (1.5f - xhalf * x * x);
        return x;
    }
#endif
};


/**
 * Encapsulates algorithms for noise generation,
 * noise transformation and general
 * procedural content generation.
 *
 * Noise is the Message.
 */
struct cNoise {
    
    /// Predictable Pseudo Random Number Generator.
    static inline unsigned long nextrand(unsigned long x) {
        return (x * 1103515245 + 12345);
    }

    // N dimensional permutation (determinisic noise).
    // perms[] has to contain the repeated permutation values - spares modulo arithmetic.
    static inline unsigned char permutation1d(unsigned char* perms, unsigned char a) {
        return (perms[a]);
    }

    static inline unsigned char permutation2d(unsigned char* perms, unsigned char a, unsigned char b) {
        return (perms[a+perms[b]]);
    }

    static inline unsigned char permutation3d(unsigned char* perms, unsigned char a, unsigned char b, unsigned char c) {
        return (perms[a+perms[b+perms[c]]]);
    }

    static inline unsigned char permutation4d(unsigned char* perms, unsigned char a, unsigned char b, unsigned char c, unsigned char d) {
        return (perms[a+perms[b+perms[c+perms[d]]]]);
    }


    /// 32 Bit Linear Feedback Shift Register (need to verify).
    static inline unsigned int LFSR32(unsigned int b) {
        unsigned int x32 = b >> 31;
        unsigned int x22 = b >> 21;
        unsigned int x2 = b >> 1;
        unsigned int x1 = b >> 0;
        unsigned int prod = (x32 ^ x22 ^ x2 ^ x1 ^ 1) & 1;
        return (b << 1) | prod;
    }

    /// 16 Bit Linear Feedback Shift Register (need to verify).
    static inline unsigned short LFSR16(unsigned short b) {
        unsigned short x16 = b >> 15;
        unsigned short x14 = b >> 13;
        unsigned short x13 = b >> 12;
        unsigned short x11 = b >> 10;
        unsigned short prod = (x16 ^ x14 ^ x13 ^ x11 ^ 1) & 1;
        return (b << 1) | prod;
    }

    /// 8 Bit Linear Feedback Shift Register.
    static inline unsigned char LFSR8(unsigned char b) {
        unsigned char x8 = b >> 7;
        unsigned char x6 = b >> 5;
        unsigned char x5 = b >> 4;
        unsigned char x4 = b >> 3;
        unsigned char prod = (x8 ^ x6 ^ x5 ^ x4 ^ 1) & 1;
        return (b << 1) | prod;
    }

    /// 4 Bit Linear Feedback Shift Register.
    static inline unsigned char LFSR4(unsigned char b) {
        unsigned char x4 = b >> 3;
        unsigned char x3 = b >> 2;
        unsigned char prod = (x4 ^ x3 ^ 1) & 1;
        return ((b << 1) | prod) & 0x0F;
    }


    /**
     * Calculates a simplex noise value for a 3d voxel or a 2d pixel and 1d time.
     * To produce Cloud/Terrain like textures add several octaves.
     * By holding one or two dimensions fixed you can extract lower dimensional
     * noise.
     *
     * Ref: Simplex-Noise-Demystified (read this)
     * Ref: Simplex Noise by Ken Perlin (not read)
     * Ref: "Improving Noise" by Ken Perlin (16 instead of 12 gradients)
     * I just implemented it adding permutation through LFSR and
     * with calculation of an edge-direction lookup index for a table instead.
     *
     * @param seed an octave unique seed value in [0,255].
     * @return float value in about [-1,+1].
     */
    static inline float simplex3(float x, float y, float z, unsigned char seed = 0) {
        // Gradient Scaling (to fit end result in [-1,+1]).
        const float s = 32;
        // Gradients (first row with redundancy to make it 16).
        static float g[16][3] = {
            {+s,+s, 0}, {-s,+s, 0}, { 0,-s,+s}, { 0,-s,-s}, // redundant entries.
            {+s,+s, 0}, {-s,+s, 0}, {+s,-s, 0}, {-s,-s, 0},
            {+s, 0,+s}, {-s, 0,+s}, {+s, 0,-s}, {-s, 0,-s},
            { 0,+s,+s}, { 0,-s,+s}, { 0,+s,-s}, { 0,-s,-s}
        };
        // 3 times the permutation of values 0 to 255.
        // We use three times instead of two times so we can shift the start.
        // There are originally two times so that perm[x+perm[y]] does not
        // exceed the array bounds and no modulo (or &-ing) neccessary.
        // Permutation through 8-bit LFSR (with full 256 period: 0-255).
        // The LFSR does actually miss the last value - we need to care about.
        static unsigned char perms[3*256];
        static bool init = true;
        if (init) {
            init = false;
            unsigned char b = 131;
            loopi(3*256) {
                if ((i & 255) == 255) {
                    perms[i] = 255;
                } else {
                    perms[i] = b = cNoise::LFSR8(b);
                }
            }
        }
        // Select permutation start point through seed.
        unsigned char* pseed = &perms[seed];

        const float skew = 1.0f / 3.0f;
        const float unsk = 1.0f / 6.0f;

        // Transform orthogonal input to skewed simplex space.
        // Take the floor to get the simplex grid point.
        float offset = (x + y + z) * skew;
        int x_ = ({float f = (x + offset); (f > 0) ? (int) f : (int) (f - 1);});
        int y_ = ({float f = (y + offset); (f > 0) ? (int) f : (int) (f - 1);});
        int z_ = ({float f = (z + offset); (f > 0) ? (int) f : (int) (f - 1);});

        // And transform grid point back to orthogonal space.
        offset = (x_ + y_ + z_) * unsk;
        float x__ = x_ - offset;
        float y__ = y_ - offset;
        float z__ = z_ - offset;

        // Determine relative distance to grid point in orthogonal space.
        float xofs0 = x - x__;
        float yofs0 = y - y__;
        float zofs0 = z - z__;

        // Relative distance relations tell us in which of the 6 tetrahedron we are.
        // Then we need to walk along edges to find the corners.
        // For each of the six (=rows) there are 2 edge direction vectors.
        // These direction vectors are called "left" and "right from here on
        // also they are not neccessaryily the left and right edges.
        int edgedirections[][3] = {
            { 1, 0, 0}, {1, 1, 0}, // x >= y >= z
            { 1, 0, 0}, {1, 0, 1}, // x >= z >= y
            { 0, 0, 0}, {0, 0, 0}, // - (transitivity disallows this)
            { 0, 0, 1}, {1, 0, 1}, // z >= x >= y

            { 0, 1, 0}, {1, 1, 0}, // y >= x >= z
            { 0, 0, 0}, {0, 0, 0}, // - (transitivity disallows this)
            { 0, 1, 0}, {0, 1, 1}, // y >= z >= x
            { 0, 0, 1}, {0, 1, 1}, // z >= y >= x
        };

        // Calculate a tetrahedron index (eight hypothetical, just six meaningful).
        // The bits are shifted one left for 2 vectors on each row (see above).
        int order = 0;
        order |= (xofs0 >= yofs0) ? 0 : 8;
        order |= (xofs0 >= zofs0) ? 0 : 4;
        order |= (yofs0 >= zofs0) ? 0 : 2;

        int* left = edgedirections[order + 0];
        int* right = edgedirections[order + 1];

        // Simplex corner 2 offset ("left")
        float xofs1 = xofs0 - left[0] + 1.0f * unsk;
        float yofs1 = yofs0 - left[1] + 1.0f * unsk;
        float zofs1 = zofs0 - left[2] + 1.0f * unsk;
        // Simplex corner 3 offset ("right")
        float xofs2 = xofs0 - right[0] + 2.0f * unsk;
        float yofs2 = yofs0 - right[1] + 2.0f * unsk;
        float zofs2 = zofs0 - right[2] + 2.0f * unsk;
        // Simplex corner 4 offset ("far")
        float xofs3 = xofs0 - 1 + 3.0f * unsk;
        float yofs3 = yofs0 - 1 + 3.0f * unsk;
        float zofs3 = zofs0 - 1 + 3.0f * unsk;

        // Have the base grid point wrapped inside [0,255].
        int u = x_ & 255;
        int v = y_ & 255;
        int w = z_ & 255;

        // Get gradient index for this permutation - one out of 16 possible.
        // "Base" corner.
        int g0 = permutation3d(pseed, u, v, w) & 15;
        // "Left" corner.
        int g1 = permutation3d(pseed, u + left[0], v + left[1], w + left[2]) & 15;
        // "Right" corner.
        int g2 = permutation3d(pseed, u + right[0], v + right[1], w + right[2]) & 15;
        // "Far" corner.
        int g3 = permutation3d(pseed, u + 1, v + 1, w + 1) & 15;

        // Contribution "attenuation" factors for distances to grid corner points.
        float c0 = 0.6f - (xofs0*xofs0 + yofs0*yofs0 + zofs0*zofs0);
        float c1 = 0.6f - (xofs1*xofs1 + yofs1*yofs1 + zofs1*zofs1);
        float c2 = 0.6f - (xofs2*xofs2 + yofs2*yofs2 + zofs2*zofs2);
        float c3 = 0.6f - (xofs3*xofs3 + yofs3*yofs3 + zofs3*zofs3);

        // Contribution sum of the four corners.
        float sum = 0.0f;

        // base
        if (c0 > 0) {
            float* g_ = g[g0];
            float sqr = c0 * c0;
            sum += sqr * sqr * (g_[0]*xofs0 + g_[1]*yofs0 + g_[2]*zofs0);
        }

        // left
        if (c1 > 0) {
            float* g_ = g[g1];
            float sqr = c1 * c1;
            sum += sqr * sqr * (g_[0]*xofs1 + g_[1]*yofs1 + g_[2]*zofs1);
        }

        // right
        if (c2 > 0) {
            float* g_ = g[g2];
            float sqr = c2 * c2;
            sum += sqr * sqr * (g_[0]*xofs2 + g_[1]*yofs2 + g_[2]*zofs2);
        }

        // far
        if (c3 > 0) {
            float* g_ = g[g3];
            float sqr = c3 * c3;
            sum += sqr * sqr * (g_[0]*xofs3 + g_[1]*yofs3 + g_[2]*zofs3);
        }

        // [-1,+1]
        return sum;
    }

    /**
     * Calculates 2 dimensional tileable voronoi/worley noise with period 256.
     * @param x position to calculate noise for (repetition mod 256).
     * @param y position to calculate noise for (repetition mod 256).
     * @param diffusion within [0,1] determines maximal irregularity of cell cores.
     * @param shift vertical shift within [0,1] of every odd column.
     * @param e inverse exponent for distance calculation (2 == sqrt).
     * @param seed the seed for noise computation.
     * @return [-1,+1] float noise value.
     */
    static inline float voronoi2(float x, float y, float diffusion = 0.5f, float shift = 0.5f, float e = 2.0f, unsigned char seed = 0) {
        // 4 times the permutation of values 0 to 255.
        // We use three times instead of two times so we can shift the start.
        // There are originally two times so that perm[x+perm[y]] does not
        // exceed the array bounds and no modulo (or &-ing) neccessary.
        // Permutation through 8-bit LFSR (with full 256 period: 0-255).
        static unsigned char perms[3*256];
        static bool init = true;
        if (init) {
            init = false;
            unsigned char b = 131;
            loopi(3*256) {
                if ((i & 255) == 255) {
                    perms[i] = 255;
                } else {
                    perms[i] = b = cNoise::LFSR8(b);
                }
            }
        }

        const float range = diffusion / 256.0f;
        const float offset = 0.5 * (1.0-diffusion);

        unsigned char s1 = seed + 1;
        unsigned char s2 = seed + 2;

        int gx = x;
        int gy = y;
        float min_d = INFINITY;
        float min_d_ = min_d;

        loopi(3) {
            int gx_ = gx + i - 1;
            loopj(3) {
                int gy_ = gy + j - 1;

                float x_ = gx_ + offset + range * permutation4d(perms, s1, gx_,gy_,0);
                float y_ = gy_ + offset + range * permutation4d(perms, s2, gx_,gy_,0) + shift * (gx_&1);

                float dx = x - x_;
                float dy = y - y_;
                float d = pow(dx,e) + pow(dy,e);
                if (d <= min_d) {
                    min_d_ = min_d;
                    min_d = d;
                } else if (d <= min_d_) {
                min_d_ = d;
                }
            }
        }
        const float inv_e = 1.0f / e;
        float u = pow(min_d, inv_e);
        float v = pow(min_d_, inv_e);
        //return 1.999999f * (u / (u + v));
        // Result within [-1,+1] because u less or equal v.
        return 4.0f * (u / (u + v)) - 1.0f;
    }

    /**
     * Calculates 3 dimensional tileable voronoi/worley noise with period 256.
     * @param x position to calculate noise for (repetition mod 256).
     * @param y position to calculate noise for (repetition mod 256).
     * @param z position to calculate noise for (repetition mod 256).
     * @param diffusion within [0,1] determines maximal irregularity of cell cores.
     * @param shift vertical shift within [0,1] of every odd column.
     * @param e inverse exponent for distance calculation (2 == sqrt).
     * @param seed the seed for noise computation.
     * @return [-1,+1] float noise value.
     */
    static inline float voronoi3(float x, float y, float z, float diffusion = 0.5f, float shift = 0.5f, float e = 2.0f, unsigned char seed = 0) {
        // 4 times the permutation of values 0 to 255.
        // We use three times instead of two times so we can shift the start.
        // There are originally two times so that perm[x+perm[y]] does not
        // exceed the array bounds and no modulo (or &-ing) neccessary.
        // Permutation through 8-bit LFSR (with full 256 period: 0-255).
        static unsigned char perms[3*256];
        static bool init = true;
        if (init) {
            init = false;
            unsigned char b = 131;
            loopi(3*256) {
                if ((i & 255) == 255) {
                    perms[i] = 255;
                } else {
                    perms[i] = b = cNoise::LFSR8(b);
                }
            }
        }

        const float range = diffusion / 256.0f;
        const float offset = 0.5f * (1.0f-diffusion);

        int gx = x;
        int gy = y;
        int gz = z;
        float min_d = INFINITY;
        float min_d_ = min_d;

        unsigned char s1 = seed + 1;
        unsigned char s2 = seed + 2;
        unsigned char s3 = seed + 3;

        loopi(3) {
            int gx_ = gx + i - 1;
            loopj(3) {
                int gy_ = gy + j - 1;
                loopk(3) {
                    int gz_ = gz + k - 1;

                    float x_ = gx_ + offset + range * permutation4d(perms, s1, gx_,gy_,gz_);
                    float y_ = gy_ + offset + range * permutation4d(perms, s2, gx_,gy_,gz_) + shift * (gx_&1);
                    float z_ = gz_ + offset + range * permutation4d(perms, s3, gx_,gy_,gz_) + shift * (gx_&1);

                    float dx = x - x_;
                    float dy = y - y_;
                    float dz = z - z_;
                    float d = dx*dx + dy*dy + dz*dz;
                    //float d = pow(dx,e) + pow(dy,e) + pow(dz,e);
                    if (d <= min_d) {
                        min_d_ = min_d;
                        min_d = d;
                    } else if (d <= min_d_) {
                        min_d_ = d;
                    }
                }
            }
        }
        //const float e_inv = 1.0f / e;
        //const float u = pow(min_d, e_inv);
        //const float v = pow(min_d_,e_inv);
        const float u = sqrtf(min_d);
        const float v = sqrtf(min_d_);
        // Result within [0,1[ because u less or equal v.
        //return 1.999999f * (u / (u + v));
        // Result within [-1,+1] because u less or equal v.
        return 4.0f * (u / (u + v)) - 1.0f;
    }

    /**
     * Calculates 3 dimensional tileable voronoi/worley noise with period 16.
     * @param x position to calculate noise for (repetition mod 16).
     * @param y position to calculate noise for (repetition mod 16).
     * @param z position to calculate noise for (repetition mod 16).
     * @param diffusion within [0,1] determines maximal irregularity of cell cores.
     * @param shift vertical shift within [0,1] of every odd column.
     * @param e inverse exponent for distance calculation (2 == sqrt).
     * @param seed the seed for noise computation.
     * @return [-1,+1] float noise value.
     */
    static inline float voronoi3_16(float x, float y, float z, float diffusion = 0.5f, float shift = 0.5f, float e = 2.0f, unsigned char seed = 0) {
        static unsigned char perms[256*3];
        static bool init = true;
        if (init) {
            init = false;
            unsigned char b = 131;

            loopi(256*3) {
                if ((i & 15) == 15) {
                    perms[i] = 15;
                } else {
                    perms[i] = b = cNoise::LFSR4(b);
                }
            }
        }

        const float range = diffusion / 16.0f;
        const float offset = 0.5f * (1.0f-diffusion);

        int gx = x;
        int gy = y;
        int gz = z;
        float min_d = INFINITY;
        float min_d_ = min_d;

        unsigned char s1 = seed + 1;
        unsigned char s2 = seed + 2;
        unsigned char s3 = seed + 3;

        loopi(3) {
            int gx_ = gx + i - 1;
            loopj(3) {
                int gy_ = gy + j - 1;
                loopk(3) {
                    int gz_ = gz + k - 1;

                    float x_ = gx_ + offset + range * permutation4d(perms, s1, gx_,gy_,gz_);
                    float y_ = gy_ + offset + range * permutation4d(perms, s2, gx_,gy_,gz_) + shift * (gx_&1);
                    float z_ = gz_ + offset + range * permutation4d(perms, s3, gx_,gy_,gz_) + shift * (gx_&1);

                    float dx = x - x_;
                    float dy = y - y_;
                    float dz = z - z_;
                    float d = pow(dx,e) + pow(dy,e) + pow(dz,e);
                    if (d <= min_d) {
                        min_d_ = min_d;
                        min_d = d;
                    } else if (d <= min_d_) {
                        min_d_ = d;
                    }
                }
            }
        }
        const float e_inv = 1.0f / e;
        const float u = pow(min_d, e_inv);
        const float v = pow(min_d_,e_inv);
        // Result within [0,1[ because u less or equal v.
        //return 1.999999f * (u / (u + v));
        // Result within [-1,+1] because u less or equal v.
        return 4.0f * (u / (u + v)) - 1.0f;
    }

    /**
     * Generates somewhat blocky noise, metal sheet like.
     * @param x position to calculate noise for (repetition mod 256).
     * @param y position to calculate noise for (repetition mod 256).
     * @param z position to calculate noise for (repetition mod 256).
     * @param seed the seed for noise computation.
     * @return [-1,+1] float noise value.
     */
    static inline float samplex3(float x, float y, float z, unsigned char seed) {
        const float tof = 0.0039062f;

        int x_ = (((int)x)^seed) & 255;
        int y_ = (((int)y)^seed) & 255;
        int z_ = (((int)z)^seed) & 255;

        unsigned char samples[8];
        loopi(8) {
            unsigned long r = nextrand(x_) ^ nextrand(y_) ^ nextrand(z_);
            r = nextrand(r);
            r = nextrand(r);
            samples[i] = r;
            x_ >>= 1;
            y_ >>= 1;
            z_ >>= 1;
        }

        float v = 0;
        float f = 1.0f;
        float sumf = 0;
        loopi(8) {
            v += samples[7-i] * f;
            sumf += f;
            f *= 0.95f;
        }
        v = v * tof / sumf * 2.0f - 1.0f;

        return v;
    }

};



#endif	/* _NOISE_H */

