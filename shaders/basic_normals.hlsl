float4x4 WORLD; //world transform
float4x4 VIEW; //view transform
float4x4 PROJ; //projection transform
float4x4 INV_TRANSPOSE; //world inverse, transposed

float3 CAMERA_VIEW; //camera view vector
float3 CAMERA_POS;
float RENDER_DISTANCE;

bool LIGHTVALID_0;
float3 LIGHTRADCONE_0;
float3 LIGHTDIR_0;
float3 LIGHTPOS_0;
float4 LIGHTDIFF_0;
float4 LIGHTAMB_0;
float4 LIGHTSPEC_0;

bool LIGHTVALID_1;
float3 LIGHTRADCONE_1;
float3 LIGHTDIR_1;
float3 LIGHTPOS_1;
float4 LIGHTDIFF_1;
float4 LIGHTAMB_1;
float4 LIGHTSPEC_1;

bool LIGHTVALID_2;
float3 LIGHTRADCONE_2;
float3 LIGHTDIR_2;
float3 LIGHTPOS_2;
float4 LIGHTDIFF_2;
float4 LIGHTAMB_2;
float4 LIGHTSPEC_2;

bool LIGHTVALID_3;
float3 LIGHTRADCONE_3;
float3 LIGHTDIR_3;
float3 LIGHTPOS_3;
float4 LIGHTDIFF_3;
float4 LIGHTAMB_3;
float4 LIGHTSPEC_3;

bool LIGHTVALID_4;
float3 LIGHTRADCONE_4;
float3 LIGHTDIR_4;
float3 LIGHTPOS_4;
float4 LIGHTDIFF_4;
float4 LIGHTAMB_4;
float4 LIGHTSPEC_4;

bool LIGHTVALID_5;
float3 LIGHTRADCONE_5;
float3 LIGHTDIR_5;
float3 LIGHTPOS_5;
float4 LIGHTDIFF_5;
float4 LIGHTAMB_5;
float4 LIGHTSPEC_5;

bool LIGHTVALID_6;
float3 LIGHTRADCONE_6;
float3 LIGHTDIR_6;
float3 LIGHTPOS_6;
float4 LIGHTDIFF_6;
float4 LIGHTAMB_6;
float4 LIGHTSPEC_6;

bool LIGHTVALID_7;
float3 LIGHTRADCONE_7;
float3 LIGHTDIR_7;
float3 LIGHTPOS_7;
float4 LIGHTDIFF_7;
float4 LIGHTAMB_7;
float4 LIGHTSPEC_7;

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
    float3 viewPosition : TEXCOORD5;
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
    ret.vertPosition = mul(input.Position, WORLD);
    ret.viewPosition = ret.Position;
    
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

float BUMP_CONST = .15;

float3 getBumpNormal(float3 normal, float2 texCoord, float3 tangent, float3 binormal)
{
    float3 bump = BUMP_CONST * (tex2D(bumpSampler, texCoord) - (.5, .5, .5));
    float3 bumpNormal = normal + (bump.x * tangent + bump.y * binormal);
    return normalize(bumpNormal);
}

//note: just the light color
float4 getLightSpecForPixel(float4 lightSpecColor, float3 direction, float2 texCoord, float3 normal, float3 tangent, float3 binormal, float intensity)
{
    float3 dir = normalize(direction);
    float3 bumpNormal = getBumpNormal(normal, texCoord, tangent, binormal);
    float3 r = normalize(2 * dot(dir, bumpNormal) * bumpNormal - dir);
    float3 v = normalize(mul(normalize(CAMERA_VIEW), WORLD));
    float dotRV = dot(r, v);
    
    //5 here is how shiny it is and the 1 is how intense the spec highlight is
    float4 specColor = 1 * lightSpecColor * max(pow(dotRV, 5), 0) * intensity;
    return specColor;
}

float4 getLightColor(VertexOutput input, float3 bumpNormal, float4 initColor, float radius, float3 position, float3 direction, float4 diff, float4 amb, float4 specular)
{
    float intensity = dot(direction, bumpNormal) * getDistanceIntensity(radius, distance(position, input.vertPosition));
    if (intensity < 0)
        intensity = 0;
        
    float4 diffuse = intensity * diff;
    float4 spec = getLightSpecForPixel(specular, direction, input.TextureCoordinate, input.Normal, input.Tangent, input.Binormal, intensity);
    return saturate(initColor + diffuse + spec + amb);
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
    
    if (LIGHTVALID_0)
    {
        lightcolor = getLightColor(input, bumpNormal, lightcolor, LIGHTRADCONE_0.x, LIGHTPOS_0, LIGHTDIR_0, LIGHTDIFF_0, LIGHTAMB_0, LIGHTSPEC_0);
    }
    if (LIGHTVALID_1)
    {
        lightcolor = getLightColor(input, bumpNormal, lightcolor, LIGHTRADCONE_1.x, LIGHTPOS_1, LIGHTDIR_1, LIGHTDIFF_1, LIGHTAMB_1, LIGHTSPEC_1);
    }
    if (LIGHTVALID_2)
    {
        lightcolor = getLightColor(input, bumpNormal, lightcolor, LIGHTRADCONE_2.x, LIGHTPOS_2, LIGHTDIR_2, LIGHTDIFF_2, LIGHTAMB_2, LIGHTSPEC_2);
    }
    if (LIGHTVALID_3)
    {
        lightcolor = getLightColor(input, bumpNormal, lightcolor, LIGHTRADCONE_3.x, LIGHTPOS_3, LIGHTDIR_3, LIGHTDIFF_3, LIGHTAMB_3, LIGHTSPEC_3);
    }
    if (LIGHTVALID_4)
    {
        lightcolor = getLightColor(input, bumpNormal, lightcolor, LIGHTRADCONE_4.x, LIGHTPOS_4, LIGHTDIR_4, LIGHTDIFF_4, LIGHTAMB_4, LIGHTSPEC_4);
    }
    if (LIGHTVALID_5)
    {
        lightcolor = getLightColor(input, bumpNormal, lightcolor, LIGHTRADCONE_5.x, LIGHTPOS_5, LIGHTDIR_5, LIGHTDIFF_5, LIGHTAMB_5, LIGHTSPEC_5);
    }
    if (LIGHTVALID_6)
    {
        lightcolor = getLightColor(input, bumpNormal, lightcolor, LIGHTRADCONE_6.x, LIGHTPOS_6, LIGHTDIR_6, LIGHTDIFF_6, LIGHTAMB_6, LIGHTSPEC_6);
    }
    if (LIGHTVALID_7)
    {
        lightcolor = getLightColor(input, bumpNormal, lightcolor, LIGHTRADCONE_7.x, LIGHTPOS_7, LIGHTDIR_7, LIGHTDIFF_7, LIGHTAMB_7, LIGHTSPEC_7);

    }
    float4 finalColor = saturate(texColor * lightcolor);
    
    if (HAS_FOG)
    {
        float dist = distance(input.viewPosition, CAMERA_POS);
        if (dist > FOG_MAX)
            return FOG_COLOR;
        if (dist < FOG_MIN)
            return finalColor;
        
        float ratio = (dist - FOG_MIN) / (FOG_MAX - FOG_MIN);
        return lerp(finalColor, FOG_COLOR, ratio);

    }
    return finalColor;
    

}