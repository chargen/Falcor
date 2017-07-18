/***************************************************************************
# Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
__import Effects.LeanMapping;
#import "ShaderCommon.slang"
#import "Shading.slang"
__import DefaultVS;

layout(set = 0, binding = 0) uniform PerFrameCB
{
    vec3 gAmbient;
};

#ifdef _MS_USER_NORMAL_MAPPING
layout(set = 1, location = 0) uniform texture2D gLeanMaps[_LEAN_MAP_COUNT];
layout(set = 1, location = 1) uniform sampler gSampler;

void perturbNormal(in const MaterialData mat, inout ShadingAttribs shAttr, bool forceSample)
{
    if (mat.desc.hasNormalMap != 0)
    {
        applyLeanMap(gLeanMaps[mat.values.id], gSampler, shAttr);
    }
}
#endif

layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 bitangent;
layout(location = 3) in vec2 texC;

layout(location = 2) out vec4 finalColor;

float4 main(VS_OUT vOut) : SV_TARGET
{
    ShadingAttribs shAttr;
    prepareShadingAttribs(gMaterial, posW, gCam.position, normalW, bitangentW, texC, shAttr);

    ShadingOutput result;

    [unroll]
    for (uint l = 0; l < _LIGHT_COUNT; l++)
    {
        evalMaterial(shAttr, gLights[l], result, l == 0);
    }

    finalColor = float4(result.finalValue + gAmbient * result.diffuseAlbedo, 1.f);
}