#include <inviwo/tnm067lab2/processors/hydrogengenerator.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/util/volumeramutils.h>
#include <modules/base/algorithm/dataminmax.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <modules/base/algorithm/dataminmax.h>

#include <numbers>


namespace inviwo {

const ProcessorInfo HydrogenGenerator::processorInfo_{
    "org.inviwo.HydrogenGenerator",  // Class identifier
    "Hydrogen Generator",            // Display name
    "TNM067",                        // Category
    CodeState::Stable,               // Code state
    Tags::CPU,                       // Tags
};

const ProcessorInfo HydrogenGenerator::getProcessorInfo() const { return processorInfo_; }

HydrogenGenerator::HydrogenGenerator()
    : Processor(), volume_("volume"), size_("size_", "Volume Size", 16, 4, 256) {
    addPort(volume_);
    addProperty(size_);
}

void HydrogenGenerator::process() {
    auto ram = std::make_shared<VolumeRAMPrecision<float>>(size3_t(size_));
    auto vol = std::make_shared<Volume>(ram);

    auto data = ram->getDataTyped();
    util::IndexMapper3D index(ram->getDimensions());

    util::forEachVoxel(*ram, [&](const size3_t& pos) {
        vec3 cartesian = idTOCartesian(pos);
        data[index(pos)] = static_cast<float>(eval(cartesian));
    });

    auto minMax = util::volumeMinMax(ram.get());
    vol->dataMap.dataRange = vol->dataMap.valueRange = dvec2(minMax.first.x, minMax.second.x);

    volume_.setData(vol);
}

vec3 HydrogenGenerator::cartesianToSpherical(vec3 cartesian) {
    vec3 sph{cartesian};

    const float x = cartesian.x;
    const float y = cartesian.y;
    const float z = cartesian.z;
    
    // TASK 1: implement conversion using the equations in the lab script
    sph.x = sqrt(x * x + y * y + z * z);

    if (sph.x < 0.01) { sph.y = 0; } 
        else { sph.y = acos(z / sph.x);}

    sph.z = atan2(y, x);

    return sph;
}

double HydrogenGenerator::eval(vec3 cartesian) {
    // Get spherical coordinates
    vec3 spherical = cartesianToSpherical(cartesian);

    // TASK 2: Evaluate wave function
    float yellow, red, blue, green, purple; //colors according to lab instruction

    yellow = 1.0 / (81 * sqrt(6 * M_PI));
    red = 1;
    blue = spherical.x * spherical.x;
    green = exp(-spherical.x/3.0);
    purple = 3*pow(cos(spherical.y),2)-1;

    float wave = yellow* red* blue* green* purple;

    return abs(pow(wave,2));
}

vec3 HydrogenGenerator::idTOCartesian(size3_t pos) {
    vec3 p(pos);
    p /= size_ - 1;
    return p * (36.0f) - 18.0f;
}

}  // namespace inviwo
