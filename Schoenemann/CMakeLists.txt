add_executable (Schoenemann "Schoenemann.cpp" "Schoenemann.h" "evaluate.cpp" "evaluate.h" "search.cpp" "search.h" "timeman.cpp" "timeman.h" "psqt.cpp" "psqt.h" "tt.cpp" "tt.h" "moveorder.cpp" "moveorder.h" "helper.h" "helper.cpp"  "consts.h" "datagen/gen.h" "datagen/gen.cpp")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Schoenemann PROPERTY CXX_STANDARD 20)
endif()