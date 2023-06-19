# ShaderRenderer
Renderer with Irrlicht 1.9 to view shaders

## OVERVIEW
This is just a brief thing to assist with the creation of shaders for DirectX 9 and HLSL (uses vertex shader 2.0 and pixel shader 2.0).

Included is a default model with normal maps and textures to muck around with.

Several default parameters are included:
```
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
```
If your shader doesn't use these, who cares, these are what the shader callback will attempt to bind. If they're not there, errors might be thrown.

The default shader included is a diffuse lighting shader with specular highlights and normal mapping.

## WEIRD IRRLICHT-SPECIFIC CRAP
Irrlicht passes in tangent and binormal information as TEXCOORD1 and TEXCOORD2, respectively. If your shader uses TANGENT0 and BINORMAL0 instead you'll need to adjust it.

As previously stated, your debug console may be bloated if variables are unused. If this *really* bothers you, you can just edit the code in `shaders.cpp` to not pass those in and recompile it.

This was compiled on Visual Studio 2022 with C++ 20 and Irrlicht 1.9 (trunk version circa early 2023).
