/* Shader description goes here */
#include "../include/bf_helpers.h"

float sawtooth(float freq; float amp; float inverse;)
{
	extern float t;
	float tt;
	float i;
	float add_iteration = 0;
	
	if (inverse > 0)
		tt = (1-t * freq * S_PI) - 1;
	else
		tt = (t * freq * S_PI) + 0.5;
		
	for (i = 2; i <= 50; i+=1)
    	add_iteration += (1/i)*sin(i*tt);

	return (sin(tt) + add_iteration) * amp;
}

class 
bf_Leaf
(
	uniform float	leaf_width = 0.5;
	uniform float	saw_freq = 10;
	uniform float	saw_amp = 0.01;
	uniform float	seed = 10;
	
	uniform float	fKd = 0.8;
	uniform float	bKd = 0.6;
	
	uniform float	Ks = 0.3;
	uniform float	specRoughness = 0.1;
	uniform color	specColor = color(1,1,1);
	
	uniform float	Km = 0.1;
	uniform float	creaseExp = 3;
	uniform float	creaseMult = 1;
	uniform float	bendExp = 3;
	uniform float	bendMult = 1;
	uniform color	tint[10] = {
								color(0.8, 0.8, 1.0),
								color(1.0, 0.4, 0.2),
								color(1.0, 0.4, 0.2),
								color(1.0, 1.0, 0.2),
								color(0.8, 1.0, 0.0),
								color(1.0, 0.8, 0.4),
								color(0.6, 0.6, 1.0),
								color(0.4, 1.0, 0.4),
								color(1.0, 0.6, 0.2),
								color(0.4, 0.2, 0.0) };
)
{
varying normal Nn = 0;
varying normal Nf = 0;
varying vector V = 0;

public void displacement(output point P; output normal N) {
	float hump = 0;
	normal n = normalize(N);
	
	float crease = creaseMult * pow(abs(s - 0.5), creaseExp);
	
	float bend = pow(t, bendExp) * bendMult;
	
	P = P - (n * ((crease + bend) * 0.5) * Km);
	N = calculatenormal(P);
	}

public void surface(output color Ci, Oi) {
	Nn = normalize(N); 
	Nf = faceforward(Nn, I);
	V = normalize(-I);
	
	// LEAF SHAPE CREATOR
	float tt = (1-(t*S_PI*0.35))- 1;
  
	float saw1 = sawtooth(saw_freq,saw_amp,1) * (1-t);
	float saw2 = sawtooth(saw_freq,saw_amp*5*clamp(leaf_width,0.5,1),0)  * (1-t);
  
	float add_noise1 = ((noise((t+seed)*5)*2)-1) * 0.05;
	float add_noise2 = ((noise((t+seed)*5)*2)-1) * 0.08;
	
	float curve1 =   (( sin(tt) * cos(tt) * log(abs(tt)) * clamp(leaf_width,0.5,1) ) + 0.5);
	float curve2 = 1-(( sin(tt) * cos(tt) * log(abs(tt)) * clamp(leaf_width,0.5,1) ) + 0.5);
  
	float curve1_noise = ((curve1*saw1)+(curve1*add_noise1)) + curve1;
	float curve2_noise = ((curve2*saw2)+(curve2*add_noise2)) + curve2;
  	
	// ALPHA CUTOUT 
	if ( s <= curve1_noise && s >= curve2_noise)
		Oi = 1;
	else  
		Oi = 0;

	// LEAF COLOR

	float ratio = 10;
	float leafVariation = noise(P * ratio);
	color leafColor = spline(leafVariation,
							tint[0],
							tint[0],
							tint[1],
							tint[2],
							tint[3],
							tint[4],
							tint[5],
							tint[6],
							tint[7],
							tint[8],
							tint[9], tint[9]);
							
							
	// DIFFUSE
	color fDiff = fKd * diffuse(Nn);
	color bDiff = bKd * diffuse(-Nn);
  	color finalDiff = fDiff + bDiff;

	// SPECULAR
	color finalSpec = Ks * specular(Nf, V, specRoughness) * specColor;

	Ci = Oi * (leafColor * finalDiff + finalSpec);
    }
}

