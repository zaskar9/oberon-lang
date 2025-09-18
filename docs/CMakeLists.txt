find_package(Sphinx)

if(Sphinx-NOTFOUND)
        message(WARNING "Skipping docs: Sphinx not found.")
else()
        set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
        set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx/)

        find_package(Python COMPONENTS Interpreter)
        
        function(add_oberon_module source dest)
        add_custom_command(
        OUTPUT ${dest}
        COMMAND ${Python_EXECUTABLE} ../../utils/docgen/docgen.py
                ${source}
                > ${dest}
        DEPENDS ${source}
        )
        endfunction()

        add_oberon_module("${CMAKE_CURRENT_SOURCE_DIR}/../stdlib/Files.Mod" "${SPHINX_SOURCE}/src/Files.rst")
        add_oberon_module("${CMAKE_CURRENT_SOURCE_DIR}/../stdlib/In.Mod" "${SPHINX_SOURCE}/src/In.rst")
        add_oberon_module("${CMAKE_CURRENT_SOURCE_DIR}/../stdlib/Math.Mod" "${SPHINX_SOURCE}/src/Math.rst")
        add_oberon_module("${CMAKE_CURRENT_SOURCE_DIR}/../stdlib/MathL.Mod" "${SPHINX_SOURCE}/src/MathL.rst")
        add_oberon_module("${CMAKE_CURRENT_SOURCE_DIR}/../stdlib/Oberon.Mod" "${SPHINX_SOURCE}/src/Oberon.rst")
        add_oberon_module("${CMAKE_CURRENT_SOURCE_DIR}/../stdlib/Out.Mod" "${SPHINX_SOURCE}/src/Out.rst")
        add_oberon_module("${CMAKE_CURRENT_SOURCE_DIR}/../stdlib/Random.Mod" "${SPHINX_SOURCE}/src/Random.rst")
        add_oberon_module("${CMAKE_CURRENT_SOURCE_DIR}/../stdlib/Strings.Mod" "${SPHINX_SOURCE}/src/Strings.rst")

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
                        ${SPHINX_SOURCE}/src/Files.rst
                        ${SPHINX_SOURCE}/src/In.rst
                        ${SPHINX_SOURCE}/src/Math.rst
                        ${SPHINX_SOURCE}/src/MathL.rst
                        ${SPHINX_SOURCE}/src/Oberon.rst
                        ${SPHINX_SOURCE}/src/Out.rst
                        ${SPHINX_SOURCE}/src/Random.rst
                        ${SPHINX_SOURCE}/src/Strings.rst
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating documentation with Sphinx")
        
        include(GNUInstallDirs)
        install(
                DIRECTORY ${SPHINX_BUILD}/
                DESTINATION ${CMAKE_INSTALL_DOCDIR}
        )
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/oberon-lang.1 DESTINATION "${CMAKE_INSTALL_MANDIR}")
