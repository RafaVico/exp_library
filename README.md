# Exp Library
Open library to add exp (achievements) to your games. The library is designed to be FAST for gp2x console and pc.

The library is designed thinking about GP2X console, so the objective is to be fast, and is written in C++.

GP2X games execute from a SD card, the library need to save the exps (achievements) enough fast to avoid lag or stop in the game.

Exp Library is divided by:<br>
&nbsp;&nbsp;&nbsp;&nbsp;- CORE. Main module, with functions that check the exps and save only the needed data to mark them (a little bytes).<br>
&nbsp;&nbsp;&nbsp;&nbsp;- SDL. Graphic module wich show the messages using SDL library. It's easy to add more modules with another graphic libraries.<br>

# Cons

Is not the best written library because it was using only for my games.<br>
Security is low or nothing. It have a simple way of offuscating the exp texts.

# TODO

First thing to do is convert code to simple C. With this the library can be used in BennuGD.

# Games & Tools
Games and tools using this library are:

<table>
<tr>
<td>
<b>EXP APPLICATION</b><br>
With this application you can create a profile for using in games with Exp.<br>
https://rafavico.itch.io/expapp</td>
<td><img src="https://img.itch.zone/aW1hZ2UvNDQ3NDQ5LzIyNjA5NDEucG5n/original/tzo0%2Fz.png" alt="Exp Application"><img src="https://img.itch.zone/aW1hZ2UvNDQ3NDQ5LzIyNjA5NDUucG5n/original/sJ4vnb.png" alt="Exp Application"></td>
</tr>
<tr>
<td>
<b>BATHYSCAPHE</b><br>
A little game to learn using the library.<br>
https://rafavico.itch.io/bathyscaphe</td>
<td><img src="https://img.itch.zone/aW1nLzIyNzQwNTYucG5n/original/xTrQU4.png" alt="Bathyscaphe"></td>
</tr>
<tr>
<td>
<b>ULTRATUMBA</b><br>
A puzzle game against the cpu or another player.<br>
https://rafavico.itch.io/ultratumba</td>
<td><img src="https://img.itch.zone/aW1nLzIyNzU3ODUucG5n/original/bH3G2p.png" alt="Ultratumba"></td>
</tr>
<tr>
<td>
<b>ROCK RAIN 2</b><br>
A platform game where you can play alone or with a friend.<br>
https://rafavico.itch.io/rock-rain-2</td>
<td><img src="https://img.itch.zone/aW1nLzIyNjA5MDcucG5n/original/F9opsu.png" alt="Rock Rain 2"></td>
</tr>
<tr>
<td>
<b>ESCAPE FROM MINOS</b><br>
An isometric game with procedural mazes.<br>
https://rafavico.itch.io/escape-from-minos</td>
<td><img src="https://img.itch.zone/aW1nLzIyOTAyMDkucG5n/original/LrYd4M.png" alt="Escape from Minos"></td>
</tr>
<tr>
<td>
<b>ROOKIE HERO</b><br>
A fast action-platformer game with a lot of statistics.<br>
https://rafavico.itch.io/rookie-hero</td>
<td><img src="https://img.itch.zone/aW1nLzIzMzcxOTQucG5n/original/Yqcqzs.png" alt="Rookie Hero"></td>
</tr>
</table>
