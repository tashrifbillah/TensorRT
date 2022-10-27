/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TRT_BATCHED_NMS_PLUGIN_H
#define TRT_BATCHED_NMS_PLUGIN_H
#include "batchedNMSPlugin/gatherNMSOutputs.h"
#include "common/kernels/kernel.h"
#include "common/nmsUtils.h"
#include "common/plugin.h"
#include <string>
#include <vector>

namespace nvinfer1
{
namespace plugin
{

class BatchedNMSPlugin : public IPluginV2Ext
{
public:
    BatchedNMSPlugin(NMSParameters param);
    BatchedNMSPlugin(const void* data, size_t length);
    ~BatchedNMSPlugin() override = default;

    // IPluginV2 methods
    const char* getPluginType() const noexcept override;
    const char* getPluginVersion() const noexcept override;
    int32_t getNbOutputs() const noexcept override;
    Dims getOutputDimensions(int32_t index, const Dims* inputs, int32_t nbInputDims) noexcept override;
    bool supportsFormat(DataType type, PluginFormat format) const noexcept override;
    size_t getWorkspaceSize(int32_t maxBatchSize) const noexcept override;
    int32_t enqueue(int32_t batchSize, void const* const* inputs, void* const* outputs, void* workspace,
        cudaStream_t stream) noexcept override;
    int32_t initialize() noexcept override;
    void terminate() noexcept override;
    size_t getSerializationSize() const noexcept override;
    void serialize(void* buffer) const noexcept override;
    void destroy() noexcept override;
    void setPluginNamespace(const char* libNamespace) noexcept override;
    const char* getPluginNamespace() const noexcept override;
    void setClipParam(bool clip) noexcept;
    void setScoreBits(int32_t scoreBits) noexcept;
    void setCaffeSemantics(bool caffeSemantics) noexcept;

    // IPluginV2Ext methods
    nvinfer1::DataType getOutputDataType(
        int32_t index, const nvinfer1::DataType* inputTypes, int32_t nbInputs) const noexcept override;
    bool isOutputBroadcastAcrossBatch(
        int32_t outputIndex, const bool* inputIsBroadcasted, int32_t nbInputs) const noexcept override;
    bool canBroadcastInputAcrossBatch(int32_t inputIndex) const noexcept override;
    void configurePlugin(const Dims* inputDims, int32_t nbInputs, const Dims* outputDims, int32_t nbOutputs,
        const DataType* inputTypes, const DataType* outputTypes, const bool* inputIsBroadcast,
        const bool* outputIsBroadcast, PluginFormat floatFormat, int32_t maxBatchSize) noexcept override;
    IPluginV2Ext* clone() const noexcept override;

private:
    NMSParameters param{};
    int32_t mBoxesSize{};
    int32_t mScoresSize{};
    int32_t mNumPriors{};
    std::string mNamespace;
    bool mClipBoxes{};
    DataType mPrecision;
    int32_t mScoreBits;
    bool mCaffeSemantics{true};
    pluginStatus_t mPluginStatus{};
};

class BatchedNMSDynamicPlugin : public IPluginV2DynamicExt
{
public:
    BatchedNMSDynamicPlugin(NMSParameters param);
    BatchedNMSDynamicPlugin(const void* data, size_t length);
    ~BatchedNMSDynamicPlugin() override = default;

    // IPluginV2 methods
    const char* getPluginType() const noexcept override;
    const char* getPluginVersion() const noexcept override;
    int32_t getNbOutputs() const noexcept override;
    int32_t initialize() noexcept override;
    void terminate() noexcept override;
    size_t getSerializationSize() const noexcept override;
    void serialize(void* buffer) const noexcept override;
    void destroy() noexcept override;
    void setPluginNamespace(const char* libNamespace) noexcept override;
    const char* getPluginNamespace() const noexcept override;
    void setClipParam(bool clip) noexcept;
    void setScoreBits(int32_t scoreBits) noexcept;
    void setCaffeSemantics(bool caffeSemantics) noexcept;

    // IPluginV2Ext methods
    nvinfer1::DataType getOutputDataType(
        int32_t index, const nvinfer1::DataType* inputType, int32_t nbInputs) const noexcept override;

    // IPluginV2DynamicExt methods
    IPluginV2DynamicExt* clone() const noexcept override;
    DimsExprs getOutputDimensions(
        int32_t outputIndex, const DimsExprs* inputs, int32_t nbInputs, IExprBuilder& exprBuilder) noexcept override;
    bool supportsFormatCombination(
        int32_t pos, const PluginTensorDesc* inOut, int32_t nbInputs, int32_t nbOutputs) noexcept override;
    void configurePlugin(const DynamicPluginTensorDesc* in, int32_t nbInputs, const DynamicPluginTensorDesc* out,
        int32_t nbOutputs) noexcept override;
    size_t getWorkspaceSize(const PluginTensorDesc* inputs, int32_t nbInputs, const PluginTensorDesc* outputs,
        int32_t nbOutputs) const noexcept override;
    int32_t enqueue(const PluginTensorDesc* inputDesc, const PluginTensorDesc* outputDesc, const void* const* inputs,
        void* const* outputs, void* workspace, cudaStream_t stream) noexcept override;

private:
    NMSParameters param{};
    int32_t mBoxesSize{};
    int32_t mScoresSize{};
    int32_t mNumPriors{};
    std::string mNamespace;
    bool mClipBoxes{};
    DataType mPrecision;
    int32_t mScoreBits;
    bool mCaffeSemantics{true};
    pluginStatus_t mPluginStatus{};
};

class BatchedNMSBasePluginCreator : public nvinfer1::pluginInternal::BaseCreator
{
public:
    BatchedNMSBasePluginCreator();
    ~BatchedNMSBasePluginCreator() override = default;

    const char* getPluginVersion() const noexcept override;
    const PluginFieldCollection* getFieldNames() noexcept override;

protected:
    static PluginFieldCollection mFC;
    static std::vector<PluginField> mPluginAttributes;
};

class BatchedNMSPluginCreator : public BatchedNMSBasePluginCreator
{
public:
    const char* getPluginName() const noexcept override;
    IPluginV2Ext* createPlugin(const char* name, const PluginFieldCollection* fc) noexcept override;
    IPluginV2Ext* deserializePlugin(const char* name, const void* serialData, size_t serialLength) noexcept override;
};

class BatchedNMSDynamicPluginCreator : public BatchedNMSBasePluginCreator
{
public:
    const char* getPluginName() const noexcept override;
    IPluginV2DynamicExt* createPlugin(const char* name, const PluginFieldCollection* fc) noexcept override;
    IPluginV2DynamicExt* deserializePlugin(
        const char* name, const void* serialData, size_t serialLength) noexcept override;
};

} // namespace plugin
} // namespace nvinfer1

#endif // TRT_BATCHED_NMS_PLUGIN_H
