# poc-networking-tcp

https://www.youtube.com/watch?v=2hNdkYInj4g (Important: [22:49 - 26:32], [31:25 - 32:09], [40:40 - 45:00], [47:21 - 47:55])
https://www.youtube.com/watch?v=UbjxGvrDrbw (Important: [00:00 - 7:12])
https://www.youtube.com/watch?v=hHowZ3bWsio (Important: [11:25 - 17:24])
https://www.youtube.com/watch?v=f_1lt9pfaEo (Not implemented in the current POC due to overhead)


For this proof of concept i have pushed the asio library in the directory for quick demo purposes. In normal circumstances you would download this library yourself and the cmake file would try to find it. 

Good to know: 
![image](https://user-images.githubusercontent.com/63774238/192738495-b8d81492-54c9-4b83-a23a-7b8905a0b5df.png)
setting both projects to run in an external console improves usabillity of keyboard input

Keyboard shortcuts: 
1 => Ping server 
2 => Sends a message to all other clients connected to the server (need to run multiple clients to test)
3 => Exit() 

CURRENT POC WORKS FULLY ON LOCALHOST AND HAS NOT BEEN TESTED IN DIFFERENT SETUPS FOR NOW!
