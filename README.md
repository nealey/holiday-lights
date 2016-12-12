Holiday Lights
=========

Some twinkling lights for your tree/menorah/office/door/roof/habitrail.


Materials
---------

Uses WS2811 lights, available from Adafruit and fine retailers worldwide.

If you've got more than 100 lights in a string,
you're probably also going to want an external 5v DC power supply.
I found a good 10A power supply for about $12 on Amazon.
You can probably salvage one from some old thing.

If you have under 80 lights, you can use an Adafruit Trinket.
I used an Adafruit Pro Trinket for my 182-light display.
(It's an issue of RAM).

Of course, a standard Arduino will work just fine too!

This was coded to color-correct a specific type of GRB LEDs on wires I got from Amazon.
It's coded to match the lights we already have, which are biased toward yellow and amber.

You may have gotten lights wired GRB, in which case this is going to look very green.
It should be just a matter of switching the first two bytes in each color definition
to go from RGB to GRB.



Setup
-------

Plug your lights into pin 6 (or whatever you set `PIN` to in the code).

Give the strip power and ground.
If you have more than about 80 LEDs, you might need to provide an external
5v power supply, since USB can only provide 2 amps. 
You can plug the LED strip into the +5v on the power supply;
You can power your microcontroller from the beefier power supply, too,
so you don't have to run USB just to power the microcontroller.


Usage
-------

Just provide power.

If you connect `WHITE_PIN` to ground,
everything goes white,
which is a tradition in my family on xmas morning.
