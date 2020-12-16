TIME FOR SOME INFORMATION THEORY!

We need to pack morse code. The most efficient packing I can think of is a stream of base 3 ints:
    0 = END, 1 = DIT, 2 = DAH

We have to encode into 8-bit bytes, so the smallest atomic (quick to index) encoded chunk is:
    int('22222', 3) = 242

Given the last int must be 0, we can encode anything up to length 4.
But I need to encode the prosign  ARK = .-.-.-.- length 8.
So I need two bytes.
Well, if I'm going to use two bytes, I may as well just encode a length  and a bitmask.
