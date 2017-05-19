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
#include "Framework.h"
#include "API/LowLevel/DescriptorPool.h"
#include "D3D12DescriptorHeap.h"

namespace Falcor
{
    struct DescriptorPoolApiData
    {
        D3D12DescriptorHeap::SharedPtr pHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    };

    bool DescriptorPool::apiInit()
    {
        // Find out how many heaps we need
        static_assert(DescriptorPool::kTypeCount == 6, "Unexpected desc count, make sure all desc types are supported");
        uint32_t descCount[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = { 0 };

        descCount[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = mDesc.mDescCount[(uint32_t)Type::Rtv];
        descCount[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = mDesc.mDescCount[(uint32_t)Type::Dsv];
        descCount[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = mDesc.mDescCount[(uint32_t)Type::Sampler];
        descCount[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = mDesc.mDescCount[(uint32_t)Type::Cbv];
        descCount[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] += mDesc.mDescCount[(uint32_t)Type::Uav];
        descCount[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] += mDesc.mDescCount[(uint32_t)Type::Srv];

        mpApiData = new DescriptorPoolApiData;
        for (uint32_t i = 0; i < arraysize(mpApiData->pHeaps); i++)
        {
            if (descCount[i])
            {
                mpApiData->pHeaps[i] = D3D12DescriptorHeap::create(D3D12_DESCRIPTOR_HEAP_TYPE(i), descCount[i], mDesc.mShaderVisible);
                if (!mpApiData->pHeaps[i]) return false;
            }
        }
        return true;
    }

    DescriptorPool::ApiHandle DescriptorPool::getApiHandle(uint32_t heapIndex) const
    {
        assert(index < arraysize(mpApiData->pHeaps));
        return mpApiData->pHeaps[heapIndex]->getApiHandle();
    }
}