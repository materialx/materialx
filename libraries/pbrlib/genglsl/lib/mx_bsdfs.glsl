#include "pbrlib/genglsl/lib/mx_refraction_index.glsl"

// https://www.graphics.rwth-aachen.de/publication/2/jgt.pdf
float mx_golden_ratio_sequence(int i)
{
    return fract((float(i) + 1.0) * M_GOLDEN_RATIO);
}

// https://people.irisa.fr/Ricardo.Marques/articles/2013/SF_CGF.pdf
vec2 mx_spherical_fibonacci(int i, int numSamples)
{
    return vec2((float(i) + 0.5) / float(numSamples), mx_golden_ratio_sequence(i));
}

float mx_average_roughness(vec2 roughness)
{
    return sqrt(roughness.x * roughness.y);
}

float mx_fresnel_schlick(float cosTheta, float F0, float F90, float exponent)
{
    float x = clamp(1.0 - cosTheta, 0.0, 1.0);
    return mix(F0, F90, pow(x, exponent));
}

vec3 mx_fresnel_schlick(float cosTheta, vec3 F0, vec3 F90, float exponent)
{
    float x = clamp(1.0 - cosTheta, 0.0, 1.0);
    return mix(F0, F90, pow(x, exponent));
}

float mx_fresnel_schlick(float cosTheta, float ior)
{
    float x = clamp(1.0 - cosTheta, 0.0, 1.0);
    float x5 = mx_pow5(x);
    float F0 = mx_ior_to_f0(ior);
    return F0 + (1.0 - F0) * x5;
}

// https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
float mx_fresnel_dielectric(float cosTheta, float ior)
{
    if (cosTheta < 0.0)
        return 1.0;

    float g =  ior*ior + cosTheta*cosTheta - 1.0;
    // Check for total internal reflection
    if (g < 0.0)
        return 1.0;

    g = sqrt(g);
    float gmc = g - cosTheta;
    float gpc = g + cosTheta;
    float x = gmc / gpc;
    float y = (gpc * cosTheta - 1.0) / (gmc * cosTheta + 1.0);
    return 0.5 * x * x * (1.0 + y * y);
}

vec3 mx_fresnel_conductor(float cosTheta, vec3 n, vec3 k)
{
   float c2 = cosTheta*cosTheta;
   vec3 n2_k2 = n*n + k*k;
   vec3 nc2 = 2.0 * n * cosTheta;

   vec3 rs_a = n2_k2 + c2;
   vec3 rp_a = n2_k2 * c2 + 1.0;
   vec3 rs = (rs_a - nc2) / (rs_a + nc2);
   vec3 rp = (rp_a - nc2) / (rp_a + nc2);

   return 0.5 * (rs + rp);
}

float mx_orennayar(vec3 L, vec3 V, vec3 N, float NdotL, float roughness)
{
    float LdotV = dot(L, V);
    float NdotV = dot(N, V);

    float t = LdotV - NdotL * NdotV;
    t = t > 0.0 ? t / max(NdotL, NdotV) : 0.0;

    float sigma2 = mx_square(roughness * M_PI);
    float A = 1.0 - 0.5 * (sigma2 / (sigma2 + 0.33));
    float B = 0.45f * sigma2 / (sigma2 + 0.09);

    return A + B * t;
}

// https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf
// Appendix B.2 Equation 13
float mx_ggx_NDF(vec3 X, vec3 Y, vec3 H, float NdotH, float alphaX, float alphaY)
{
    float XdotH = dot(X, H);
    float YdotH = dot(Y, H);
    float denom = mx_square(XdotH / alphaX) + mx_square(YdotH / alphaY) + mx_square(NdotH);
    return 1.0 / (M_PI * alphaX * alphaY * mx_square(denom));
}

// https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf
// Appendix B.1 Equation 3
float mx_ggx_PDF(vec3 X, vec3 Y, vec3 H, float NdotH, float LdotH, float alphaX, float alphaY)
{
    return mx_ggx_NDF(X, Y, H, NdotH, alphaX, alphaY) * NdotH / (4.0 * LdotH);
}

