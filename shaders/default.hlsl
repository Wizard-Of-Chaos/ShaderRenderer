float4x4 WORLD; //world transform
float4x4 VIEW; //view transform
float4x4 PROJ; //projection transform
float4x4 TRANS; //world, transposed
float4x4 INV; //world, inverse
float4x4 INV_TRANSPOSE; //world inverse, transposed
float3 LIGHT_POS; //light position
float4 LIGHT_DIFFUSE_COLOR; //light color
float4 LIGHT_AMBIENT_COLOR; //ambient light color
float4 LIGHT_SPECULAR_COLOR; //light specular color
float3 CAMERA_VIEW; //camera view vector

struct VertexInput
{
    float4 Position : POSITION;
    float4 Normal : NORMAL0;
    float2 TextureCoordinate : TEXCOORD0;
    float3 Tangent : TEXCOORD1; //IMPORTANT: Irrlicht passes in Tangent and Binormal information as TEXCOORD1 and TEXCOORD2. Wild.
    float3 Binormal : TEXCOORD2;
};

struct VertexOutput
{
    float4 Position : POSITION;
    float2 TextureCoordinate : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 Binormal : TEXCOORD3;
    
    float3 LightDirection : TEXCOORD4;
    float LightIntensity : TEXCOORD5;
    float2 junk : TEXCOORD6; //utter junk, just threw this in to stop the compiler being mean to me :(
};

VertexOutput vertexMain(VertexInput input)
{
    VertexOutput ret;
    ret.Position = mul(mul(mul(input.Position, WORLD), VIEW), PROJ);
    ret.Normal = normalize(mul(input.Normal, INV_TRANSPOSE));
    ret.Tangent = normalize(mul(input.Tangent, INV_TRANSPOSE));
    ret.Binormal = normalize(mul(input.Binormal, INV_TRANSPOSE));
    ret.TextureCoordinate = input.TextureCoordinate;
    
    float3 direction = LIGHT_POS - input.Position;
    direction = normalize(direction);
    
    float intensity = dot(ret.Normal, direction);
    ret.LightIntensity = intensity * 4;
    ret.LightDirection = direction;
    ret.TextureCoordinate = input.TextureCoordinate;
    
    //junk to make the compiler not optimize it away
    ret.junk = mul(CAMERA_VIEW, mul(TRANS, INV));
    
    return ret;
}

sampler2D textureSampler : register(s0);
sampler2D bumpSampler : register(s1);

float4 pixelMain(VertexOutput input) : COLOR0
{
    //.5 is the bump constant
    float3 bump = .5 * (tex2D(bumpSampler, input.TextureCoordinate) - (.5, .5, .5));
    float3 bumpNormal = input.Normal + (bump.x * input.Tangent + bump.y * input.Binormal);
    bumpNormal = normalize(bumpNormal);
    
    float intensity = dot(normalize(input.LightDirection), bumpNormal);
    if (intensity < 0)
        intensity = 0;
    
    float3 light = normalize(input.LightDirection);
    float3 r = normalize(2 * dot(light, bumpNormal) * bumpNormal - light);
    float3 v = normalize(mul(normalize(CAMERA_VIEW), WORLD));
    float dotRV = dot(r, v);
    
    //200 here is how shiny it is and the 1 is how intense the spec highlight is
    float4 spec = 1 * LIGHT_SPECULAR_COLOR * max(pow(dotRV, 200), 0) * input.LightIntensity;
    float4 texColor = tex2D(textureSampler, input.TextureCoordinate);
    texColor.a = 1;
    
    return saturate(texColor * (input.LightIntensity * LIGHT_DIFFUSE_COLOR) + spec + LIGHT_AMBIENT_COLOR);

}