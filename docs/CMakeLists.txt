find_package(Sphinx)

if(Sphinx-NOTFOUND)
        message(WARNING "Skipping docs: Sphinx not found.")
else()
        set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
        set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx/)
        
        function(add_oberon_module source dest)
        add_custom_command(
        OUTPUT ${dest}
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/../tools/docgen.py
                ${source}
                > ${dest}
        DEPENDS ${source}
        )
        endfunction()
        
        add_oberon_module("${CMAKE_CURRENT_SOURCE_DIR}/../liboberon/Math.Mod" "${SPHINX_SOURCE}/src/API_Math.rst")
        add_oberon_module("${CMAKE_CURRENT_SOURCE_DIR}/../liboberon/Oberon.Mod" "${SPHINX_SOURCE}/src/API_Oberon.rst")
        add_oberon_module("${CMAKE_CURRENT_SOURCE_DIR}/../liboberon/Out.Mod" "${SPHINX_SOURCE}/src/API_Out.rst")
        add_oberon_module("${CMAKE_CURRENT_SOURCE_DIR}/../liboberon/Random.Mod" "${SPHINX_SOURCE}/src/API_Random.rst")

        add_custom_target(docs
                COMMAND ${SPHINX_EXECUTABLE} -b html
                ${SPHINX_SOURCE} ${SPHINX_BUILD}
                DEPENDS
                        ${SPHINX_SOURCE}/conf.py
                        ${SPHINX_SOURCE}/index.rst
                        ${SPHINX_SOURCE}/src/CommandLineInterface.rst
                        ${SPHINX_SOURCE}/src/FAQ.rst
                        ${SPHINX_SOURCE}/src/GettingStarted.rst
                        ${SPHINX_SOURCE}/src/Installation.rst
                        ${SPHINX_SOURCE}/src/API_Math.rst
                        ${SPHINX_SOURCE}/src/API_Oberon.rst
                        ${SPHINX_SOURCE}/src/API_Out.rst
                        ${SPHINX_SOURCE}/src/API_Random.rst
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating documentation with Sphinx")
        
        include(GNUInstallDirs)
        install(
                DIRECTORY ${SPHINX_BUILD}/
                DESTINATION ${CMAKE_INSTALL_DOCDIR}
        )
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/oberon-lang.1 DESTINATION "${CMAKE_INSTALL_MANDIR}")
