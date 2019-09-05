/* Shader description goes here */
#include "../include/helpers.h"
class 
mwGerstnerWaveDisp
(
	float	Km = 0.1,
			octaves = 3,
			Wlen = 60,
			Amp = 2.5,
			ChopFact = .763,
			WlenGain = .7,
			AmpGain = .6,
			AngSpread = 50,
			AngSeed = 200;
)
{
varying float octave = 0, m, w, wt, Rot;
varying vector KnMMag;
varying normal n = 0;
uniform float g = 9.8;

matrix RotMat = 0;
float disp, dispy;
point Po = transform("current", "object", P);
vector V = vector(1, 1, 1);
float Phase = Wlen/V[2]/2;
//float ChopMod = 1;
//float Chop = ChopFact * ChopMod;
public void displacement(output point P; output normal N) {
	n = normalize(N);
	
	for(octave=0; octave<octaves; octave+=1) {
		//RotMat = setcomp(matrix RotMat; 0, 1, (random(octave*10*AngSeed)-.5) * AngSpread));
		//RotMat = matrix(0,0,0, 0,(random(octave*10*AngSeed)-.5) * AngSpread),0, 1,1,1);
		float Rot = random(octave*10*AngSeed)-.5 * AngSpread;
		m = S_2PI/Wlen;
		KnMMag = RotMat*(n*m);
		float gmultm = g * m;
		w = sqrt(gmultm);
		wt = w * time;
		float x0 = P * V;
		dispy += Amp * cos(KnMMag.x0 - wt + Phase) * Amp;
		disp += Amp * (KnMMag / m) * sin(KnMMag.x0 - wt + Phase); //* Chop;
		}
	
	
	
	P = P - n * Km;
	N = calculatenormal(P);
	}    
}
