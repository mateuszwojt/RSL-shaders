#ifndef NOISES_H
#define NOISES_H 1
/*

Name: bf_noises.h
Type: Header
Auth: Mateusz Wojt
Date: 07.09.2013

*/

#include "bf_helpers.h"

#ifndef snoise
/*
 * Signed noise -- the original Perlin kind with range (-1,1) We prefer
 * signed noise to regular noise mostly because its average is zero.
 * We define three simple macros:
 *   snoise(p) - Perlin noise on either a 1-D (float) or 3-D (point) domain.
 *   snoisexy(x,y) - Perlin noise on a 2-D domain.
 *   vsnoise(p) - vector-valued Perlin noise on either 1-D or 3-D domain.
 */
#define snoise(p) (2 * (float noise(p)) - 1)
#define snoisexy(x,y) (2 * (float noise(x,y)) - 1)
#define vsnoise(p) (2 * (vector noise(p)) - 1)
#endif


/* If we know the filter size, we can crudely antialias snoise by fading
 * to its average value at approximately the Nyquist limit.
 */
#define filteredsnoise(p,width) (snoise(p) * (1-smoothstep (0.2,0.75,width)))
#define filteredvsnoise(p,width) (vsnoise(p) * (1-smoothstep (0.2,0.75,width)))

/* fractional Brownian motion
* Inputs:
*    p, filtwidth   position and approximate inter-pixel spacing
*    octaves        max # of octaves to calculate
*    lacunarity     frequency spacing between successive octaves
*    gain           scaling factor between successive octaves
*/
float fBm (point p; float filtwidth;
           uniform float octaves, lacunarity, gain, fourthdimension)
{
    uniform float amp = 1;
    varying point pp = p;
    varying float sum = 0, fw = filtwidth;
    uniform float i;

    for (i = 0;  i < octaves;  i += 1) {
#pragma nolint
        sum += amp * filteredsnoiset (pp, fw, fourthdimension);
        amp *= gain;  pp *= lacunarity;  fw *= lacunarity;
    }
    return sum;
}



#endif /* NOISES_H */
