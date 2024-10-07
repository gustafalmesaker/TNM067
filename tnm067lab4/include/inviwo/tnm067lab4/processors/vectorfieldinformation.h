#pragma once

#include <inviwo/tnm067lab4/tnm067lab4moduledefine.h>
#include <modules/basegl/processors/layerprocessing/layerglprocessor.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/stringproperty.h>

namespace inviwo {

class IVW_MODULE_TNM067LAB4_API VectorFieldInformation : public LayerGLProcessor {
public:
    enum class Information { PassThrough, Magnitude, Divergence, Rotation };

    VectorFieldInformation();

    virtual void initializeResources() override;

    static const ProcessorInfo processorInfo_;
    virtual const ProcessorInfo getProcessorInfo() const override;

protected:
    virtual void preProcess(TextureUnitContainer& cont, const Layer& input, Layer& output) override;

private:
    OptionProperty<Information> outputType_;
    StringProperty outputTypeStr_;
    StringProperty minS_;
    StringProperty maxS_;
};

}  // namespace inviwo
