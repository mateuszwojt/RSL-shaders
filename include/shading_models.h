#ifndef SHADING_MODELS_H
#define SHADING_MODELS_H 1
/* 

Name: shading_models.h
Type: Header
Auth: Mateusz Wojt
Date: 14.07.2013


*/

#include "helpers.h" // General usage helpers and functions
#include "extras.h" // Geometric attenuation for Cook-Torrance model

/* Oren-Nayar diffuse model from Leonardo Covarrubias at blog.leocov.com */
color OrenNayar (normal N; vector V; float roughness;) {

  extern point P;

  float sigma2 = SQR(roughness);
  float A = 1 - 0.5 * sigma2 / (sigma2 + 0.33);
  float B = 0.45 * sigma2 / (sigma2 + 0.09);

  float theta_r = acos (V.N); //angle between V and N
  vector V_perp_N = normalize(V-N*(V.N)); //part of V perpendicular to N

  color C = 0;
  illuminance(P,N,PI/2)
  {
    extern vector L;
    extern color Cl;

    float nondiff = 0;
    lightsource ("__nodiffuse", nondiff);
    if(nondiff < 1)
    {
      vector Ln = normalize(L);
      float cos_theta_i = Ln.N;
      float cos_phi_diff = V_perp_N . normalize(Ln - N * cos_theta_i);
      float theta_i = acos (cos_theta_i);
      float alpha = max(theta_i, theta_r);
      float beta  = min(theta_i, theta_r);
      C += (1-nondiff) * Cl * cos_theta_i * (A + B * max(0,cos_phi_diff) * sin(alpha) * tan(beta));
    }
  }
  return C;
}

/* Oren-Nayar-Wolff diffuse model from odforce.net forum */
color OrenNayarWolff(float sigma, ior; color Cdiff; normal Nn; vector In) {
	extern point P;
	normal Nf = faceforward(Nn, In);
	vector Vf = -normalize(In);

	float cos_theta_r = Vf.Nf;
	float theta_r = acos( cos_theta_r );
	float sigma2 = SQR(sigma);

	color C = color(0), L1 = color(0), L2 = color(0);
	float eta = AIR/ior;

	illuminance( P, Nf, PI/2) {
		extern vector L;
		extern color Cl;
		uniform float nondiff = 0;
		lightsource("__nondiffuse", nondiff);
		
		if (nondiff < 1) {
			
			vector Ln = normalize(L);
			float cos_theta_i = Ln.Nf;
			float cos_phi_diff = normalize(Vf-Nf*cos_theta_r) .
									normalize(Ln-Nf*cos_theta_i);
			float theta_i = acos( cos_theta_i );
			float alpha = max( theta_i, theta_r );
			float beta = min( theta_i, theta_r );

			/* transmitted angle */
			float cos_theta_tl = sqrt( max( 0, 1 - (SQR(eta) * (1 - 
								(SQR(cos_theta_i)) )) ));
	
			/* 1st Oren-Nayar coefficient */
			float C1 = (1 - .5 * sigma2 / (sigma2 + .33)) * (1 - nfresnel(
				cos_theta_i, ior )) * (1 - nfresnel(cos_theta_tl, eta ));
			/* 2nd coefficient */
			float C2 = .45 * sigma2 / (sigma2 + .09);		
			if (cos_phi_diff >= 0) {
				C2 *= sin(alpha);
			} else {
				C2 *= (sin(alpha) - pow( 2 * beta / PI, 3));
			}
			/* 3rd coefficient (Qualitative model only uses C1+C2) */
			float C3 = .125 * sigma2 / (sigma2 + .09) * pow(( 4 * alpha *
						beta) / (PI * PI), 2);

			/* Final L1 term */
			L1 = Cdiff * (cos_theta_i * (C1 + cos_phi_diff * C2 * tan(beta)
						+ (1 - abs( cos_phi_diff)) * C3 * tan( (alpha
								+ beta) / 2 )));
			/* Final L2 term */
			L2 = (Cdiff * Cdiff) * (.17 * cos_theta_i * sigma2 / (sigma2
						+ .13) * (1 - cos_phi_diff * (4 * beta * beta) /
							(PI * PI)));
										
			C += (1-nondiff) * Cl * (L1 + L2);
		}
	}
	return C;
}

color ward_Aniso (normal N; vector V; vector xdir; float xroughness, yroughness;)
{

	float cos_theta_r = clamp(N.V, 0.0001, 1);
	vector X = xdir/xroughness;
	vector Y = (N ^ xdir)/yroughness;

	color C = 0;
	extern point P;
	illuminance(P,N,PI/2) {
		extern vector L; extern color Cl;
		float nonspec = 0;
		lightsource ("_nonspecular", nonspec);
		if (nonspec < 1) {
			vector LN = normalize(L);
			float cos_theta_i = LN.N;
			if (cos_theta_i > 0.0) {
				vector H = normalize(V + LN);
				float rho = exp(-2 * (SQR(X.H) + SQR(Y.H))/(1+H.N))/sqrt (cos_theta_i * cos_theta_r);
				C += Cl * ((1-nonspec) * cos_theta_i * rho);
			}
		}
	}
	return C / (4 * xroughness* yroughness);
}

/* Cook-Torrance specular model from odforce.net forum */
color CookTorrance(normal Nn; vector In; float Ks, Kd, eta, roughness; color Cdiff, Cspec;) {
	extern point P;
	color Ct = 0;
	float m2= SQR(roughness);

	vector Vn = normalize(In);
	vector Nf = faceforward(Nn, -Vn);
	illuminance ( P, Nn, PI/2 ) {
		extern vector L;
		vector Ln = normalize (L);
		vector Hn = normalize (Vn + Ln);
	//Cook-Torrance parmeters
		float t = Hn.Nn;
		float t2 = t*t; //t squared
		float v = Vn.Nn;
		float vp= Ln.Nn;
		float u = Hn.Vn;

		float D = 0.5/(m2*t2*t2) * exp( (t2-1)/(m2*t2) );
		float G = min(1 , 2*min(t*v/u, t*vp/u) );
		float Kr, Kt;
		fresnel(Ln, Nn, eta, Kr, Kt);
		Ct += Cl * ( Cdiff*Kd + Cspec*Ks*Kr*D*G/(vp*v) ) * Nn.Ln;
	}
	return Ct;
}

color SchlickFresnel (float bias; float eta; float Kfr;)
{
	color C = 0;
  	extern point P;
  	extern normal N;
  	extern vector I;
	
	normal Nn = normalize(N);
  	vector Vn = -normalize(I);
  	float dotnv = abs(Nn.Vn);
  	float Kr = eta + (1-eta)*pow(1-dotnv,5);
  	Kr = Kfr*Bias(Kr,bias);

  	C = color(Kr,Kr,Kr);
  	return C;
}

#endif /* SHADING_MODELS_H */


			
				
				
				
				
				
				
				
				
				
				
				
