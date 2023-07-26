add_executable(FilesProject main.cpp)

un_configure_target(FilesProject)

set_target_properties(FilesProject PROPERTIES FOLDER "Samples")
target_link_libraries(FilesProject UnTL)
