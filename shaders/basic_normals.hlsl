float4x4 WORLD; //world transform
float4x4 VIEW; //view transform
float4x4 PROJ; //projection transform
float4x4 INV_TRANSPOSE; //world inverse, transposed

float3 CAMERA_VIEW; //camera view vector
float3 CAMERA_POS;
float RENDER_DISTANCE;

float4x4 LIGHTINFO_0;
float3 LIGHTRADCONE_0;
float4x4 LIGHTINFO_1;
float3 LIGHTRADCONE_1;
float4x4 LIGHTINFO_2;
float3 LIGHTRADCONE_2;
float4x4 LIGHTINFO_3;
float3 LIGHTRADCONE_3;
float4x4 LIGHTINFO_4;
float3 LIGHTRADCONE_4;
float4x4 LIGHTINFO_5;
float3 LIGHTRADCONE_5;
float4x4 LIGHTINFO_6;
float3 LIGHTRADCONE_6;
float4x4 LIGHTINFO_7;
float3 LIGHTRADCONE_7;

bool HAS_FOG;
float4 FOG_COLOR;
float FOG_MIN;
float FOG_MAX;

struct VertexInput
{
    float4 Position : POSITION;
    float4 Normal : NORMAL0;
    float2 TextureCoordinate : TEXCOORD0;
    float3 Tangent : TEXCOORD1; //IMPORTANT: Irrlicht passes in Tangent and Binormal information as TEXCOORD1 and TEXCOORD2. Wild.
    float3 Binormal : TEXCOORD2;
    float4 Color : COLOR0;
};

struct VertexOutput
{
    float4 Position : POSITION;
    float2 TextureCoordinate : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 Binormal : TEXCOORD3;
    float4 Color : COLOR0;
    float3 vertPosition : TEXCOORD4; //used to sneak info into the pixel shader
};

VertexOutput vertexMain(VertexInput input)
{
    VertexOutput ret;
    
    float percentageRenderDist = (RENDER_DISTANCE - length(CAMERA_POS - input.Position)) / RENDER_DISTANCE;
    float alpha = 1;
    if (percentageRenderDist > .8)
    {
        float percentTransparency = 1 - (percentageRenderDist - .8);
        alpha = percentTransparency;
    }
    ret.Color = input.Color;
    ret.Color.a = alpha;
    ret.Position = mul(mul(mul(input.Position, WORLD), VIEW), PROJ);
    ret.Normal = normalize(mul(input.Normal, INV_TRANSPOSE));
    ret.Tangent = normalize(mul(input.Tangent, INV_TRANSPOSE));
    ret.Binormal = normalize(mul(input.Binormal, INV_TRANSPOSE));
    ret.TextureCoordinate = input.TextureCoordinate;
    ret.vertPosition = ret.Position;
    
    return ret;
}

sampler2D textureSampler : register(s0);
sampler2D bumpSampler : register(s1);

float getDistanceIntensity(float radius, float distance)
{
    if (distance > radius)
        return 0;
    
    return 1 - ((distance - radius) / radius);
}

float getLightIntensity(float3 direction, float3 normal)
{
    float intensity = dot(normal, direction) * 10;
    if (intensity < 0)
        intensity = 0;
    return intensity;
}
float3 getLightDirection(float4x4 info)
{
    return normalize(float3(info[0].x, info[0].y, info[0].z));
}
float4 getLightDiffuse(float4x4 info)
{
    return info[1];
}
float4 getLightAmbient(float4x4 info)
{
    return info[2];
}
float4 getLightSpec(float4x4 info)
{
    return info[3];
}

float BUMP_CONST = .15;

float3 getBumpNormal(float3 normal, float2 texCoord, float3 tangent, float3 binormal)
{
    float3 bump = BUMP_CONST * (tex2D(bumpSampler, texCoord) - (.5, .5, .5));
    float3 bumpNormal = normal + (bump.x * tangent + bump.y * binormal);
    return normalize(bumpNormal);
}

//note: just the light color
float4 getLightSpecForPixel(float4x4 light, float2 texCoord, float3 normal, float3 tangent, float3 binormal, float intensity)
{
    if (light[0].w == -1)
    { //is this a bogus light?
        return float4(0, 0, 0, 0);
    }
    float3 dir = normalize(getLightDirection(light));
    float3 bumpNormal = getBumpNormal(normal, texCoord, tangent, binormal);
    float3 r = normalize(2 * dot(dir, bumpNormal) * bumpNormal - dir);
    float3 v = normalize(mul(normalize(CAMERA_VIEW), WORLD));
    float dotRV = dot(r, v);
    
    //5 here is how shiny it is and the 1 is how intense the spec highlight is
    float4 specColor = 1 * getLightSpec(light) * max(pow(dotRV, 5), 0) * intensity;
    return specColor;
}

