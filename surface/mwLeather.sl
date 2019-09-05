/* 

Name: mwLeather.sl
Type: Surface
Auth: Mateusz Wojt
Date: 23.06.2013


*/
class 
mwLeather(
			float	Kfb = 1,
					Ks = 2,
					Ka = 0.5,
					amp = 1,
					roughness = 0.1,
					freq = 4,
					rim_width = 1,
					rim_strength = 1;
			color 	top = 1,
					highlight = 1,
					low = 1,
					rim_color = 1;
			float	Km = 0.1,
					Kfreq = 6,
					second = 1,
					Kfreq2 = 20,
					Km2 = 0.1;
			)
{
varying float  hump = 0;
varying normal n = 0, nf= 0;
varying vector i = 0;
  
public void displacement(output point P; output normal N) {
	n = normalize(N);
	hump = 0;
	vector diff = n - normalize(Ng);
	
	hump = abs(noise(s * Kfreq/6, t * Kfreq) - 0.5);
	
	P = P - n * hump * Km;
	N = normalize(calculatenormal(P)) + diff;
	
	if (second) {
		n = normalize(N);
		diff = n - normalize(Ng);
		hump = abs(noise((s + 0.3) * Kfreq2 * 3, (t - 0.3) * Kfreq2 * 3) - 0.5);
		P = P - n * hump * Km2 * 0.5;
		N = normalize(calculatenormal(P)) - diff;
	}
	}    
public void surface(output color Ci, Oi) {
	float ns = abs(noise(s * freq, t * freq) - 0.8);
	n = normalize(N);
	nf = faceforward(n, I);
	i = normalize(-I);
	float tt = t + ns * amp;
	float dot = 1 - i.nf;
	color fakeEdge = smoothstep(1 - rim_width, 1, dot) * rim_strength * rim_color;
	color diffusecolor = Kfb * diffuse(nf);
	color ambientcolor = Ka * ambient();
	color surfcolor = mix(top, low, tt);
	
	color speccolor = Ks * specular(nf, i, roughness) * highlight;
	Oi = Os;
	Ci = Oi * Cs * surfcolor * Kfb * (diffusecolor + ambientcolor + fakeEdge + speccolor);
	}
}
