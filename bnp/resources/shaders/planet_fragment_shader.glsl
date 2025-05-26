#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform float time;
uniform float noise_radius;
uniform float noise_seed;
uniform vec3 axis;
uniform vec3 sun_direction;

uniform float water_depth;
uniform float coast_depth;
uniform float mainland_depth;
uniform vec3 water_color;
uniform vec3 coast_color;
uniform vec3 mainland_color;
uniform vec3 mountain_color;

// Classic Perlin 3D Noise
// by Stefan Gustavson (https://github.com/stegu/webgl-noise)
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
vec4 fade(vec4 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float cnoise(vec4 P){
    vec4 Pi0 = floor(P); // Integer part for indexing
    vec4 Pi1 = Pi0 + 1.0; // Integer part + 1
    Pi0 = mod(Pi0, 289.0);
    Pi1 = mod(Pi1, 289.0);
    vec4 Pf0 = fract(P); // Fractional part for interpolation
    vec4 Pf1 = Pf0 - 1.0; // Fractional part - 1.0
    vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    vec4 iy = vec4(Pi0.yy, Pi1.yy);
    vec4 iz0 = vec4(Pi0.zzzz);
    vec4 iz1 = vec4(Pi1.zzzz);
    vec4 iw0 = vec4(Pi0.wwww);
    vec4 iw1 = vec4(Pi1.wwww);

    vec4 ixy = permute(permute(ix) + iy);
    vec4 ixy0 = permute(ixy + iz0);
    vec4 ixy1 = permute(ixy + iz1);
    vec4 ixy00 = permute(ixy0 + iw0);
    vec4 ixy01 = permute(ixy0 + iw1);
    vec4 ixy10 = permute(ixy1 + iw0);
    vec4 ixy11 = permute(ixy1 + iw1);

    vec4 gx00 = ixy00 / 7.0;
    vec4 gy00 = floor(gx00) / 7.0;
    vec4 gz00 = floor(gy00) / 6.0;
    gx00 = fract(gx00) - 0.5;
    gy00 = fract(gy00) - 0.5;
    gz00 = fract(gz00) - 0.5;
    vec4 gw00 = vec4(0.75) - abs(gx00) - abs(gy00) - abs(gz00);
    vec4 sw00 = step(gw00, vec4(0.0));
    gx00 -= sw00 * (step(0.0, gx00) - 0.5);
    gy00 -= sw00 * (step(0.0, gy00) - 0.5);

    vec4 gx01 = ixy01 / 7.0;
    vec4 gy01 = floor(gx01) / 7.0;
    vec4 gz01 = floor(gy01) / 6.0;
    gx01 = fract(gx01) - 0.5;
    gy01 = fract(gy01) - 0.5;
    gz01 = fract(gz01) - 0.5;
    vec4 gw01 = vec4(0.75) - abs(gx01) - abs(gy01) - abs(gz01);
    vec4 sw01 = step(gw01, vec4(0.0));
    gx01 -= sw01 * (step(0.0, gx01) - 0.5);
    gy01 -= sw01 * (step(0.0, gy01) - 0.5);

    vec4 gx10 = ixy10 / 7.0;
    vec4 gy10 = floor(gx10) / 7.0;
    vec4 gz10 = floor(gy10) / 6.0;
    gx10 = fract(gx10) - 0.5;
    gy10 = fract(gy10) - 0.5;
    gz10 = fract(gz10) - 0.5;
    vec4 gw10 = vec4(0.75) - abs(gx10) - abs(gy10) - abs(gz10);
    vec4 sw10 = step(gw10, vec4(0.0));
    gx10 -= sw10 * (step(0.0, gx10) - 0.5);
    gy10 -= sw10 * (step(0.0, gy10) - 0.5);

    vec4 gx11 = ixy11 / 7.0;
    vec4 gy11 = floor(gx11) / 7.0;
    vec4 gz11 = floor(gy11) / 6.0;
    gx11 = fract(gx11) - 0.5;
    gy11 = fract(gy11) - 0.5;
    gz11 = fract(gz11) - 0.5;
    vec4 gw11 = vec4(0.75) - abs(gx11) - abs(gy11) - abs(gz11);
    vec4 sw11 = step(gw11, vec4(0.0));
    gx11 -= sw11 * (step(0.0, gx11) - 0.5);
    gy11 -= sw11 * (step(0.0, gy11) - 0.5);

    vec4 g0000 = vec4(gx00.x,gy00.x,gz00.x,gw00.x);
    vec4 g1000 = vec4(gx00.y,gy00.y,gz00.y,gw00.y);
    vec4 g0100 = vec4(gx00.z,gy00.z,gz00.z,gw00.z);
    vec4 g1100 = vec4(gx00.w,gy00.w,gz00.w,gw00.w);
    vec4 g0010 = vec4(gx10.x,gy10.x,gz10.x,gw10.x);
    vec4 g1010 = vec4(gx10.y,gy10.y,gz10.y,gw10.y);
    vec4 g0110 = vec4(gx10.z,gy10.z,gz10.z,gw10.z);
    vec4 g1110 = vec4(gx10.w,gy10.w,gz10.w,gw10.w);
    vec4 g0001 = vec4(gx01.x,gy01.x,gz01.x,gw01.x);
    vec4 g1001 = vec4(gx01.y,gy01.y,gz01.y,gw01.y);
    vec4 g0101 = vec4(gx01.z,gy01.z,gz01.z,gw01.z);
    vec4 g1101 = vec4(gx01.w,gy01.w,gz01.w,gw01.w);
    vec4 g0011 = vec4(gx11.x,gy11.x,gz11.x,gw11.x);
    vec4 g1011 = vec4(gx11.y,gy11.y,gz11.y,gw11.y);
    vec4 g0111 = vec4(gx11.z,gy11.z,gz11.z,gw11.z);
    vec4 g1111 = vec4(gx11.w,gy11.w,gz11.w,gw11.w);

    vec4 norm00 = taylorInvSqrt(vec4(dot(g0000, g0000), dot(g0100, g0100), dot(g1000, g1000), dot(g1100, g1100)));
    g0000 *= norm00.x;
    g0100 *= norm00.y;
    g1000 *= norm00.z;
    g1100 *= norm00.w;

    vec4 norm01 = taylorInvSqrt(vec4(dot(g0001, g0001), dot(g0101, g0101), dot(g1001, g1001), dot(g1101, g1101)));
    g0001 *= norm01.x;
    g0101 *= norm01.y;
    g1001 *= norm01.z;
    g1101 *= norm01.w;

    vec4 norm10 = taylorInvSqrt(vec4(dot(g0010, g0010), dot(g0110, g0110), dot(g1010, g1010), dot(g1110, g1110)));
    g0010 *= norm10.x;
    g0110 *= norm10.y;
    g1010 *= norm10.z;
    g1110 *= norm10.w;

    vec4 norm11 = taylorInvSqrt(vec4(dot(g0011, g0011), dot(g0111, g0111), dot(g1011, g1011), dot(g1111, g1111)));
    g0011 *= norm11.x;
    g0111 *= norm11.y;
    g1011 *= norm11.z;
    g1111 *= norm11.w;

    float n0000 = dot(g0000, Pf0);
    float n1000 = dot(g1000, vec4(Pf1.x, Pf0.yzw));
    float n0100 = dot(g0100, vec4(Pf0.x, Pf1.y, Pf0.zw));
    float n1100 = dot(g1100, vec4(Pf1.xy, Pf0.zw));
    float n0010 = dot(g0010, vec4(Pf0.xy, Pf1.z, Pf0.w));
    float n1010 = dot(g1010, vec4(Pf1.x, Pf0.y, Pf1.z, Pf0.w));
    float n0110 = dot(g0110, vec4(Pf0.x, Pf1.yz, Pf0.w));
    float n1110 = dot(g1110, vec4(Pf1.xyz, Pf0.w));
    float n0001 = dot(g0001, vec4(Pf0.xyz, Pf1.w));
    float n1001 = dot(g1001, vec4(Pf1.x, Pf0.yz, Pf1.w));
    float n0101 = dot(g0101, vec4(Pf0.x, Pf1.y, Pf0.z, Pf1.w));
    float n1101 = dot(g1101, vec4(Pf1.xy, Pf0.z, Pf1.w));
    float n0011 = dot(g0011, vec4(Pf0.xy, Pf1.zw));
    float n1011 = dot(g1011, vec4(Pf1.x, Pf0.y, Pf1.zw));
    float n0111 = dot(g0111, vec4(Pf0.x, Pf1.yzw));
    float n1111 = dot(g1111, Pf1);

    vec4 fade_xyzw = fade(Pf0);
    vec4 n_0w = mix(vec4(n0000, n1000, n0100, n1100), vec4(n0001, n1001, n0101, n1101), fade_xyzw.w);
    vec4 n_1w = mix(vec4(n0010, n1010, n0110, n1110), vec4(n0011, n1011, n0111, n1111), fade_xyzw.w);
    vec4 n_zw = mix(n_0w, n_1w, fade_xyzw.z);
    vec2 n_yzw = mix(n_zw.xy, n_zw.zw, fade_xyzw.y);
    float n_xyzw = mix(n_yzw.x, n_yzw.y, fade_xyzw.x);
    return 2.2 * n_xyzw;
}

// Rodrigues' rotation formula for rotating a vector around an arbitrary axis
mat3 rotateAroundAxis(vec3 axis, float angle) {
    axis = normalize(axis);  // Normalize the axis of rotation
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    // Outer product of axis with itself to get the "axis * axis" matrix
    mat3 axisOuterProduct = mat3(
        axis.x * axis.x, axis.x * axis.y, axis.x * axis.z,
        axis.y * axis.x, axis.y * axis.y, axis.y * axis.z,
        axis.z * axis.x, axis.z * axis.y, axis.z * axis.z
    );

    // Cross product matrix for the axis
    mat3 crossProd = mat3(
        0.0, -axis.z, axis.y,
        axis.z, 0.0, -axis.x,
        -axis.y, axis.x, 0.0
    );

    // Rotation matrix: Identity matrix + outer product + cross product
    return cosAngle * mat3(1.0) + (1.0 - cosAngle) * axisOuterProduct + sinAngle * crossProd;
}

vec3 spherical_coord(vec2 texCoord, float time) {
    // Normalize texcoord to [-0.5, 0.5]
    vec2 centerTexCoord = texCoord - vec2(0.5f);

    // Compute spherical coordinates (on a unit sphere)
    //float radius = sqrt(0.25f - (centerTexCoord.y * centerTexCoord.y));
    //vec3 sphereCoord = vec3(centerTexCoord.x, centerTexCoord.y, sqrt((radius * radius) - (centerTexCoord.x * centerTexCoord.x)));
    float radius = sqrt(0.25f - (centerTexCoord.y * centerTexCoord.y));
    vec3 sphereCoord = vec3(centerTexCoord.x, centerTexCoord.y, sqrt((radius * radius) - (centerTexCoord.x * centerTexCoord.x)));

    return sphereCoord;
}

vec3 surface_color(vec3 sphere_coord) {
    float rotationAngle = time * 0.1; // todo: rotation speed
    mat3 rotationMatrix = rotateAroundAxis(axis, rotationAngle);

    // Apply rotation
    vec3 rotated_coord = rotationMatrix * sphere_coord;

    // Sample the Perlin noise value (this is your "heightmap")
    float surface_noise_value = cnoise(vec4(rotated_coord * noise_radius, noise_seed));
    vec3 color;

    if (surface_noise_value < water_depth) {
        color = water_color;
    } else if (surface_noise_value - water_depth < coast_depth) {
        color = coast_color;
    } else if (surface_noise_value - water_depth - coast_depth < mainland_depth) {
        color = mainland_color;
    } else {
        color = mountain_color;
    }

    // ice caps
    float cap_shimmer = abs(surface_noise_value * 0.15f);
    float equator_dist = abs(dot(normalize(rotated_coord), normalize(axis)));
    if (equator_dist > clamp(0.9f - sin(cap_shimmer), 0.75, 1.0f))
    {
        color = vec3(0.9f, 0.9f, 1.0f) + vec3(0, 0, cap_shimmer);
    }

    return color;
}

vec3 atmosphere_color(vec3 sphere_coord) {
    float rotationAngle = time * 0.09; // todo: rotation speed
    mat3 rotationMatrix = rotateAroundAxis(axis, rotationAngle);

    // Apply rotation
    vec3 rotated_coord = rotationMatrix * sphere_coord;
    float equator_dist = abs(dot(normalize(rotated_coord), normalize(axis)));

    /////////////////////////////////
    // todo: shader inputs

    // affects thickness of clouds
    float coverage_depth = -0.125;

    // affects banding towards the equator, lower exponents increase banding
    float banding_exp = 1.7;

    // base sampling radius
    float base_radius = 5.0;

    // affects how exacerbated banding is by smoothing the radius gradient
    // between the equator and poles. values < 1 have less of a gradient,
    // while values > 1 have a more noticeable difference
    float radius_exp = 0.4;

    // end of inputs
    /////////////////////////////////

    float banding_factor = pow(equator_dist, banding_exp);
    // min of 0.5 so we never sample at origin
    float radius_factor = 0.5 + pow(1.0 - equator_dist, radius_exp);

    vec3 atmosphere_coord = rotated_coord;

    // stretch the sphere coordinate along axis to simulate banding
    // in tandem with increasing sample radius
    if (dot(atmosphere_coord, axis) > 0) {
        atmosphere_coord += axis * banding_factor;
    } else {
        atmosphere_coord -= axis * banding_factor;
    }

    float atmosphere_radius_factor = base_radius * radius_factor;
    float atmosphere_noise_value = cnoise(vec4(atmosphere_coord * atmosphere_radius_factor, noise_seed + sin(time * 0.03)));

    // check below value so clouds (mostly) form over water
    if (atmosphere_noise_value < coverage_depth) {
        return vec3(1.0f);
    }

    return vec3(0.0f);
}

void main() {
    float snap_interval = 0.02f;
    //vec2 snapped_coord = floor(TexCoord / snap_interval) * snap_interval;
    vec2 snapped_coord = TexCoord;
    vec3 coord = spherical_coord(snapped_coord, time);
    
    // Rotation angle based on time (adjust rotation speed as needed)
    float rotationAngle = time * 0.1;  // Adjust speed of rotation

    // The circle is always centered at (0.5, 0.5) with a fixed radius of 0.5
    float dist = distance(snapped_coord, vec2(0.5f, 0.5f));  // Distance from center of quad

    if (dist >= 0.5f)  // If the fragment is outside the circle, discard it
    {
        discard;
    }

    // Use the Perlin noise to determine the planet's color: green for land, blue for water
    vec3 surface_color = surface_color(coord);
    vec3 atmosphere_color = atmosphere_color(coord);

    // calculate brightness of fragment based on sun position
    float brightness = clamp(sqrt(10 * dot(coord, normalize(sun_direction))), 0.25f, 1.0f);

    FragColor = vec4(brightness * (surface_color + atmosphere_color), 1.0f);
}
