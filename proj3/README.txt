# proj3
cd proj3
make clean
make
./myserver 50000
a:telnet linprog<x> 50000


register <name> <pwd>	 # register a new user (only be allowed in guest account)
command: register shuai s

exit 			 # quit the system
command: 
	shuai:exit

quit			 # quit the system
command: 
	shuai:quit

Login User a, User b and User c:
tell <name> <msg>  	 # tell user <name> message
command: 
	a:tell b Yo!

shout <msg> 		 # shout <msg> to every one online (in game and out game)
command: 
	a:shout hello

help			 # print this message
command: 
	a:help

?			 # print this message
command: 
	a:?

-who			 # List all online users
command: 
	a:who

-stats			 # Display user information
command: 
	a:stats a
	a:stats b

block <id> 		 # No more communication from <id>
command: 
	b:shout abc
	b:tell a abc
	a:block b
	b:shout abc
	b:tell a abc
	 

unblock <id> 		 # Allow communication from <id>
command: 
	a:unblock b
	b:shout abc
	b:tell a abc

quiet 			 # Quiet mode, no broadcast messages
command: 
	b:shout abc
	b:tell a abc
	a:quiet
	b:shout abc
	b:tell a abc

nonquiet 		 # Non-quiet mode
command: 
	a:nonquiet
	b:shout abc
	b:tell a abc

listmail 		 # List the header of the mails
command: 
	a:listmail

mail <id> <title>	 # Send id a mail
Command: 
	a:mail b SayGoodby
	(Your content, end by the ".")

readmail <msg_num> 	 # Read the particular mail
Command: 
	a:readmail 0

deletemail <msg_num> 	 # Delete the particular mail
Command: 
	a: deletemail 0
	a: listmail

-match <name> <b|w> [t]  # Try to start a game 		 
command: 
	a:match b b 100
	 (default) match b (b) (600)
	b:match a 
	 (you cant type b|w and time)

observe <game_id> 	 # Observe a game
command: 
	c:observe 0

unobserve 		 # Unobserve a game
command: 
	c:unobserve 

resign 			 # Resign a game (admit defeat)(if you quit when you are playing, you lose 			 the game)
command: 
	a:resign

-game			 # list all current games
command: 
	c:game

login User o
kibitz <msg> 		 # Comment on a game when observing
command: 
	a:match b
	b:match a
	c:observe 0
	d:observe 0
	c:kibitz Hello! 
	o:kibitz Hello! 

'msg <msg> 		 # Comment on a game when observing
command: 
	c:' Hello! 
	o:' Hello!

refresh 		 # Refresh a game (reload)
command:
	a:refresh
	b:refresh
	c:refresh
	o:refresh	

<A|B|C><1|2|3> 		 # Make a move in a game
command: 
	a:a1
	b:b1
	a:a2
	b:b2
	a:a3
	b:b3

info <msg> 		 # change your information to <msg>
Command: 
	a:info I'm the boss
	b:info what can i say
	a:stats a
	a:stats b

passwd <new>		 # change password
command: 
	a:passwd cd
	a:quit
	a:login




