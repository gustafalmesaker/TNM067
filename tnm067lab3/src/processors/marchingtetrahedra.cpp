#include <inviwo/tnm067lab3/processors/marchingtetrahedra.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/util/assertion.h>
#include <inviwo/core/network/networklock.h>
#include <inviwo/tnm067lab1/util/interpolationmethods.h>
#include <iostream>
#include <fstream>


namespace inviwo {

const ProcessorInfo MarchingTetrahedra::processorInfo_{
    "org.inviwo.MarchingTetrahedra",  // Class identifier
    "Marching Tetrahedra",            // Display name
    "TNM067",                         // Category
    CodeState::Stable,                // Code state
    Tags::CPU,                        // Tags
};
const ProcessorInfo MarchingTetrahedra::getProcessorInfo() const { return processorInfo_; }

MarchingTetrahedra::MarchingTetrahedra()
    : Processor()
    , volume_("volume")
    , mesh_("mesh")
    , isoValue_("isoValue", "ISO value", 0.5f, 0.0f, 1.0f) {

    addPort(volume_);
    addPort(mesh_);

    addProperty(isoValue_);

    isoValue_.setSerializationMode(PropertySerializationMode::All);

    volume_.onChange([&]() {
        if (!volume_.hasData()) {
            return;
        }
        NetworkLock lock(getNetwork());
        float iso = (isoValue_.get() - isoValue_.getMinValue()) /
                    (isoValue_.getMaxValue() - isoValue_.getMinValue());
        const auto vr = volume_.getData()->dataMap.valueRange;
        isoValue_.setMinValue(static_cast<float>(vr.x));
        isoValue_.setMaxValue(static_cast<float>(vr.y));
        isoValue_.setIncrement(static_cast<float>(glm::abs(vr.y - vr.x) / 50.0));
        isoValue_.set(static_cast<float>(iso * (vr.y - vr.x) + vr.x));
        isoValue_.setCurrentStateAsDefault();
    });
}

void MarchingTetrahedra::process() {
    auto volume = volume_.getData()->getRepresentation<VolumeRAM>();
    MeshHelper mesh(volume_.getData());

    const auto& dims = volume->getDimensions();

    const float iso = isoValue_.get();

    util::IndexMapper3D mapVolPosToIndex(dims);

    static constexpr std::array<std::array<size_t, 4>, 6> tetrahedraIds = {
        {{0, 1, 2, 5}, {1, 3, 2, 5}, {3, 2, 5, 7}, {0, 2, 4, 5}, {6, 4, 2, 5}, {6, 7, 5, 2}}};

    size3_t pos{};
    for (pos.z = 0; pos.z < dims.z - 1; ++pos.z) {
        for (pos.y = 0; pos.y < dims.y - 1; ++pos.y) {
            for (pos.x = 0; pos.x < dims.x - 1; ++pos.x) {
                // The DataPoint index should be the 1D-index for the DataPoint in the cell
                // Use volume->getAsDouble to query values from the volume
                // Spatial position should be between 0 and 1

                // TODO: TASK 2: create a nested for loop to construct the cell
                Cell c{};

                size_t cellVertex = 0;

                for (int z = 0; z < 2; z++){ //3 loops, 8 iterations
                    for (int y = 0; y < 2; y++) {
                        for (int x = 0; x < 2; x++) {
                            vec3 localPos(x,y,z);
                            vec3 globalPos(x + pos.x, y + pos.y, z + pos.z);
                            
                            vec3 scaledCellPos =
                                calculateDataPointPos(pos, localPos, dims);

                            size_t indexInVolume = mapVolPosToIndex(globalPos);

                            double value = volume->getAsDouble(globalPos);

                            cellVertex = calculateDataPointIndexInCell(localPos);

                            c.dataPoints[cellVertex].pos = scaledCellPos;
                            c.dataPoints[cellVertex].value = value;
                            c.dataPoints[cellVertex].indexInVolume = indexInVolume; 
                        }
                    }
                }


                // TODO: TASK 3: Subdivide cell into 6 tetrahedra (hint: use tetrahedraIds)
                std::vector<Tetrahedra> tetrahedras;
                for (size_t i = 0; i < 6; i++){
                    Tetrahedra newTetra{};

                    for (size_t j = 0; j < 4; j++){ 
                        newTetra.dataPoints[j] = c.dataPoints[tetrahedraIds[i][j]];
                    }
                    tetrahedras.push_back(newTetra);
                }


                for (const Tetrahedra& tetrahedra : tetrahedras) {
                    // TODO: TASK 4: Calculate case id for each tetrahedra, and add triangles for
                    // each case (use MeshHelper)

                    // Calculate for tetra case index
                    int caseId = 0;

                    for (size_t index = 0; index < 4; index++)
                    {
                        if (tetrahedra.dataPoints[index].value < iso)
                        {
                            caseId += (int)glm::pow(2.0, index); //calculating caseID based on binaries
                            //example: 1010 -> 10 & 0010 -> 2
                        }
                    }

                    DataPoint v0 = tetrahedra.dataPoints[0];
                    DataPoint v1 = tetrahedra.dataPoints[1];
                    DataPoint v2 = tetrahedra.dataPoints[2];
                    DataPoint v3 = tetrahedra.dataPoints[3];


                    // Extract triangles
                    switch (caseId) {
                        case 0:
                        case 15:
                            break;
                        case 1:
                        case 14: {

                            if (caseId == 1) {
                                createTriangle(v0, v1, v0, v2, v0, v3, iso, mesh);
                            } else {
                                createTriangle(v0, v1, v0, v3, v0, v2, iso, mesh);
                            }
                            break;
                        }
                        case 2:
                        case 13: {
                            if (caseId == 2) {
                                createTriangle(v1, v0, v1, v3, v1, v2, iso, mesh);
                            } else {
                                createTriangle(v1, v0, v1, v2, v1, v3, iso, mesh);
                            }
                            break;
                        }
                        case 3:
                        case 12: {
                            if (caseId == 3) {
                                createTriangle(v0, v3, v1, v3, v1, v2, iso, mesh);
                                createTriangle(v0, v3, v1, v2, v0, v2, iso, mesh);
                            } else {
                                createTriangle(v0, v3, v1, v2, v1, v3, iso, mesh);
                                createTriangle(v0, v3, v0, v2, v1, v2, iso, mesh);
                            }
                            break;
                        }
                        case 4:
                        case 11: {
                            if (caseId == 4) {
                                createTriangle(v2, v0, v2, v1, v2, v3, iso, mesh);
                            } else {
                                createTriangle(v2, v0, v2, v3, v2, v1, iso, mesh);
                            }
                            break;
                        }
                        case 5:
                        case 10: {
                            if (caseId == 5) {
                                createTriangle(v0, v3, v0, v1, v2, v1, iso, mesh);
                                createTriangle(v0, v3, v2, v1, v2, v3, iso, mesh);
                            } else {
                                createTriangle(v0, v3, v2, v1, v0, v1, iso, mesh);
                                createTriangle(v0, v3, v2, v3, v2, v1, iso, mesh);
                            }
                            break;
                        }
                        case 6:
                        case 9: {
                            if (caseId == 6) {
                                createTriangle(v2, v0, v1, v0, v1, v3, iso, mesh);
                                createTriangle(v2, v0, v1, v3, v2, v3, iso, mesh);
                            } else {
                                createTriangle(v2, v0, v1, v3, v1, v0, iso, mesh);
                                createTriangle(v2, v0, v2, v3, v1, v3, iso, mesh);
                            }
                            break;
                        }
                        case 7:
                        case 8: {
                            if (caseId == 7) {
                                createTriangle(v3, v1, v3, v2, v3, v0, iso, mesh);
                            } else {
                                createTriangle(v3, v1, v3, v0, v3, v2, iso, mesh);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    mesh_.setData(mesh.toBasicMesh());
}

int MarchingTetrahedra::calculateDataPointIndexInCell(ivec3 index3D) {
    // TODO: TASK 1: map 3D index to 1D
     return (index3D.x * 1) + (index3D.y * 2) + (index3D.z * 4); //2^0 * x + 2^1 * y + 2^2 * z
}

vec3 MarchingTetrahedra::calculateDataPointPos(size3_t posVolume, ivec3 posCell, ivec3 dims) {
    // TODO: TASK 1: scale DataPoint position with dimensions to be between 0 and 1
     float x = (float(posVolume[0] + posCell[0])) / float(dims[0] - 1);
     float y = (float(posVolume[1] + posCell[1])) / float(dims[1] - 1);
     float z = (float(posVolume[2] + posCell[2])) / float(dims[2] - 1);

    return vec3(x, y, z);
}

MarchingTetrahedra::MeshHelper::MeshHelper(std::shared_ptr<const Volume> vol)
    : edgeToVertex_()
    , vertices_()
    , mesh_(std::make_shared<BasicMesh>())
    , indexBuffer_(mesh_->addIndexBuffer(DrawType::Triangles, ConnectivityType::None)) {
    mesh_->setModelMatrix(vol->getModelMatrix());
    mesh_->setWorldMatrix(vol->getWorldMatrix());
}

void MarchingTetrahedra::MeshHelper::addTriangle(size_t i0, size_t i1, size_t i2) {
    IVW_ASSERT(i0 != i1, "i0 and i1 should not be the same value");
    IVW_ASSERT(i0 != i2, "i0 and i2 should not be the same value");
    IVW_ASSERT(i1 != i2, "i1 and i2 should not be the same value");

    indexBuffer_->add(static_cast<glm::uint32_t>(i0));
    indexBuffer_->add(static_cast<glm::uint32_t>(i1));
    indexBuffer_->add(static_cast<glm::uint32_t>(i2));

    const auto a = std::get<0>(vertices_[i0]);
    const auto b = std::get<0>(vertices_[i1]);
    const auto c = std::get<0>(vertices_[i2]);

    const vec3 n = glm::normalize(glm::cross(b - a, c - a));
    std::get<1>(vertices_[i0]) += n;
    std::get<1>(vertices_[i1]) += n;
    std::get<1>(vertices_[i2]) += n;
}

std::shared_ptr<BasicMesh> MarchingTetrahedra::MeshHelper::toBasicMesh() {
    for (auto& vertex : vertices_) {
        // Normalize the normal of the vertex
        std::get<1>(vertex) = glm::normalize(std::get<1>(vertex));
    }
    mesh_->addVertices(vertices_);
    return mesh_;
}

std::uint32_t MarchingTetrahedra::MeshHelper::addVertex(vec3 pos, size_t i, size_t j) {
    IVW_ASSERT(i != j, "i and j should not be the same value");
    if (j < i) std::swap(i, j);

    auto [edgeIt, inserted] = edgeToVertex_.try_emplace(std::make_pair(i, j), vertices_.size());
    if (inserted) {
        vertices_.push_back({pos, vec3(0, 0, 0), pos, vec4(0.7f, 0.7f, 0.7f, 1.0f)});
    }
    return static_cast<std::uint32_t>(edgeIt->second);
}

vec3 MarchingTetrahedra::Interpolation(const DataPoint& p1, const DataPoint& p2, const float iso) {
    return p1.pos + (iso - p1.value) * ((p2.pos - p1.pos) / (p2.value - p1.value));
}

void MarchingTetrahedra::createTriangle(const MarchingTetrahedra::DataPoint& v0,
                                        const MarchingTetrahedra::DataPoint& v1,
                                        const MarchingTetrahedra::DataPoint& v2,
                                        const MarchingTetrahedra::DataPoint& v3,
                                        const MarchingTetrahedra::DataPoint& v4,
                                        const MarchingTetrahedra::DataPoint& v5,
                                        float iso,
                                        MarchingTetrahedra::MeshHelper& mesh) {

    vec3 interpolation1 = Interpolation(v0, v1, iso);
    vec3 interpolation2 = Interpolation(v2, v3, iso);
    vec3 interpolation3 = Interpolation(v4, v5, iso);

    auto updatedV0 = mesh.addVertex(interpolation1, v0.indexInVolume, v1.indexInVolume);
    auto updatedV1 = mesh.addVertex(interpolation2, v2.indexInVolume, v3.indexInVolume);
    auto updatedV2 = mesh.addVertex(interpolation3, v4.indexInVolume, v5.indexInVolume);

    mesh.addTriangle(updatedV0, updatedV1, updatedV2);
}

}  // namespace inviwo
