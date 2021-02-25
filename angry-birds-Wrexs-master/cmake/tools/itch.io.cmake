#[[ A specially curated CMAKE file designed to handle
    data creation, compression and itch.io support. It will
    create a game.dat archive of all resources, which can be
    read by ASGE. The itch.io command relies on butler and will
    correctly tag and push builds to the service, assuming the
    project exists.

    1. For data packaging to work the variable GAMEDATA_FOLDER
       must be set

    2. For itch.io support, both GAMEDATA_FOLDER and ITCHIO_USER
       must bet set

    NB: It's probably best not to change these scripts
    unless you have a good reason to do so ]]

if(GAMEDATA_FOLDER)

    ## copies the game data folder to the bin folder, maybe unix should use links??##
    get_target_property(PROJECT_OUTPUT_DIR ${PROJECT_NAME} RUNTIME_OUTPUT_DIRECTORY)

    if(WIN32)
        add_custom_command(
                TARGET ${PROJECT_NAME}
                POST_BUILD
                COMMAND "${CMAKE_SOURCE_DIR}/tools/7zip/${PLATFORM}/7za" -tzip a
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/game.dat" "${CMAKE_SOURCE_DIR}/${GAMEDATA_FOLDER}/*"
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/${GAMEDATA_FOLDER}"
                BYPRODUCTS "${RUNTIME_OUTPUT_DIRECTORY}/game.dat"
                COMMENT "creating data archive")
    else()
        add_custom_command(
                TARGET ${PROJECT_NAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E tar cfv "$<TARGET_FILE_DIR:${PROJECT_NAME}>/game.dat" --format=zip -- *
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/${GAMEDATA_FOLDER}"
                BYPRODUCTS "${RUNTIME_OUTPUT_DIRECTORY}/game.dat"
                COMMENT "creating data archive")       
    endif()
    
    if(ITCHIO_USER)
        ## uses butler to push the built game to itch.io ##
        add_custom_target(Publish${PROJECT_NAME}
                EXCLUDE_FROM_ALL
                COMMAND "${CMAKE_SOURCE_DIR}/tools/itch.io/${PLATFORM}/butler" update &&
                "${CMAKE_SOURCE_DIR}/tools/itch.io/${PLATFORM}/butler" --ignore=data push ..
                ${ITCHIO_USER}/${PROJECT_NAME}:${PLATFORM}-${CMAKE_BUILD_TYPE}
                WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                COMMENT "uploading game to itch.io. be patient")

        add_dependencies(Publish${PROJECT_NAME} ${PROJECT_NAME}+GameData)
    else()
        message("No itch.io user has been specified: disabling itch build targets")
    endif()

else()
    message("No game data folder has been specified: disabling data and itch.io targets")
endif()

