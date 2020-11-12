
PYTHON_ADD_MODULE(epos_boost epos_boost.cpp)
target_link_libraries(epos_boost -lEposCmd)

FILE(COPY test.py DESTINATION .)
ADD_TEST(NAME EPOS4Boost COMMAND ${PYTHON_EXECUTABLE} test.py)