// https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf
// Appendix B.2 Equation 15
vec3 mx_ggx_importance_sample_NDF(vec2 Xi, vec3 X, vec3 Y, vec3 N, float alphaX, float alphaY)
{
    float phi = 2.0 * M_PI * Xi.x;
    float tanTheta = sqrt(Xi.y / (1.0 - Xi.y));
    vec3 H = X * (tanTheta * alphaX * cos(phi)) +
             Y * (tanTheta * alphaY * sin(phi)) +
             N;
    return normalize(H);
}

// http://jcgt.org/published/0003/02/03/paper.pdf
// Equations 72 and 99
float mx_ggx_smith_G(float NdotL, float NdotV, float alpha)
{
    float alpha2 = mx_square(alpha);
    float lambdaL = sqrt(alpha2 + (1.0 - alpha2) * mx_square(NdotL));
    float lambdaV = sqrt(alpha2 + (1.0 - alpha2) * mx_square(NdotV));
    return 2.0 / (lambdaL / NdotL + lambdaV / NdotV);
}

// https://www.unrealengine.com/blog/physically-based-shading-on-mobile
vec3 mx_ggx_directional_albedo_curve_fit(float NdotV, float roughness, vec3 F0, vec3 F90)
{
    const vec4 c0 = vec4(-1, -0.0275, -0.572, 0.022);
    const vec4 c1 = vec4( 1,  0.0425,  1.04, -0.04 );
    vec4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NdotV)) * r.x + r.y;
    vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
    return F0 * AB.x + F90 * AB.y;
}

vec3 mx_ggx_directional_albedo_table_lookup(float NdotV, float roughness, vec3 F0, vec3 F90)
{
#if DIRECTIONAL_ALBEDO_METHOD == 1
    vec2 res = textureSize($albedoTable, 0);
    if (res.x > 0)
    {
        vec2 AB = texture($albedoTable, vec2(NdotV, roughness)).rg;
        return F0 * AB.x + F90 * AB.y;
    }
#endif
    return vec3(0.0);
}

// https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
vec3 mx_ggx_directional_albedo_importance_sample(float NdotV, float roughness, vec3 F0, vec3 F90)
{
    NdotV = clamp(NdotV, 1e-8, 1.0);
    vec3 V = vec3(sqrt(1.0f - mx_square(NdotV)), 0, NdotV);

    vec2 AB = vec2(0.0);
    const int SAMPLE_COUNT = 64;
    for (int i = 0; i < SAMPLE_COUNT; i++)
    {
        vec2 Xi = mx_spherical_fibonacci(i, SAMPLE_COUNT);

        // Compute the half vector and incoming light direction.
        vec3 H = mx_ggx_importance_sample_NDF(Xi, vec3(-1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), roughness, roughness);
        vec3 L = -reflect(V, H);
        
        // Compute dot products for this sample.
        float NdotL = clamp(L.z, 1e-8, 1.0);
        float NdotH = clamp(H.z, 1e-8, 1.0);
        float VdotH = clamp(dot(V, H), 1e-8, 1.0);

        // Compute the Fresnel term.
        float F = mx_pow5(1 - VdotH);

        // Compute the geometric visibility term.
        float Gvis = mx_ggx_smith_G(NdotL, NdotV, roughness) * VdotH / (NdotH * NdotV);
        
        // Add the contribution of this sample.
        AB += vec2(Gvis * (1 - F), Gvis * F);
    }

    // Normalize integrated terms.
    AB /= SAMPLE_COUNT;

    // Return the final directional albedo.
    return F0 * AB.x + F90 * AB.y;
}

vec3 mx_ggx_directional_albedo(float NdotV, float roughness, vec3 F0, vec3 F90)
{
#if DIRECTIONAL_ALBEDO_METHOD == 0
    return mx_ggx_directional_albedo_curve_fit(NdotV, roughness, F0, F90);
#elif DIRECTIONAL_ALBEDO_METHOD == 1
    return mx_ggx_directional_albedo_table_lookup(NdotV, roughness, F0, F90);
#else
    return mx_ggx_directional_albedo_importance_sample(NdotV, roughness, F0, F90);
#endif
}

