#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 diffuse_color;
in vec3 ambient_color;
in vec3 specular_color;


// Values that stay constant for the whole mesh.
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;
uniform sampler2D myTextureSampler;

uniform highp sampler2D vertex_pos_texture;
uniform highp int  vertex_rows_uniform;
uniform highp int data_texture_width_uniform;
uniform highp int triangle_number_uniform;

vec3 camera_origin = vec3(0, 0, 0);
vec3 camera_lower_left_corner = vec3(-2.0, -1.0, -1.0);
vec3 camera_horizontal = vec3(4.0, 0.0, 0.0);
vec3 camera_vertical = vec3(0.0, 2.0, 0.0);

struct ray {
    highp vec3 P;
    mediump vec3 D;
};

struct surface_hit
{
    highp float t;
    highp float triangle_number;
    mediump vec3 uvw;
    mediump vec3 normal;
};

struct range
{
    highp float t0, t1;
};

vec2 index_to_sample(highp int vertex_number, highp int width, highp int height)
{
    float i = (vertex_number % width) / width;
    float j = (vertex_number / width) / height;
    mediump vec2 samp = vec2(i, j);
    return samp;
}

bool triangle_intersect(highp int triangle_number, in ray theray, in range r, inout surface_hit hit)
{
    highp vec3 v0, v1, v2;
    v0 = texture(vertex_pos_texture, index_to_sample(triangle_number * 3 + 0, data_texture_width_uniform, vertex_rows_uniform)).xyz;
    v1 = texture(vertex_pos_texture, index_to_sample(triangle_number * 3 + 1, data_texture_width_uniform, vertex_rows_uniform)).xyz;
    v2 = texture(vertex_pos_texture, index_to_sample(triangle_number * 3 + 2, data_texture_width_uniform, vertex_rows_uniform)).xyz;
    hit.uvw = v0;
    return true;

    highp vec3 e0 = v1 - v0;
    highp vec3 e1 = v0 - v2;
    highp vec3 e2 = v2 - v1;

    highp vec3 M = cross(e1, theray.D);

    highp float det = dot(e0, M);

    const highp float epsilon = 0.0000001;// .000001 from M-T paper too large for bunny
    if (det > -epsilon && det < epsilon) {
        return false;
    }

    float inv_det = 1.0 / det;

    // Do this in a somewhat different order than M-T in order to early out
    // if previous intersection is closer than this intersection
    highp vec3 T = theray.P - v0;
    highp vec3 Q = cross(T, e0);
    highp float d = -dot(e1, Q) * inv_det;
    if (d > hit.t) {
        return false;
    }
    if (d < r.t0 || d > r.t1) {
        return false;
    }

    mediump float u = dot(T, M) * inv_det;
    if (u < 0.0 || u > 1.0) {
        return false;
    }

    mediump float v = dot(theray.D, Q) * inv_det;
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    hit.triangle_number = triangle_number;
    hit.t = d;
    hit.uvw[0] = 1.0 - u - v;
    hit.uvw[1] = u;
    hit.uvw[2] = v;
    hit.normal = M;

    return true;
}


bool world_hit(ray r, float t_min, float t_max, out surface_hit hit) {
    surface_hit temp_hit;
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (int i = 0; i < triangle_number_uniform; i++) {
        if (triangle_intersect(i, r, range(t_min, closest_so_far), temp_hit)) {
            hit_anything = true;
            hit = temp_hit;
            closest_so_far = temp_hit.t;
        }
    }

    return hit_anything;
}

/* returns a varying number between 0 and 1 */
float drand48(vec2 co) {
    return 2 * fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453) - 1;
}

vec3 random_in_unit(vec3 p) {
    int n = 0;
    do {
        p = vec3(drand48(p.xy), drand48(p.zy), drand48(p.xz));
        n++;
    } while(length(p) * length(p) >= 1.0 && n < 3);
    return p;
}

bool lambertian_scatter(in ray r, in surface_hit hit, out vec3 attenuation, out ray scattered) {
    vec3 target = hit.uvw + hit.normal + random_in_unit(hit.uvw);
    scattered = ray(hit.uvw, target - hit.uvw);
//    attenuation = hit.uvw; // for albedo
    attenuation = vec3(1.0, 0.0, 0.0); // for albedo
    return true;
}

bool dispatch_scatter(in ray r, surface_hit hit, out vec3 attenuation, out ray scattered) {
    return lambertian_scatter(r, hit, attenuation, scattered);
}

