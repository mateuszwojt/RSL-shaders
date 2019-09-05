/* Shader description goes here */

#include "../include/shading_models.h"
#include "../include/noises.h"
#include "../include/helpers.h"
#include "../include/extras.h"
class 
mwCarPaint
(
	color			c1[4] = {color(0,0.243137,0.52549),
								color(0,0.345098,0.741176),
								color(0,0.607843,1),
								color(0.32549,0.878431,1)};
								
	uniform float	Kd = 0.6;
	uniform color	diffColor = color(0.317,0.584,0.843);
	uniform float	diffRoughness = 0.1;
	
	uniform float	Ks = 0.4;
	uniform float	specRoughness = 0.0;
	uniform float	shellacRoughness = 1;
	uniform float	specGlow = 0.35;
	
	uniform string	envMap = "";
	uniform float	Kr = 0.3;
	uniform float	reflIOR = 1.15;
	uniform float	reflSamples = 4;
	uniform float	reflBlur = 0.0;
	uniform float	reflMode = 2;
	uniform float	reflOcc = 0;
)
{
color minC = color(0,0,0);
color maxC = color(1,1,1);

color _diff = 0;
color _spec = 0;
color _refl = 0;

varying normal Nn = 0;
varying normal Nf = 0;
varying vector In = 0;
varying vector V = 0;
varying float fr = 0;

public void surface(output color Ci, Oi) {
	Nn = normalize(N);
	In = normalize(I);
	Nf = faceforward(Nn, I);
	V = -In;
	fr = abs(Nn.In);
	
	// MAIN BODY
	point pp = transform("object", P);
	float noiseWidth = filterwidthp(pp);
	float freq = 300;
	float speckles = filteredsnoise(pp * freq, noiseWidth * freq) / 3;
	color paintColor = spline(speckles, c1[0], c1[0], c1[1], c1[2], c1[3], c1[3]);
	
	// SHELLAC
	float procShellac = filteredsnoise(pp * freq, noiseWidth * freq);
	
	// DIFFUSE
	_diff = Kd * OrenNayar(Nn, V, diffRoughness) * diffColor * fr ;
	
	// SPECULAR
	_spec = Ks * specular(Nf, V, specRoughness) * paintColor * fr;
	
	// ROUGH SPECULAR
	
	color _rspec = specular(Nf, V, procShellac / shellacRoughness) * fr * specGlow;
	
	// REFLECTION
	float fkr, fkt;
	vector R, T;
	
	fresnel(In, Nf, 1/reflIOR, fkr, fkt, R, T);
	fkt = 1 - fkr;
	
	_refl = Kr * rflCalc(Nf, envMap, reflSamples, reflBlur, reflMode, reflOcc) * fkr;
	
	// FINAL ILLUMINATION
	Oi = Os;
	Ci = _diff + _spec + _rspec + _refl;
	//Ci = _refl;
	Ci *= Oi;
    }
}
