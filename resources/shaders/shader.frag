#extension GL_ARB_shader_storage_buffer_object : require

layout (location = 0) in vec3 fragNormal;
layout (location = 1) in vec2 fragTexCoords;
layout (location = 2) in vec3 fragPos;
layout (location = 3) flat in uint instanceID;

layout (location = 0) out vec4 outFragColor;
layout (location = 1) out vec4 outBrightColor;

uniform sampler2D AmbientTexture;
uniform sampler2D DiffuseTexture;
uniform sampler2D BloomTexture;
uniform sampler2D BloomTextureAlpha;

layout (std140, binding = 0) uniform materialUniformBlock
{
    vec4    ambient;
    vec4    diffuse;
    vec4    bloom;
} material;


layout (std140, binding = 2) uniform lightUniformBlock
{
    vec3 ambient;
    vec3 diffuse;
    vec3 direction;
} light;

struct Model
{
    mat4 transform;
    vec4 color;
};

layout (std430, binding = 3) buffer modelUniformBlock
{
    Model model[];
};

vec4 getAmbient()
{
    #ifdef TEXTURE_AMBIENT
        return vec4(light.ambient, 1.0f)  * texture(AmbientTexture, fragTexCoords);
    #else
        return vec4(light.ambient, 1.0f) * material.ambient;
    #endif
}

vec4 getDiffuse(vec3 normal)
{
    normal = normalize(normal);
    float diff = max(dot(normal, -light.direction), 0.0);

    #ifdef TEXTURE_DIFFUSE
        return vec4(light.diffuse, 1.0f) * (diff * texture(DiffuseTexture, fragTexCoords));
    #else
        return vec4(light.diffuse, 1.0f) * (diff * material.diffuse);
    #endif
}

vec4 CalcFragColor(vec3 normal)
{
    // Ambient
    vec4 ambient = getAmbient() * model[instanceID].color;

    // Diffuse shading
    vec4 diffuse = getDiffuse(normal) * model[instanceID].color;

    return (ambient + diffuse);
}


void main()
{
    vec4 color = CalcFragColor(fragNormal);

    outFragColor = color;

    #ifdef BLOOM
        vec4 brightColor = vec4(0.0);
        // Use texture for bloom
        #ifdef TEXTURE_BLOOM
            vec4 bloomTextureColor = texture(BloomTexture, fragTexCoords);

            brightColor = bloomTextureColor;
        // Use color for bloom
        #else
            brightColor = material.bloom;
        #endif

        #ifdef TEXTURE_BLOOM_ALPHA
            vec4 bloomTextureAlpha = texture(BloomTextureAlpha, fragTexCoords);

            brightColor = brightColor * bloomTextureAlpha;
        #endif

        outBrightColor = brightColor;
    #else
        outBrightColor = vec4(0.0f);
    #endif
}
