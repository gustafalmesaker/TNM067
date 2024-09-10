#include <inviwo/tnm067lab1/util/scalartocolormapping.h>

namespace inviwo {

void ScalarToColorMapping::clearColors() { baseColors_.clear(); }
void ScalarToColorMapping::addBaseColors(vec4 color) { baseColors_.push_back(color); }

vec4 ScalarToColorMapping::sample(float t) const {
    if (baseColors_.size() == 0) return vec4(t);
    if (baseColors_.size() == 1) return vec4(baseColors_[0]);
    if (t <= 0) return vec4(baseColors_.front());
    if (t >= 1) return vec4(baseColors_.back());

    // TODO: use t to select which two base colors to interpolate in-between
    t = t * (baseColors_.size() - 1);
    int colorIndex = floor(t);

    t = t - colorIndex;

    // TODO: Interpolate colors in baseColors_ and set dummy color to result
    vec4 finalColor = (1 - t) * baseColors_[colorIndex] + t * baseColors_[colorIndex + 1];


    return finalColor;
}

}  // namespace inviwo
