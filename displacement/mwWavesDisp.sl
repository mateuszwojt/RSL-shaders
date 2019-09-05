/* Shader description goes here */
displacement
mwWavesDisp(float Km = 0.1,
			freq = 1,
			sfreq = 1,
            layers = 3,
			waviness = 0.3,
			height = 1,
			phase = 1;
  string    space = "object")
{
	float	hump = 0;
	normal n = normalize(N);

	point   p = transform(space, P);
	float   j, f = freq, amplitude = 1;
	
	float w =  noise(p) * waviness;
	hump = sin((s + phase + w) * 2 * PI * sfreq) * height;  
	
	for(j = 0; j < layers; j += 1) {
		hump += abs(noise(p * f) - 0.5) * amplitude;
    	f *= 2;
    	amplitude *= 0.5;
	}	

  
	P = P - n * hump * Km;
	N = calculatenormal(P);
}