float mx_ggx_directional_albedo(float NdotV, float roughness, float ior)
{
    return mx_ggx_directional_albedo(NdotV, roughness, vec3(mx_ior_to_f0(ior)), vec3(1.0)).x;
}

// https://blog.selfshadow.com/publications/turquin/ms_comp_final.pdf
// Equations 14 and 16
vec3 mx_ggx_energy_compensation(float NdotV, float roughness, vec3 Fss)
{
    float Ess = mx_ggx_directional_albedo(NdotV, roughness, vec3(1.0), vec3(1.0)).x;
    return 1.0 + Fss * (1.0 - Ess) / Ess;
}

float mx_ggx_energy_compensation(float NdotV, float roughness, float Fss)
{
    return mx_ggx_energy_compensation(NdotV, roughness, vec3(Fss)).x;
}

// http://blog.selfshadow.com/publications/s2017-shading-course/imageworks/s2017_pbs_imageworks_sheen.pdf (Equation 2)
float mx_imageworks_sheen_NDF(float cosTheta, float roughness)
{
    // Given roughness is assumed to be clamped to [M_FLOAT_EPS, 1.0]
    float invRoughness = 1.0 / roughness;
    float cos2 = cosTheta * cosTheta;
    float sin2 = 1.0 - cos2;
    return (2.0 + invRoughness) * pow(sin2, invRoughness * 0.5) / (2.0 * M_PI);
}

// LUT for sheen directional albedo. 
// A 2D table parameterized with 'cosTheta' (cosine of angle to normal) on x-axis and 'roughness' on y-axis.
#define SHEEN_ALBEDO_TABLE_SIZE 16
const float u_sheenAlbedo[SHEEN_ALBEDO_TABLE_SIZE*SHEEN_ALBEDO_TABLE_SIZE] = float[](
    1.6177, 0.978927, 0.618938, 0.391714, 0.245177, 0.150234, 0.0893475, 0.0511377, 0.0280191, 0.0144204, 0.00687674, 0.00295935, 0.00111049, 0.000336768, 7.07119e-05, 6.22646e-06,
    1.1084, 0.813928, 0.621389, 0.479304, 0.370299, 0.284835, 0.21724, 0.163558, 0.121254, 0.0878921, 0.0619052, 0.0419894, 0.0270556, 0.0161443, 0.00848212, 0.00342323,
    0.930468, 0.725652, 0.586532, 0.479542, 0.393596, 0.322736, 0.26353, 0.213565, 0.171456, 0.135718, 0.105481, 0.0800472, 0.0588117, 0.0412172, 0.0268329, 0.0152799,
    0.833791, 0.671201, 0.558957, 0.471006, 0.398823, 0.337883, 0.285615, 0.240206, 0.200696, 0.16597, 0.135422, 0.10859, 0.0850611, 0.0644477, 0.0464763, 0.0308878,
    0.771692, 0.633819, 0.537877, 0.461939, 0.398865, 0.344892, 0.297895, 0.256371, 0.219562, 0.186548, 0.156842, 0.130095, 0.10598, 0.0841919, 0.0645311, 0.04679,
    0.727979, 0.606373, 0.52141, 0.453769, 0.397174, 0.348337, 0.305403, 0.267056, 0.232655, 0.201398, 0.17286, 0.146756, 0.122808, 0.100751, 0.0804254, 0.0616485,
    0.695353, 0.585281, 0.508227, 0.44667, 0.394925, 0.350027, 0.310302, 0.274561, 0.242236, 0.212604, 0.185281, 0.16002, 0.13657, 0.114693, 0.0942543, 0.0750799,
    0.669981, 0.568519, 0.497442, 0.440542, 0.392567, 0.350786, 0.313656, 0.280075, 0.249533, 0.221359, 0.195196, 0.170824, 0.148012, 0.126537, 0.106279, 0.0870713,
    0.649644, 0.554855, 0.488453, 0.435237, 0.390279, 0.351028, 0.316036, 0.284274, 0.255266, 0.228387, 0.203297, 0.179796, 0.157665, 0.136695, 0.116774, 0.0977403,
    0.632951, 0.543489, 0.480849, 0.430619, 0.388132, 0.350974, 0.317777, 0.287562, 0.259885, 0.234153, 0.210041, 0.187365, 0.165914, 0.145488, 0.125983, 0.10724,
    0.61899, 0.533877, 0.47433, 0.426573, 0.386145, 0.35075, 0.319078, 0.290197, 0.263681, 0.238971, 0.215746, 0.193838, 0.173043, 0.153167, 0.134113, 0.115722,
    0.607131, 0.52564, 0.468678, 0.423001, 0.38432, 0.35043, 0.320072, 0.292349, 0.266856, 0.243055, 0.220636, 0.199438, 0.179264, 0.159926, 0.141332, 0.123323,
    0.596927, 0.518497, 0.463731, 0.419829, 0.382647, 0.350056, 0.320842, 0.294137, 0.269549, 0.246564, 0.224875, 0.204331, 0.18474, 0.165919, 0.147778, 0.130162,
    0.588052, 0.512241, 0.459365, 0.416996, 0.381114, 0.349657, 0.321448, 0.295641, 0.271862, 0.24961, 0.228584, 0.208643, 0.189596, 0.171266, 0.153566, 0.136341,
    0.580257, 0.506717, 0.455481, 0.41445, 0.379708, 0.34925, 0.321929, 0.296923, 0.273869, 0.252279, 0.231859, 0.212472, 0.193933, 0.176066, 0.158788, 0.141945,
    0.573355, 0.5018, 0.452005, 0.412151, 0.378416, 0.348844, 0.322316, 0.298028, 0.275627, 0.254638, 0.234772, 0.215896, 0.197828, 0.180398, 0.163522, 0.147049
);

