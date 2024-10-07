#pragma once

#include <inviwo/tnm067lab4/tnm067lab4moduledefine.h>
#include <inviwo/core/ports/layerport.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <modules/basegl/processors/layerprocessing/layerglprocessor.h>

namespace inviwo {

class IVW_MODULE_TNM067LAB4_API LineIntegralConvolution : public LayerGLProcessor {
public:
    LineIntegralConvolution();

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    virtual void preProcess(TextureUnitContainer& cont, const Layer& input, Layer& output) override;
    virtual LayerConfig outputConfig([[maybe_unused]] const Layer& input) const override;

    LayerInport noiseTexture_;

    IntProperty steps_;
    FloatProperty stepSize_;
};

}  // namespace inviwo
