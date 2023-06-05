float4x4 WORLD; //world transform
float4x4 VIEW; //view transform
float4x4 PROJ; //projection transform
float4x4 TRANS; //world, transposed
float4x4 INV; //world, inverse
float4x4 INV_TRANSPOSE; //world inverse, transposed
float3 LIGHT_POS; //light position
float4 LIGHT_COLOR; //light color
float3 CAMERA_VIEW; //camera view vector

struct VertexInput
{
    float4 Position : POSITION;
    float4 Normal : NORMAL0;
    float2 TextureCoordinate : TEXCOORD1;
};

struct VertexOutput
{
    float4 Position : POSITION;
    float4 Color : COLOR0;
    float2 TextureCoordinate : TEXCOORD1;
    float2 junk : TEXCOORD5; //utter junk, just threw this in to stop the compiler being mean to me :(
};

VertexOutput vertexMain(VertexInput input)
{
    VertexOutput ret;
    float4 pos = mul(mul(mul(input.Position, WORLD), VIEW), PROJ);
    float4 normal = mul(input.Normal, INV_TRANSPOSE);
    float3 direction = input.Position - LIGHT_POS;
    float diffuseIntensity = length(direction);
    direction = normalize(direction);
    
    float intensity = dot(normal, direction);

    ret.Color = saturate(LIGHT_COLOR * intensity * -diffuseIntensity);
    ret.Position = pos;
    ret.TextureCoordinate = input.TextureCoordinate;
    
    //junk to make the compiler not optimize it away
    ret.junk = mul(CAMERA_VIEW, mul(TRANS, INV));
    
    return ret;
}

sampler2D textureSampler : register(s0);

float4 pixelMain(VertexOutput input) : COLOR0
{
    return input.Color;
}