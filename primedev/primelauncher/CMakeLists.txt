# RoguelikeLauncher

add_executable(RoguelikeLauncher "main.cpp" "resources.rc")

target_compile_definitions(RoguelikeLauncher PRIVATE UNICODE _UNICODE)

target_link_libraries(
    RoguelikeLauncher
    PRIVATE shlwapi.lib
            kernel32.lib
            user32.lib
            gdi32.lib
            winspool.lib
            comdlg32.lib
            advapi32.lib
            shell32.lib
            ole32.lib
            oleaut32.lib
            uuid.lib
            odbc32.lib
            odbccp32.lib
            ws2_32.lib
    )

set_target_properties(
    RoguelikeLauncher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${NS_BINARY_DIR} LINK_FLAGS
                                                                           "/MANIFEST:NO /DEBUG /STACK:8000000"
    )
