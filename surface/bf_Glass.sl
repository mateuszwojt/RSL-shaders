/* Shader description goes here */
class bf_Glass
(
	uniform float Ks = 1;
	uniform float Kr = 1;
	uniform float Krf = 1;
)
{
varying float  hump = 0;
varying normal n = 0;
  
public void surface(output color Ci, Oi) {
	Ks *= (1-roughness);
	_spec = Ks * specular(Nf, V, roughness);
	color hitCi = color(0,0,0);
	//sampCone = the sampling area (in radians)
	gather("illuminance", P, refrVector, sampCone, refrSamps, "surface:Ci", hitCi) {
	Ci += hitCi;
	} else {
	//ray didn't hit a surface - return environment or other operation
	}

	Ci /= refrSamps;
	Oi = 1
	Ci += _diff + _spec + _refl + _refr;
	Ci *= Oi;
    }
}
