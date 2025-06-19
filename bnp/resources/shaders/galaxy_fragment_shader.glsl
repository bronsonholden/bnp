#version 330 core
out vec4 FragColor;

in vec2 TexCoord;


float noise_seed = 12.0;
float spin_dir = -1.0;

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

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p) {
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
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

vec3 spherical_coord(vec2 texCoord) {
    // Normalize texcoord to [-0.5, 0.5]
    vec2 centerTexCoord = texCoord - vec2(0.5f);

    // Compute spherical coordinates (on a unit sphere)
    //float radius = sqrt(0.25f - (centerTexCoord.y * centerTexCoord.y));
    //vec3 sphereCoord = vec3(centerTexCoord.x, centerTexCoord.y, sqrt((radius * radius) - (centerTexCoord.x * centerTexCoord.x)));
    float radius = sqrt(0.25f - (centerTexCoord.y * centerTexCoord.y));
    vec3 sphereCoord = vec3(centerTexCoord.x, centerTexCoord.y, sqrt((radius * radius) - (centerTexCoord.x * centerTexCoord.x)));

    return sphereCoord;
}

vec3 random_spherical_coord(vec3 sample_coord) {
    float phi = noise(sample_coord) * 2 * 3.14159265;

    float theta = asin(2.0 * noise(-sample_coord) - 1.0);

    float x = cos(theta) * cos(phi);
    float y = cos(theta) * sin(phi);
    float z = sin(theta);

    return vec3(x, y, z);  // Return the 3D spherical coordinate (unit vector on the sphere)
}

vec3 base_color(vec3 sphere_coord, float dist_from_center) {
    float noise_radius = 4.0;
    float rotationAngle = 0;
    mat3 rotationMatrix = rotateAroundAxis(vec3(0, 0, 1), rotationAngle);

    // Apply rotation
    vec3 rotated_coord = rotationMatrix * sphere_coord;

    // Sample the Perlin noise value (this is your "heightmap")
    float surface_noise_value;

    surface_noise_value = cnoise(vec4(rotated_coord * noise_radius / (0.1 + dist_from_center), noise_seed));
    surface_noise_value += 2 * cnoise(vec4(rotated_coord * noise_radius * 4, noise_seed));

    if (surface_noise_value > 0.4 || dist_from_center < 0.03) {
        return vec3(0.6, 0.4, 0.25);
    }

    return vec3(0);
}

vec3 subtle_detail_color(vec3 sphere_coord, float dist_from_center) {
    float noise_radius = 30.0;
    float rotationAngle = 0;
    mat3 rotationMatrix = rotateAroundAxis(vec3(0, 0, 1), rotationAngle);

    // Apply rotation
    vec3 rotated_coord = rotationMatrix * sphere_coord;

    // Sample the Perlin noise value (this is your "heightmap")
    float surface_noise_value;

    surface_noise_value = cnoise(vec4(rotated_coord * noise_radius, noise_seed * 2.0));

    if (surface_noise_value > 0 || dist_from_center < 0.04) {
        return vec3(0.43, 0.16, 0.25);
    }

    return vec3(0);
}

vec3 detail_color(vec3 sphere_coord, float dist_from_center, float seed_mult) {
    float noise_radius = 9.0;
    float rotationAngle = 0;
    mat3 rotationMatrix = rotateAroundAxis(vec3(0, 0, 1), rotationAngle);

    // Apply rotation
    vec3 rotated_coord = rotationMatrix * sphere_coord;

    // Sample the Perlin noise value (this is your "heightmap")
    float surface_noise_value;

    surface_noise_value = cnoise(vec4(rotated_coord * noise_radius, noise_seed * seed_mult));
    surface_noise_value += cnoise(vec4(rotated_coord * 15.0 * noise_radius, noise_seed * seed_mult));

    surface_noise_value += max(0.2 - dist_from_center, 0);

    float dist_scale = dist_from_center * 2.0;
    if (surface_noise_value > dist_scale) {
        return mix(vec3(0.96, 0.94, 0.68), vec3(0.1, 0.17, 0.5), pow(dist_scale, 0.55));
    }

    return vec3(0);
}

float negative_detail(vec3 sphere_coord, float dist_from_center, float seed_mult) {
    float noise_radius = 2.0;
    float rotationAngle = 0;
    mat3 rotationMatrix = rotateAroundAxis(vec3(0, 0, 1), rotationAngle);

    // Apply rotation
    vec3 rotated_coord = rotationMatrix * sphere_coord;

    // Sample the Perlin noise value (this is your "heightmap")
    float surface_noise_value;

    surface_noise_value = cnoise(vec4(rotated_coord * noise_radius / (0.03 + dist_from_center), noise_seed * seed_mult));
    surface_noise_value += cnoise(vec4(rotated_coord * noise_radius * 3, noise_seed * seed_mult));

    if (dist_from_center < 0.04) {
        return 0;
    }

    if (surface_noise_value > 0.4) {
        return dist_from_center;
    }

    return 0;
}

void main() {
    float snap_interval = 0.02f;
    vec3 coord = spherical_coord(TexCoord);

    // The circle is always centered at (0.5, 0.5) with a fixed radius of 0.5
    float dist = distance(TexCoord, vec2(0.5, 0.5));  // Distance from center of quad

    // check if outside circle
    if (dist >= 0.42f * (0.9 + 0.05 * tan(TexCoord.x))) {
        discard;
    }

    if (dist < 0.008) {
        FragColor = vec4(1);
        return;
    } else if (dist < 0.013) {
        FragColor = vec4(1, 1, 0.97, 1.0);
        return;
    }

    // Rotation angle based on time (adjust rotation speed as needed)
    float angle = dist;  // Adjust speed of rotation

    mat3 rotationMatrix = rotateAroundAxis(vec3(0, 0, 1), spin_dir / (0.3 + dist) * 3);

    // Apply rotation
    vec3 rotated_coord = rotationMatrix * coord;

    // Use the Perlin noise to determine the planet's color: green for land, blue for water
    vec3 surface_color = base_color(rotated_coord, dist);

    // calculate brightness of fragment based on sun position

    vec3 color = surface_color;

    color = mix(color, detail_color(rotated_coord, dist, 1.0), 0.5);
    color = mix(color, subtle_detail_color(rotated_coord, dist), 0.5);

    for (int i = 0; i < 7; ++i) {
        color = mix(color, detail_color(rotated_coord, dist, 2.0 + i), 0.26);
    }

    vec3 negative_color = vec3(0);

    negative_color += negative_detail(rotated_coord, dist, 1.0);
    negative_color += negative_detail(rotated_coord, dist, 2.0);
    negative_color = clamp(negative_color, 0.0, 1.0);

    color = mix(color, vec3(0), negative_color);

    color *= 2.3 * (1 - pow(dist, 0.7));

    FragColor = vec4(color, 1);
}
