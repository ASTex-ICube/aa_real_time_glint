#version 330

// The MIT License
// Copyright © 2021 Xavier Chermain (ICUBE), Simon Lucas(ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Implementation of
// Real-Time Geometric Glint Anti-Aliasing with Normal Map Filtering
// 2021 Xavier Chermain (ICUBE), Simon Lucas(ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Accepted for [i3D 2021](http://i3dsymposium.github.io/2021/) and for CFG special issue.

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
