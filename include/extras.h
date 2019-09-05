#ifndef EXTRAS_H
#define EXTRAS_H 1
/* 

Name: extras.h
Type: Header
Auth: Mateusz Wojt
Date: 08.07.2013


*/

#include "helpers.h"

// Beckmann distribution
float beckmann
(
	float cosalpha, roughness;
)
{
	float m2 = SQR(roughness);
	float cosalpha2 = SQR(cosalpha);
	float tanalpha2_over_m2 = max(0, 1 - cosalpha2) / (cosalpha * m2);
	return exp(-tanalpha2_over_m2) / (m2 * SQR(cosalpha2));
}

// Ward distribution
float ward
(
	float cosalpha, roughness;
)
{
	float m2 = SQR(roughness);
	float cosalpha2 = SQR(cosalpha);
	float tanalpha2_over_m2 = max(0, 1 - cosalpha2) / (cosalpha * m2);
	return exp(-tanalpha2_over_m2) / (m2 * S_PI * cosalpha2 * cosalpha);
}

// Trowbridge-Reitz distribution
float trowbridge_reitz
(
	float cosalpha, roughness;
)
{
	float m2 = SQR(roughness);
	float cosalpha2 = SQR(cosalpha);
	float d2 = m2 / (cosalpha * (m2-1)+1);
	return SQR(d2);
}

// Heidrich-Seidel distribution
float heidrich_seidel
(
	normal Nf;
	vector Vf, Ln;
	vector xdir;
	float roughness;
)
{
	vector dir = normalize(Nf ^ xdir);
	float cosbeta = Ln.dir;
	float costheta = Vf.dir;
	float sinbeta = sqrt(max(0, 1 - SQR(cosbeta)));
	float sintheta = sqrt(max(0, 1 - SQR(costheta)));
	return pow(sinbeta * sintheta - (cosbeta * costheta), 1 / roughness);
}

float blinn_ts
(
	float costheta, cosalpha, cospsi, cospsi2;
)
{
	float shadowing = (cosalpha * costheta) / cospsi2;
	float masking = (cosalpha * cospsi) / cospsi2;
	
	return min(1, 2 * min(shadowing, masking));
}

// Schlick-Smith approximation
float schlick_smith
(
	float cospsi, costheta, roughness;
)
{
	float g1 = cospsi / (roughness + (1 - roughness) * cospsi);
	float g2 = costheta / (roughness + (1 - roughness) * costheta);
	return g1 * g2;
}

// Smith equation
float smith(
			float costheta, cosalpha, roughness;
			)
{
	float cosalpha2 = SQR(cosalpha);
	float costheta2 = SQR(costheta);
	float m2 = SQR(roughness);
	
	float hnl = cosalpha2 / (2 * m2 * (1 - cosalpha2));
	float hnv = costheta2 / (2 * m2 * (1 - costheta2));
	
	float gnl = sqrt(S_PI * hnl * (2 - mm_erfc(sqrt(hnl))));
	float gnv = sqrt(S_PI * hnv * (2 - mm_erfc(sqrt(hnv))));
	
	return (gnl / (gnl + 1)) * (gnv / (gnv + 1));
}

// Jerry Tessendorf's Fresnel function from Lollipop Shaders
float JTessendorfFresnel(vector I; normal N; float nSnell)
{
               float reflectivity;
               vector In = normalize(I);
               normal Nn = normalize(N);
               float costhetai = abs(In . Nn);
               float thetai = acos(costhetai);
               float sinthetat = sin(thetai)/nSnell;
               float thetat = asin(sinthetat);

               if (thetai == 0.0)
           {
                   reflectivity = (nSnell - 1)/(nSnell + 1);
                   reflectivity = reflectivity * reflectivity;
               } else {
                   float fs = sin(thetat - thetai) / sin(thetat + thetai);
                   float ts = tan(thetat - thetai) / tan(thetat + thetai);
                   reflectivity = 0.5 * ( fs*fs + ts*ts );
               }

           return reflectivity;
}