vec3 color(ray r) {
    surface_hit hit;
    vec3 col = vec3(0, 0, 0); /* visible color */
    vec3 total_attenuation = vec3(1.0, 1.0, 1.0); /* reduction of light transmission */

    for (int bounce = 0; bounce < 4; bounce++) {

        if (world_hit(r, 0.001, 1000000.0, hit)) {
            /* create a new reflected ray */
            return hit.uvw;
            ray scattered;
            vec3 local_attenuation;

            if (dispatch_scatter(r, hit, local_attenuation, scattered)) {
                total_attenuation *= local_attenuation;
                r = scattered;
            } else {
                total_attenuation *= vec3(0, 0, 0);
            }
        }
//        else {
//            /* background hit (light source) */
//            vec3 unit_dir = normalize(r.D);
//            float t = 0.5 * (unit_dir.y + 1.0);
//            col = total_attenuation * ((1.0-t)*vec3(1.0, 1.0, 1.0)+t*vec3(0.5, 0.7, 1.0));
//            break;
//        }
    }
    return col;
}

vec3 random_in_unit_disk(vec2 co) {
    vec3 p;
    int n = 0;
    do {
        p = vec3(drand48(co.xy), drand48(co.yx), 0);
        n++;
    } while (dot(p,p) >= 1.0 && n < 3);
    return p;
}

ray get_ray(float u, float v) {
//    vec3 rd = random_in_unit_disk(vec2(s,t));
//    vec3 offset = vec3(s * rd.x, t * rd.y, 0);
//    return ray(camera_origin + offset, camera_lower_left_corner + s * camera_horizontal + t * camera_vertical - camera_origin - offset);

    ray eyeray;

    // u is 0.0 at left, 1.0 at right
    // v is 0.0 at bottom, 1.0 at top
    eyeray.D = normalize(vec3(1280 * (u - 0.5), 1280 * (v - 0.5) * 1280.0 / 768.0, -1.0));
    eyeray.P = vec3(0.0, 0.0, 0.0);

    return eyeray;
}

void main() {
    vec3 col = vec3(0, 0, 0);
    vec2 window_size = vec2(1280.0, 768.0);
    float u, v;
    ray r;
    const int nsamples = 1;
    for (int s = 0; s < nsamples; s++) {
        u = (gl_FragCoord.x + drand48(col.xy + s)) / window_size.x;
        v = (gl_FragCoord.y + drand48(col.xz + s)) / window_size.y;
        r = get_ray(u, v);
        col += color(r);
    }
    col /= nsamples;
    col = vec3(sqrt(col.x), sqrt(col.y), sqrt(col.z));

    gl_FragColor = vec4(col, 1.0);
}


//void main(){
//
//    // Light emission properties
//    // You probably want to put them as uniforms
//    vec3 LightColor = vec3(1, 1, 1);
//    float LightPower = 50.0f;
//
//    // Material properties
//    //    vec3 MaterialDiffuseColor = texture(myTextureSampler, UV).rgb;
//    //    vec3 MaterialAmbientColor = vec3(0.1, 0.1, 0.1) * MaterialDiffuseColor;
//    //    vec3 MaterialSpecularColor = vec3(0.3, 0.3, 0.3);
//
//    vec3 MaterialDiffuseColor = diffuse_color;
//    vec3 MaterialAmbientColor = MaterialDiffuseColor * vec3(0.1, 0.1, 0.1);
//    vec3 MaterialSpecularColor = specular_color;
//
//    // Distance to the light
//    float distance = length(LightPosition_worldspace - Position_worldspace);
//
//    // Normal of the computed fragment, in camera space
//    vec3 n = normalize(Normal_cameraspace);
//    // Direction of the light (from the fragment to the light)
//    vec3 l = normalize(LightDirection_cameraspace);
//    // Cosine of the angle between the normal and the light direction,
//    // clamped above 0
//    //  - light is at the vertical of the triangle -> 1
//    //  - light is perpendicular to the triangle -> 0
//    //  - light is behind the triangle -> 0
//    float cosTheta = clamp(dot(n, l), 0, 1);
//
//    // Eye vector (towards the camera)
//    vec3 E = normalize(EyeDirection_cameraspace);
//    // Direction in which the triangle reflects the light
//    vec3 R = reflect(-l, n);
//    // Cosine of the angle between the Eye vector and the Reflect vector,
//    // clamped to 0
//    //  - Looking into the reflection -> 1
//    //  - Looking elsewhere -> < 1
//    float cosAlpha = clamp(dot(E, R), 0, 1);
//
//        color =
//        // Ambient : simulates indirect lighting
//        MaterialAmbientColor * LightColor +
//        // Diffuse : "color" of the object
//        MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
//        // Specular : reflective highlight, like a mirror
//        MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha, 5) / (distance*distance);
//
////    color = MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) + MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha, 5) / (distance*distance);
//
//}