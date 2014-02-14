ramulate
========

this will be an emulator for several video-game-consoles, depending on the radare2-api.
This will emulate these archs:


Gameboy-z80 <br>
  `-you can use http://runas-racer.com/prj/gbdis/hackboy.gb for legal testing. It's a tiny gb-rom I created once for <br>
  testing payloads for gameboy. <br>
    Here is how it works( if you want to play with it): <br>
    arrow keys: normal seek <br>
    start: call current offset <br>
    select: open select-menu <br>
      (in select-menu) <br>
      up and down: seek through modes <br>
      a: choose current mode <br>
      b: abort <br>
      (modes) <br>
      s: seek to a specific 16-bit-offset <br>
      w: write byte to current offset(you can only write on the ram; there is a 64-byte-playground for you) <br>
      d: nothing (was planed to disassemble (sorry, I am lazy)) <br> <br>
      if you have selected one mode, use the arrow keys to set a value for w and s.

sms-z80

snes

and much more

A lot of things need be to done
