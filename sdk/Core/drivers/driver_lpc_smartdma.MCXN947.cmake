# Add set(CONFIG_USE_driver_lpc_smartdma true) in config.cmake to use this component

include_guard(GLOBAL)
message("${CMAKE_CURRENT_LIST_FILE} component is included.")

if((CONFIG_DEVICE_ID STREQUAL MCXN947) AND CONFIG_USE_driver_common)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
${CMAKE_CURRENT_LIST_DIR}/fsl_smartdma.c
${CMAKE_CURRENT_LIST_DIR}/fsl_smartdma_rt500.c
${CMAKE_CURRENT_LIST_DIR}/fsl_smartdma_mcxn.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
${CMAKE_CURRENT_LIST_DIR}/
)

else()

message(SEND_ERROR "driver_lpc_smartdma.MCXN947 dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()
