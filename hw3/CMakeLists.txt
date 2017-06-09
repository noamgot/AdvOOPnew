#############################################################
### YOU SHOULD EDIT THE BELOW WITH YOUR DETAILS AND FILES ###
#############################################################

set(user1 benohayon)
set(user2 uribracha)
set(user3 noamgottlieb)
set(id1 "304848518")
set(id2 "313296550")
set(id3 "201606951")

# All source files needed to compile your exe
set (EXE_SOURCE_FILES
	AlgorithmLoader.cpp
	AlgorithmLoader.h
	CommonUtilities.cpp
	CommonUtilities.h
	CompetitionManager.cpp
	CompetitionManager.h
	GameResultsTable.cpp
	GameResultsTable.h
	GameRunner.cpp
	GameRunner.h
	IBattleshipGameAlgo.h
	InitUtilities.cpp
	InitUtilities.h
	Logger.cpp
	Logger.h
	main.cpp
	MyBoardData.h
	PlayerGameResults.h
	SafeQueue.h
	Ship.cpp
	Ship.h
)
# All source files needed to compile your smart algorithm's .dll
set (SMART_ALGO_SOURCE_FILES
	CommonUtilities.cpp
	CommonUtilities.h
	IBattleshipGameAlgo.h
	MyBoardData.h
	Player.cpp
	Player.h
	Ship.cpp
	Ship.h
)

##################################
### DO NOT CHANGE FROM HERE ON ###
##################################

message("user: ${user1}, id: ${id1}")

add_definitions(-DUNICODE -D_UNICODE)

set(EX ex3)

# Set exe build parameters
set(EXE_NAME "${EX}.${user1}")
add_executable( ${EXE_NAME}	${EXE_SOURCE_FILES})
set_property(TARGET ${EXE_NAME} PROPERTY FOLDER "${EX}/${user1}")
set_property(TARGET ${EXE_NAME} PROPERTY OUTPUT_NAME "${user1}")

# Set smart algorithm's .dll build parameters
set(ALG3_NAME "${EX}.${user1}.smart")
add_library(${ALG3_NAME} SHARED ${SMART_ALGO_SOURCE_FILES})
set_property(TARGET ${ALG3_NAME} PROPERTY FOLDER "${EX}/${user1}")
set_property(TARGET ${ALG3_NAME} PROPERTY OUTPUT_NAME "${user1}.smart")
target_compile_definitions(${ALG3_NAME} PRIVATE -DALGO_EXPORTS) # preprocessor flag saying we are the one compiling the header now