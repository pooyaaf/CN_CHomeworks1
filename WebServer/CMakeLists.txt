cmake_minimum_required(VERSION 3.9.1)
project(WebServer)
add_executable(server server.c)
add_executable(client  client.c)   
###########
##Files##
###########  
add_subdirectory(FileData)             
target_include_directories(server PUBLIC file)
target_link_directories(server PUBLIC FileData/)           
target_link_libraries(server file)
