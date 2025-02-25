add_subdirectory(
    ${GIT_ROOT}/../lvgl
    ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/lvgl
)

target_compile_definitions(lvgl PUBLIC 
    LV_CONF_PATH=${CMAKE_CURRENT_SOURCE_DIR}/Lib/lvgl/lv_conf.h
)

set(LV_BUILD_EXAMPLES OFF CACHE BOOL "Disable building LVGL examples")
set(LV_BUILD_TESTS    OFF CACHE BOOL "Disable building LVGL tests")
set(LV_BUILD_DOCS     OFF CACHE BOOL "Disable building LVGL docs")
set(LV_BUILD_DEMOS    OFF CACHE BOOL "Disable building LVGL demos")