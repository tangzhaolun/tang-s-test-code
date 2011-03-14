set(BIN_PROJECT fpgasim)

include_directories( 	./
						../../
)

link_directories ( ${LIBRARY_PATH} )
 
add_executable(	${BIN_PROJECT}
            	udpreceiver.cpp
            	cframe.cpp
            	cframebuffer.cpp
            	ctestplayer.cpp
)

target_link_libraries(  ${BIN_PROJECT} smscommon)

set_target_properties(  ${BIN_PROJECT} PROPERTIES
   	                RUNTIME_OUTPUT_DIRECTORY ${BINARY_PATH}
)
