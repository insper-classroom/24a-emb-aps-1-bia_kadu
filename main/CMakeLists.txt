add_executable(main main.c)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_LIST_DIR})
# pull in common dependencies
target_link_libraries(main pico_stdlib hardware_flash hardware_sync)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(main)