float4 pixelMain(VertexOutput input) : COLOR0
{

    float4 texColor = tex2D(textureSampler, input.TextureCoordinate);
    texColor.a = 1;

    float4 diffuse;
    float4 spec;
    float4 amb;
    float intensity=0;
    
    float4 lightcolor = float4(0, 0, 0, 0);
    
    float3 bump = .15 * (tex2D(bumpSampler, input.TextureCoordinate) - (.5, .5, .5));
    float3 bumpNormal = input.Normal + (bump.x * input.Tangent + bump.y * input.Binormal);
    bumpNormal = normalize(bumpNormal);
    
    if (LIGHTINFO_0[0].w >= 0)
    {
        float intensity = dot(getLightDirection(LIGHTINFO_0), bumpNormal) * getDistanceIntensity(LIGHTRADCONE_0.x, LIGHTINFO_0[0].w);
        if (intensity < 0)
            intensity = 0;
        
        diffuse = intensity * getLightDiffuse(LIGHTINFO_0);
        spec = getLightSpecForPixel(LIGHTINFO_0, input.TextureCoordinate, input.Normal, input.Tangent, input.Binormal, intensity);
        amb = getLightAmbient(LIGHTINFO_0);
        lightcolor = saturate(lightcolor + diffuse + spec + amb);
    }
    if (LIGHTINFO_1[0].w >= 0)
    {
        float intensity = dot(getLightDirection(LIGHTINFO_1), bumpNormal) * getDistanceIntensity(LIGHTRADCONE_1.x, LIGHTINFO_1[0].w);
        if (intensity < 0)
            intensity = 0;
        
        diffuse = intensity * getLightDiffuse(LIGHTINFO_1);
        spec = getLightSpecForPixel(LIGHTINFO_1, input.TextureCoordinate, input.Normal, input.Tangent, input.Binormal, intensity);
        amb = getLightAmbient(LIGHTINFO_1);
        lightcolor = saturate(lightcolor + diffuse + spec + amb);
    }
    if (LIGHTINFO_2[0].w >= 0)
    {
        float intensity = dot(getLightDirection(LIGHTINFO_2), bumpNormal) * getDistanceIntensity(LIGHTRADCONE_2.x, LIGHTINFO_2[0].w);
        if (intensity < 0)
            intensity = 0;
        
        diffuse = intensity * getLightDiffuse(LIGHTINFO_2);
        spec = getLightSpecForPixel(LIGHTINFO_2, input.TextureCoordinate, input.Normal, input.Tangent, input.Binormal, intensity);
        amb = getLightAmbient(LIGHTINFO_2);
        lightcolor = saturate(lightcolor + diffuse + spec + amb);
    }
    if (LIGHTINFO_3[0].w >= 0)
    {
        float intensity = dot(getLightDirection(LIGHTINFO_3), bumpNormal) * getDistanceIntensity(LIGHTRADCONE_3.x, LIGHTINFO_3[0].w);
        if (intensity < 0)
            intensity = 0;
        
        diffuse = intensity * getLightDiffuse(LIGHTINFO_3);
        spec = getLightSpecForPixel(LIGHTINFO_3, input.TextureCoordinate, input.Normal, input.Tangent, input.Binormal, intensity);
        amb = getLightAmbient(LIGHTINFO_3);
        lightcolor = saturate(lightcolor + diffuse + spec + amb);
    }
    if (LIGHTINFO_4[0].w >= 0)
    {
        float intensity = dot(getLightDirection(LIGHTINFO_4), bumpNormal) * getDistanceIntensity(LIGHTRADCONE_4.x, LIGHTINFO_4[0].w);
        if (intensity < 0)
            intensity = 0;
        
        diffuse = intensity * getLightDiffuse(LIGHTINFO_4);
        spec = getLightSpecForPixel(LIGHTINFO_4, input.TextureCoordinate, input.Normal, input.Tangent, input.Binormal, intensity);
        amb = getLightAmbient(LIGHTINFO_4);
        lightcolor = saturate(lightcolor + diffuse + spec + amb);
    }
    if (LIGHTINFO_5[0].w >= 0)
    {
        float intensity = dot(getLightDirection(LIGHTINFO_5), bumpNormal) * getDistanceIntensity(LIGHTRADCONE_5.x, LIGHTINFO_5[0].w);
        if (intensity < 0)
            intensity = 0;
        
        diffuse = intensity * getLightDiffuse(LIGHTINFO_5);
        spec = getLightSpecForPixel(LIGHTINFO_5, input.TextureCoordinate, input.Normal, input.Tangent, input.Binormal, intensity);
        amb = getLightAmbient(LIGHTINFO_5);
        lightcolor = saturate(lightcolor + diffuse + spec + amb);
    }
    if (LIGHTINFO_6[0].w >= 0)
    {
        float intensity = dot(getLightDirection(LIGHTINFO_6), bumpNormal) * getDistanceIntensity(LIGHTRADCONE_6.x, LIGHTINFO_6[0].w);
        if (intensity < 0)
            intensity = 0;
        
        diffuse = intensity * getLightDiffuse(LIGHTINFO_6);
        spec = getLightSpecForPixel(LIGHTINFO_6, input.TextureCoordinate, input.Normal, input.Tangent, input.Binormal, intensity);
        amb = getLightAmbient(LIGHTINFO_6);
        lightcolor = saturate(lightcolor + diffuse + spec + amb);
    }
    if (LIGHTINFO_7[0].w >= 0)
    {
        float intensity = dot(getLightDirection(LIGHTINFO_7), bumpNormal) * getDistanceIntensity(LIGHTRADCONE_7.x, LIGHTINFO_7[0].w);
        if (intensity < 0)
            intensity = 0;
        
        diffuse = intensity * getLightDiffuse(LIGHTINFO_7);
        spec = getLightSpecForPixel(LIGHTINFO_7, input.TextureCoordinate, input.Normal, input.Tangent, input.Binormal, intensity);
        amb = getLightAmbient(LIGHTINFO_7);
        lightcolor = saturate(lightcolor + diffuse + spec + amb);
    }
    float4 finalColor = saturate(texColor * lightcolor);
    
    if (HAS_FOG)
    {
        float dist = distance(input.vertPosition, CAMERA_POS);
        if (dist > FOG_MAX)
            return FOG_COLOR;
        if (dist < FOG_MIN)
            return finalColor;
        
        float ratio = (dist - FOG_MIN) / (FOG_MAX - FOG_MIN);
        return lerp(finalColor, FOG_COLOR, ratio);

    }
    return finalColor;
    

}