
"protoc.exe" -I=./ --cpp_out=./ ./packet_def.proto 
"protoc.exe" -I=./ --cpp_out=./ ./packet.proto 

copy *.cc ..\server\pb
copy *.h ..\server\pb

rem copy *.cc ..\testclient\pb
rem copy *.h ..\testclient\pb




pause