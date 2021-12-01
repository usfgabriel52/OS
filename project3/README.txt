The extra credit was accomplished by detecting a keyboard input without taking an input,
The function kbhit was craeted to return if the keyboard was hit or not 
After the keyboard is hit, the message has started so the program locks down a shared mutex between the send and receive threads
Until the message is sent, the receive function cannot get the mutex
after the message is sent, the mutex is unlocked and the receive function can now receive the messages