// Reflection parameters calculation from Brian Meanleys' bhm_refl.h
color rflCalc(vector Nf; 
			string envMap;
			float samples, blur, rflMode, envOcc;)
{
    extern vector I;
    color Cr, CrEnv, CrRT;
    float rlo;

    normalize(Nf);

    if( rflMode != 0 )
    {
        vector R = reflect(I, Nf);
        vector Rshad = reflect(I, Nf);
        if( envMap != "raytrace" )
        {
            R = vtransform( "world", R);
        }
        else
        {
            R = R;
        }
        if( rflMode == 1 && envMap != "" )
        {
            Cr = color environment(
                        envMap, R, "samples", samples, "blur", blur );
            if(envOcc != 0)
            {
                /* This hack is used to occlude some of the image based
                * reflections where light does not hit the surface. This
                * will scale the environment based reflections based on
                * the ammount of light hitting the surface (diffuse).
                * No light = No environmental reflection.  This is in place
                * of true reflection occlusion (cheap!)
                */
                float rOccScale = comp(diffuse(normalize(Nf)), 1);
                float rflOcc = Compress(rOccScale, (1 - envOcc), 1);
                Cr *= rflOcc;
            }
        }
        if( rflMode == 2 )
        {
            Cr = color environment("raytrace", Rshad, "samples", samples, "blur", blur );
        }
        if( rflMode == 3 && envMap != "" )
        {
            CrEnv = color environment(envMap, R, "samples", samples, "blur", blur );
            CrRT = color environment("raytrace", Rshad, "samples", samples, "blur", blur ); 
            rlo = smoothstep(0.0, 0.1, comp(CrRT, 0));
            Cr = mix(CrEnv, CrRT, rlo);
        }
    }
    else
    {
        Cr = 0;
    }
return Cr;
}

// Refraction parameters calculation from Brian Meanleys' bhm_refl.h
color rfrCalc(vector T, Nf;
              string envMap; 
              float samples, blur, rfrMode;
             )
{
	color Ct, CtEnv, CtRT;
	float tlo;

	if( rfrMode != 0 )
	{
		if( rfrMode == 1 && envMap != "" )
		{
			Ct = color environment(
					envMap, T, "samples", samples, "blur", blur );
		}
		if( rfrMode	== 2 )
		{
			Ct = color environment(
					"raytrace", T, "samples", samples, "blur", blur );			
		}
		if( rfrMode == 3 && envMap != "" )
		{
			CtEnv = color environment(
					envMap, T, "samples", samples, "blur", blur );
			CtRT = color environment(
					"raytrace", T, "samples", samples, "blur", blur );	
			
			tlo = smoothstep(0.0, 0.1, comp(CtRT, 0));
			
			Ct = mix(CtEnv, CtRT, tlo);
		}
	}
	else
	{
		Ct = 0;
	}
	return Ct;
}

// decode from sRGB luma to linear light
float sRGB_decode_f(float F)
{
    float lin;
    if(F <= 0.03928)
        lin = F/12.92;
    else
        lin = pow((F+0.055)/1.055, 2.4);
    return lin;
}
 
color sRGB_decode(color C)
{
    color D;
    setcomp(D, 0, sRGB_decode_f(comp(C,0)));
    setcomp(D, 1, sRGB_decode_f(comp(C,1)));
    setcomp(D, 2, sRGB_decode_f(comp(C,2)));
    return D;
}

// Blackbody color map function
color blackbodyfast( float temperature;)
{
    uniform color c[16] = 
        {
        (1,0.0401,0),(1,0.1718,0),(1,0.293,0.0257),(1,0.4195,0.1119),(1,0.5336,0.2301),
        (1,0.6354,0.3684),(1,0.7253,0.517),(1,0.8044,0.6685),(1,0.874,0.8179),(1,0.9254,0.9384),(0.929,0.9107,1),
        (0.8289,0.8527,1),(0.7531,0.8069,1),(0.6941,0.77,1),(0.6402,0.7352,1),(0.6033,0.7106,1)
        };
    float amount = smoothstep ( 1000, 10000, temperature );
    color blackbody = spline ( "catmull-rom", amount, c[0],
        c[0],c[1],c[2],c[3],c[4],c[5],c[6],c[7],c[8],c[9],
        c[10],c[11],c[12],c[13],c[14],c[15],
        c[15]);
    return blackbody;
}


#endif /* EXTRAS_H */
