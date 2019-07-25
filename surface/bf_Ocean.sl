/* 
bf_Ocean.sl

Taken from 3delight Studio Pro 11 Ocean headers

*/

#include "../include/bf_ocean.h"

class bf_Ocean
(

	uniform float Kd = 1;
	uniform float Km = 0.1;
	
	uniform float time = 1;
	uniform float waveSpeed = 0.732;
	uniform float scale = 1;
	uniform float windUV[2] = {1, 0};
	uniform float observerSpeed = 0;
	uniform float numFrequencies = 24;
	uniform float waveDirSpread = 0.691;
	uniform float waveLengthMin = 1;
	uniform float waveLengthMax = 4;
	uniform float waveHeight = 1;
	uniform float waveTurbulence = 1;
	uniform float wavePeaking = 1;
	uniform float foamEmission = 1;
	uniform float foamThreshold = 0.8;
	
	color foamColor = color(1,1,1);
	color waterColor = color(0,0.65,0.95);
)
{
varying float  hump = 0;
varying normal n = 0;
varying normal Ndiff = 0;

varying	float ocean;
varying	float foam;
varying float underwaterFoam;
varying float foamInverse;
  
public void displacement(output point P; output normal N) {
	n = normalize(N);
	Ndiff = n - normalize(Ng);
	hump = 0;
	

	point Pw = transform("world", P);
	
	float _time = 0.2 * time * waveSpeed;
	
	getOcean(
			xcomp(P), -zcomp(P), _time, scale, windUV,
			observerSpeed, waveDirSpread, numFrequencies,
			waveLengthMin, waveLengthMax,
			waveHeight,
			waveTurbulence, wavePeaking, foamEmission,
			foamThreshold, ocean, foam);
			
	float normalizedWaves = 2 * ocean / (scale * waveLengthMax) + 0.5;
	
			
	
	P = P - n * normalizedWaves * Km;
	//P = P;
	N = calculatenormal(P);
	N = n - Ndiff;
	}    
public void surface(output color Ci, Oi) {
	n = normalize(N); 
	normal nf = faceforward(n, I);
	color   surfcolor = Cs;
	color  diffusecolor = Kd * diffuse(nf); 
	
	getOcean(
			s, t, time, scale, windUV,
			observerSpeed, waveDirSpread, numFrequencies,
			waveLengthMin, waveLengthMax,
			waveHeight,
			waveTurbulence, wavePeaking, foamEmission,
			foamThreshold, ocean, foam);
	
	if (foam > 0)
	{
		underwaterFoam = min(0.5, foam);
		foam = max(0, foam - 0.3);
		foamInverse = 1 - 0.5 * foam;
		foamInverse = max(0, foamInverse);
	}
		
	Oi = Os;
	Ci = Oi * underwaterFoam * foamColor * waterColor * diffusecolor;
    }

}
