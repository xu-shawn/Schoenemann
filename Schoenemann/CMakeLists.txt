add_executable (Schoenemann "Schoenemann.cpp" "Schoenemann.h" "evaluate.h" "search.cpp" "search.h" "timeman.cpp" "timeman.h" "tt.cpp" "tt.h" "moveorder.cpp" "moveorder.h" "helper.h" "helper.cpp"  "consts.h" "datagen/gen.h" "datagen/gen.cpp" "MantaRay/AccumulatorOperation.h" "MantaRay/BenchmarkRunner.cpp" "MantaRay/SIMD.h" "MantaRay/Activation/ClippedReLU.h" "MantaRay/Backend/Avx.h" "MantaRay/Backend/Avx2.h" "MantaRay/Backend/Avx512.h" "MantaRay/Backend/RegisterDefinition.h" "MantaRay/IO/BinaryFileStream.h" "MantaRay/IO/BinaryMemoryStream.h" "MantaRay/IO/DataStream.h" "MantaRay/IO/MarlinflowStream.h" "MantaRay/Perspective/PerspectiveAccumulator.h" "MantaRay/Perspective/PerspectiveNNUE.h")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Schoenemann PROPERTY CXX_STANDARD 20)
endif()