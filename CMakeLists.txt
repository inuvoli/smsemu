# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.31)

# Set the project name
project (smsemu VERSION 0.2)

# Set variables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Search needed libraries
find_package(SDL3 CONFIG REQUIRED)
find_package(loguru CONFIG REQUIRED)

# Create a sources variable with a link to all cpp files to compile
set(SMSEMU_SOURCES
		src/cpu/z80a.cpp
		src/memory/bios.cpp                                                             
		src/memory/cartridge.cpp                                                        
		src/memory/mapper.cpp                                                           
		src/memory/mappercodemaster.cpp                                                 
		src/memory/mappersega.cpp                                                       
		src/memory/memorymanager.cpp                                                    
		src/video/vdp.cpp
		src/video/framebuffer.cpp
		src/audio/tonegen.cpp                                                          
		src/audio/noisegen.cpp                                                         
		src/audio/psg.cpp                                                              
		src/audio/filt.cpp                                                             
		src/controller/controller.cpp                                                       
		src/utils/bitplaneshifter.cpp                                                  
		src/utils/circularbuffer.cpp
		src/utils/commandline.cpp
		src/debugger/debugconsole.cpp                                                     
		src/main.cpp                                                             
		src/segasmu.cpp                                                          
		src/sms.cpp                                               
)

# Add an executable with the above sources
add_executable(smsemu ${SMSEMU_SOURCES})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(smsemu
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/src/audio
		${PROJECT_SOURCE_DIR}/src/controller
		${PROJECT_SOURCE_DIR}/src/cpu 
		${PROJECT_SOURCE_DIR}/src/debugger
		${PROJECT_SOURCE_DIR}/src/memory
		${PROJECT_SOURCE_DIR}/src/utils
		${PROJECT_SOURCE_DIR}/src/video
		${PROJECT_SOURCE_DIR}/src
		${SDL3_INCLUDE_DIRS}
		${LOGURU_INCLUDE_DIRS}
)

# Set libraries to be included
target_link_libraries(smsemu
	PRIVATE
		SDL3::SDL3
		loguru::loguru)
