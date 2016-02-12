* Tools included:

dinput.dll: directinput hook for sp.exe that does the following:
- remove start parameters check
- remove ioprotect check (makes it possible to use other hacking tools that are detected by ioprotect)
- remove screen resolution check (makes it possible to run in a window using dxwnd)
- remove global mutex (makes it possible to run multiple instances)
- fix udp port binding (makes it possible to run multiple instances)
- replace messagebox dialogs with notepad instances (prevents freezing the program when running it with dxwnd)
- prevent it from switching back to the game window after it detects a window switch (currently has some strange side effects when running with dxwnd without the "do not notify on task change" option)
- prevent the disabling of the system hotkeys
- add a chat command to get IPs of all players in a room (just type "-" in the chat, preferably in a whisper to some non-existant user)
- fix a glitch related to the HP of NPCs when 2 players hit them at the same time
- remove the encryption of game data

(to use dinput.dll, put it in the SP folder)

SPContainer: since DXWnd makes SP freeze when you minimize it, I made a program to house the SP window inside so I could minimize it. lol

SPTools: a program for decrypting and encrypting messages easily. it works with the hex representation of messages (eg. 01 23 AB CD ...).



* Other tools that I use (just google them):

- Cheat Engine 6.1 (or whatever the latest one is): for debugging (remember to use VEH debugger, which isn't detected by Themida)
- WPE Pro or rPE: for TCP packet capturing (process scope)
- Wireshark: for TCP and UDP packet capturing (adapter scope)
- DXWnd: to run SP in a window


* Setting up DXWnd:

- target: sp.exe
- DX version: 7
- tick "do not notify on task change"
- tick "directinput is hooked"
- tick "reduce cpu load"
