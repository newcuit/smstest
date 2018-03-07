cmake_minimum_required(VERSION 2.6)

PROJECT(sms C)
ADD_DEFINITIONS(-Os -Wall --std=gnu99 -Wmissing-declarations)

FILE(GLOB SOURCES *.c)

ADD_EXECUTABLE(sms ${SOURCES})

INSTALL(TARGETS sms 
        RUNTIME DESTINATION sbin
)
