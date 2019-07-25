#ifndef HELPERS_H
#define HELPERS_H 1
/*

Name: bf_helpers.h
Type: Header
Auth: Mateusz Wojt
Date: 07.07.2013


*/


#ifndef SQR /* Square root */
#define SQR(X) ( (X) * (X) )
#endif

#define S_E			2.718281828459045	/* Euler's number */
#define S_PI		3.141592653589793	/* PI */
#define S_PI_2		1.570796326794896	/* PI/2 */
#define S_2PI		6.283185307179586	/* 2*PI */
#define S_1_PI		0.318309886183790	/* 1/PI */
#define S_PHI		1.618033988749894	/* Golden ratio */
#define S_SIN36		0.587785252292473	/* sin(radians(36)) */
#define S_SIN72		0.951056516295153	/* sin(radians(72)) */
#define S_COS36		0.809016994374947	/* cos(radians(36)) */
#define S_COS72		0.309016994374947	/* cos(radians(72)) */
#define S_SQRT2		1.414213562373095	/* sqrt(2) */
#define S_SQRT3		1.732050807568877	/* sqrt(3) */

#define EPSILON		1.0e-6

#ifndef AIR
#define AIR 1.000293 /* IOR value for air */
#endif

#ifndef MINFILTWIDTH
#  define MINFILTWIDTH 1.0e-6
#endif


/* The filterwidth macro takes a float and returns the approximate
 * amount that the float changes from pixel to adjacent pixel.
 */
#define filterwidth(x)  max (abs(Du(x)*du) + abs(Dv(x)*dv), MINFILTWIDTH)

/* The filterwidthp macro is similar to filterwidth, but is for
 * point data.
 */
#define filterwidthp(p) max (sqrt(area(p)), MINFILTWIDTH)


/* Given a function g, its known average g_avg, its high frequency
 * feature size, and a filter width, fade to the average as we approach
 * the Nyquist limit.
 */
#define fadeout(g,g_avg,featuresize,fwidth) \
        mix (g, g_avg, smoothstep(.2,.6,fwidth/featuresize))


#define gamma(x, value) pow(x, value) /* Gamma for correction operation */

/* Simple float operations */
float Compress(float x; float lo; float hi;)
{
	return (hi-lo) * x + lo;
}

float Expand(float x; float lo; float hi;)
{
	float retval = 0;
	if (lo == hi)
		retval = x < lo ? 0 : 1;
	else
		retval = (x-lo) / (hi-lo);

	return retval;
}

float Bias(float t; float a;)
{
	return pow(t,-(log(a)/log(2)));
}

float Bias2(float val; float b;)
{
	return (b > 0) ? pow(val,log(b) / log(0.5)): 0;
}

float Gain(float val; float g;)
{
	return 0.5 * ((val < 0.5) ? Bias2(2 * val, 1 - g) : (2 - Bias2(2 - 2 * val, 1 - g)));
}

float Remap(float x; float a1; float b1; float a2; float b2)
{
    return (x*(b2-a2) - a1*b2 + b1*a2) / (b1-a1);
}

/* Gamma adjustments for linear workflow */
color gammaCorrect(color x; float gamma;)
{
	color xGC;

    xGC = color(gamma(comp(x,0), gamma),
                gamma(comp(x,1), gamma),
                gamma(comp(x,2), gamma));

    return xGC;
}

/* Fresnel reflection function (Siegal and Howell, 1981). */
float nfresnel( float cos_theta_r, eta; )
{
	float g2 = eta*eta + (cos_theta_r*cos_theta_r) - 1;
	float A = g2 - cos_theta_r;
	float B = g2 + cos_theta_r;
	float tmp = .5 * ((A*A)/(B*B));
	A = B*cos_theta_r - 1;
	B = A*cos_theta_r + 1;
	float tmp2 = 1 + ((A*A)/(B*B));
	return tmp * tmp2;
}

/* Mario Marengo fast lookup table implementation */
float mm_erf(	float x; )
{
	float ax = abs(x);
	float ret = 0;

	if (ax >= 4.0) {
		ret = 1.0;
	} else {
		ret = spline( "linear", ax/ 4.0,
				0.000000, 0.112463, 0.222703, 0.328627, 0.428392,
				0.520500, 0.603856, 0.677801, 0.742101, 0.796908,
				0.842701, 0.880205, 0.910314, 0.934008, 0.952285,
				0.966105, 0.976348, 0.983790, 0.989091, 0.992790,
				0.995322, 0.997021, 0.998137, 0.998857, 0.999311,
				0.999593, 0.999764, 0.999866, 0.999925, 0.999959,
				0.999978, 0.999988, 0.999994, 0.999997, 0.999998,
				0.999999, 1.000000, 1.000000, 1.000000, 1.000000,
				1.000000 );
	}
	return ret;
}

//
float mm_erfc( float x; ) {
	return 1 - mm_erf(x);
}

// biMix
#define vslBimix(c0,c1,c2,c3,x,y) \
  mix (mix (c2,c0,y), mix (c3,c1,y), x)

color vslColourBias( color bias; color c; )
{
  color cb, w = color(1,1,1);
  cb = c / ((w/max(bias,color(1e-4)) - 2) * (w - c) + w);
  return cb;
}

float vslFloatBias( float bias; float t; )
{
  float tb = t / ((1/max(bias,1e-4) - 2) * (1 - t) + 1);
  return tb;
}

float vslFloatGain( float gg; float t; )
{
  float tg, gain;
  gain = clamp(gg, .0001, .9999);

  if( t < .5 )
  {
    tg = t / ((1/gain - 2) * (1 - 2*t) + 1);
  }
  else
  {
    tg = ((1/gain - 2) * (1 - 2*t) - t) / ((1/gain - 2) * (1 - 2*t) - 1);
  }
  return tg;
}


#endif /* HELPERS_H */
