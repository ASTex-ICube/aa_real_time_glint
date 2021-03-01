#version 330

uniform ivec2 Resolution;

uniform sampler2D Tex;

uniform float MaxIntensity;
uniform bool Tonemapping;
uniform bool GammaCorrection;
uniform bool Bloom;

layout(location = 0) out vec4 FragColor;

// bloom function from https://www.shadertoy.com/view/ldVGRh
vec3 bloom(vec2 coord, vec2 res){
    const int r = 8;
	float stddev = float(r) / 3.;

	vec3 bloom_ = vec3(.0);
	float w = 0.0;

    vec2 offsetCoord = coord + .5;
    
	const float threshold = 0.5;
    ivec2 d = ivec2(1, -1); 
	for (int di = 0; di <= 5; ++di) {
		for (int o = -r; o <= r; ++o) {
			vec2 of = vec2(o) / stddev;
			float weight = exp(-.5 * dot(of, of));
			vec3 s = texture(Tex, clamp((offsetCoord + vec2(o * d)) / res,vec2(0.0001),vec2(0.9999))).xyz;
			bloom_ += weight * max(vec3(.0), s - threshold);
			w += weight;
		}
        d.y += 1;
	}

    bloom_ /= w;
    
	vec3 radiance = texture(Tex, coord / res).xyz;
	radiance +=  bloom_;
	return radiance;
}

vec3 tonemapping(vec3 radiance){
	vec3 r = radiance *  vec3(0.212671, 0.715160, 0.072169);
	float y = r.x + r.y + r.z;
    float scale = (1 + y / (MaxIntensity * MaxIntensity)) / (1 + y);
    return radiance * scale;
}


void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(Resolution);
    vec3 radiance = textureLod(Tex,uv,0.).xyz;

    if(Bloom)
		radiance = bloom(gl_FragCoord.xy, vec2(Resolution));
    
	if(Tonemapping)
		radiance = tonemapping(radiance);

	if(GammaCorrection)
		radiance = pow( radiance, vec3(1./2.2) );

    FragColor = vec4(radiance,1.);

}
