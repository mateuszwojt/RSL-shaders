/* 
bf_SimpleProduction.sl

Simple version of Production shader.

Uses OrenNayarWolff diffuse model 
and CookTorrance specular term.



 */

#include "../include/bf_shading_models.h"
#include "../include/bf_helpers.h"
#include "../include/bf_extras.h"

class bf_Production
(
	// ST coordinates modification
	uniform float st_uv = 0;
	uniform float txCoord_scale_s = 1;
	uniform float txCoord_scale_t = 1;
	uniform float uvSet = 0;
	uniform float totalUVSets = 1;
	
	// Diffuse
	color diffColor = color(0.18, 0.18, 0.18);
	string diffMap = "";
	uniform float diffAlphaChan = 3;
	uniform float Kd = 0.8;
	uniform float gamma = 1.0;
	uniform float diffSigma = 0.1;
	uniform float diffIOR = 1.5;
	
	// Specular
	color specColor = color(1,1,1);
	string specMap = "";
	uniform float enableAniso = 0;		//enable aniso modulation
	uniform float xRoughness = 0.5;		// ideal # 0.15
    uniform float yRoughness = 0.15; 	// ideal # 0.5
    uniform float dirModulate = 1;
	uniform float specRoughness = .1;
	uniform float specEta = .8;
	uniform float Ks = 0.8;
	
	// Reflection
	string envMap = "";
	uniform float Kr = 0.3;
	uniform float reflIOR = 1.5;
	uniform float reflSamples = 4;
	uniform float reflBlur = 0.0;
	uniform float reflMode = 2;
	uniform float reflOcc = 0;
	
	// Refraction
	uniform float Krf = 1;
	uniform float refrSamples = 4;
	uniform float refrBlur = 0.0;
	uniform float refrMode = 2;
	
	// Bump
	uniform float Km = 0.1;
	string bmpMap = "";
	
	//AOVs
	output varying color aov_diffuse = 0;
	output varying color aov_specular = 0;
	output varying color aov_reflection = 0;
	output varying color aov_refraction = 0;
	output varying color aov_occlusion = 0;
	output varying color aov_Pw = 0;
	output varying color aov_Pc = 0;
	output varying color aov_Pn = 0;
	output varying color aov_sss = 0;
)
{
color _diff = 0;
color _diffMap = 1;
float _diffMsk = 1;
color _diffGamma = 0;
color _spec = 0;
color _specMap = 1;
color _refl = 0;
color _refr = 0;
float _occl = 1;

varying normal Nn = 0;
varying normal Ns = 0;
varying normal Nf = 0;
varying vector In = 0;
varying vector Vn = 0;
varying vector xdir = 0;

public void surface(output color Ci, Oi) {
	Nn = normalize(N);
	In = normalize(I);
	Vn = -normalize(I);
	Nf = faceforward(Nn, I);
	xdir = normalize(dPdu) * dirModulate;
	
	// Texture coordinates modification
	float ss, tt;
	if (st_uv == 1)
	{
		varying float u_uvSet[totalUVSets] = {totalUVSets};
		varying float v_uvSet[totalUVSets] = {totalUVSets};
		
		ss = u_uvSet[uvSet];
		tt = v_uvSet[uvSet];
	}
	else
	{
		ss = s * txCoord_scale_s;
		tt = t * txCoord_scale_t;
	}
	// DIFFUSE
	// Apply Gamma (linearisation)
	_diffGamma = gammaCorrect(diffColor, gamma);
	
	if (diffMap != "")
		_diffMap = texture(diffMap, ss, tt);
		_diffMsk = texture(diffMap[diffAlphaChan], ss, tt);
		_diffGamma = mix(_diffGamma, _diffMap, _diffMsk);
	_diff = OrenNayarWolff(diffSigma, diffIOR, _diffGamma, Nn, In);
	
	// SPECULAR
	if (specMap != "")
		_specMap = texture(specMap, ss, tt);
	
	if (enableAniso > 0.5) 
	{
		if (Ks > 0) 
		{
			_spec = ward_Aniso(Nn,-In,xdir,xRoughness,yRoughness); // Ward Anisotropic reflection
		}
	}
	else 
	{
		_spec = CookTorrance(Nn, In, Ks, Kd, specEta, specRoughness, diffColor, specColor); //Cook Torrance reflection
	}	
	_spec *= (specColor * _specMap);
	
	// REFLECTION
	float fkr, fkt;
	vector R, T;
	
	fresnel(In, Nf, 1/reflIOR, fkr, fkt, R, T);
	fkt = 1 - fkr;
	
	_refl = rflCalc(Nf, envMap, reflSamples, reflBlur, reflMode, reflOcc) * fkr;
	
	// REFRACTION
	_refr = rfrCalc(T, Nf, envMap, refrSamples, refrBlur, refrMode) * fkt;
	
	// FINAL ILLUMINANCE
	Oi = Os;
	//Ci = Kd * _diff + Ks * _spec + Kr * _refl + Krf * _refr;
	Ci = _occl;
	Ci *= Oi;
	
	aov_diffuse = _diff;
	aov_specular = _spec;
	aov_reflection = _refl;
	aov_refraction = _refr;
	aov_occlusion = _occl;
	
	// Additional point color AOV transformations
	point Pw = transform("current", "world", P);
	point Pc = transform("current", "camera", P);
	normal Pn = (ntransform("camera", Nn) + 1) / 2;
	aov_Pw = color(Pw[0], Pw[1], Pw[2]);
	aov_Pc = color(Pc[0], Pc[1], Pc[2]);
	aov_Pn = color(Pn[0], Pn[1], Pn[2]);
    }
}
