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

float4 ShieldColor = float4(0, .8, 1, 0);

struct VertexInput
{
    float4 Position : POSITION;
    float4 Color : COLOR0;
};

struct VertexOutput
{
    float4 Position : POSITION;
    float4 Color : COLOR0;
};

VertexOutput vertexMain(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(mul(mul(input.Position, WORLD), VIEW), PROJ);
    
    float3 worldpos = mul(WORLD, input.Position);
    float distance = length(worldpos - LIGHT_POS);
    float alpha = 1 - min(distance / 100, 1);
    float4 color = ShieldColor;
    color.w = alpha;
    output.Color = color;
    
    return output;
}

float4 pixelMain(VertexOutput input) : COLOR0
{
    return input.Color;
}