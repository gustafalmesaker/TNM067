ivw_module(TNM067Lab1)

set(HEADER_FILES
    include/inviwo/tnm067lab1/tnm067lab1module.h
    include/inviwo/tnm067lab1/tnm067lab1moduledefine.h
    include/inviwo/tnm067lab1/processors/imagemappingcpu.h
    include/inviwo/tnm067lab1/processors/imagetoheightfield.h
    include/inviwo/tnm067lab1/processors/imageupsampler.h
    include/inviwo/tnm067lab1/util/interpolationmethods.h
    include/inviwo/tnm067lab1/util/scalartocolormapping.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/tnm067lab1module.cpp
    src/processors/imagemappingcpu.cpp
    src/processors/imagetoheightfield.cpp
    src/processors/imageupsampler.cpp
    src/util/interpolationmethods.cpp
    src/util/scalartocolormapping.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(SHADER_FILES
	# Add shaders
)
ivw_group("Shader Files" ${SHADER_FILES})

set(TEST_FILES
    tests/unittests/imageupsampler-test.cpp
    tests/unittests/interploation-test.cpp
    tests/unittests/tnm067lab1-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

# Add shader directory to install package
#ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
ivw_folder(inviwo-module-tnm067lab1 TNM067)

add_custom_command( TARGET inviwo-unittests-tnm067lab1 POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                        $<TARGET_RUNTIME_DLLS:inviwo-unittests-tnm067lab1>
                        $<TARGET_FILE_DIR:inviwo-unittests-tnm067lab1>
                    COMMAND_EXPAND_LISTS)

