ramulate
========

this will be an emulator for several video-game-consoles, depending on the radare2-api and allegro.
This will emulate these archs:


Gameboy-z80
  `-you can use http://runas-racer.com/prj/gbdis/hackboy.gb for legal testing. It's a tiny gb-rom I created once for <br>
  testing payloads for gameboy.
    Here is how it works( if you want to play with it):
    arrow keys: normal seek
    start: call current offset
    select: open select-menu
      (in select-menu)
      up and down: seek through modes
      a: choose current mode
      b: abort
      (modes)
      s: seek to a specific 16-bit-offset
      w: write byte to current offset(you can only write on the ram; there is a 64-byte-playground for you)
      d: nothing (was planed to disassemble (sorry, I am lazy))
      
      if you have selected one mode, use the arrow keys to set a value for w and s.

sms-z80

snes


A lot of things need be to done
</p>