float mx_imageworks_sheen_directional_albedo(float cosTheta, float roughness)
{
    float x = cosTheta  * (SHEEN_ALBEDO_TABLE_SIZE - 1);
    float y = roughness * (SHEEN_ALBEDO_TABLE_SIZE - 1);
    int ix = int(x);
    int iy = int(y);
    int ix2 = clamp(ix + 1, 0, SHEEN_ALBEDO_TABLE_SIZE - 1);
    int iy2 = clamp(iy + 1, 0, SHEEN_ALBEDO_TABLE_SIZE - 1);
    float fx = x - ix;
    float fy = y - iy;

    // Bi-linear interpolation of the LUT values
    float v1 = mix(u_sheenAlbedo[iy  * SHEEN_ALBEDO_TABLE_SIZE + ix], u_sheenAlbedo[iy  * SHEEN_ALBEDO_TABLE_SIZE + ix2], fx);
    float v2 = mix(u_sheenAlbedo[iy2 * SHEEN_ALBEDO_TABLE_SIZE + ix], u_sheenAlbedo[iy2 * SHEEN_ALBEDO_TABLE_SIZE + ix2], fx);
    float albedo = mix(v1, v2, fy);

    return clamp(albedo, 0.0, 1.0);
}

// https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf
// Section 5.3
float mx_burley_diffuse(vec3 L, vec3 V, vec3 N, float NdotL, float roughness)
{
    vec3 H = normalize(L + V);
    float LdotH = max(dot(L, H), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    float F90 = 0.5 + (2.0 * roughness * mx_square(LdotH));
    float refL = mx_fresnel_schlick(NdotL, 1.0, F90, 5.0);
    float refV = mx_fresnel_schlick(NdotV, 1.0, F90, 5.0);
    return refL * refV * M_PI_INV;
}

// Compute the directional albedo component of Burley diffuse for the given
// view angle and roughness.  Curve fit provided by Stephen Hill.
float mx_burley_directional_albedo(vec3 V, vec3 N, float roughness)
{
    float x = dot(N, V);
    float fit0 = 0.97619 - 0.488095 * mx_pow5(1 - x);
    float fit1 = 1.55754 + (-2.02221 + (2.56283 - 1.06244 * x) * x) * x;
    return mix(fit0, fit1, roughness);
}
