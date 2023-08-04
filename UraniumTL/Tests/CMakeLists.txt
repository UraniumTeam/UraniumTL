set(SRC
    Common/Common.h
    main.cpp

    Buffers/ArrayPool.cpp
    Utils/UUID.cpp
    Containers/List.cpp
    RTTI/RTTI.cpp
    Strings/Format.cpp
    Strings/String.cpp
)

add_executable(UnTLTests ${SRC})

set_target_properties(UnTLTests PROPERTIES FOLDER "UraniumTL")
target_link_libraries(UnTLTests gtest gmock UnTL)

get_property("TARGET_SOURCE_FILES" TARGET UnTLTests PROPERTY SOURCES)
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${TARGET_SOURCE_FILES})

include(GoogleTest)
gtest_discover_tests(UnTLTests)
