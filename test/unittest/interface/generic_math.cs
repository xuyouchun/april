// The integer types through the interfaces of the numbers (IBinaryInteger<T>, IMinMaxValue<T>, ISignedNumber<T>,
// IUnsignedNumber<T>): the operators, the constants, and the methods of the numbers and of the bits, with the values
// of the limits. The expected text is made by tools/gen_numerics_test.py (a model of the integers).

/* EXPECT

Int8 min=-128 max=127 zero=0 one=1 allbits=-1 radix=2 addid=0 mulid=1
Int8 negone=-1
Int8 a=127 b=3 add=-126 sub=124 mul=125 div=42 rem=1 neg=-127 and=3 or=127 xor=124 not=-128 shl=-8 shr=31 inc=-128 dec=126 lt=False ge=True eq=False ne=True abs=127 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=6 lzc=1 pop=7 tzc=0 rotl=-5 rotr=-17 max=127 min=3 clamp=3 copysign=127 maxmag=127 minmag=3 divrem=42,1 bytes=1 shortest=7
Int8 a=-128 b=5 add=-123 sub=123 mul=-128 div=-25 rem=-3 neg=-128 and=0 or=-123 xor=-123 not=127 shl=0 shr=-32 inc=-127 dec=127 lt=True ge=False eq=False ne=True abs=ovf sign=-1 neg?=True pos?=False even=True odd=False zero=False pow2=False log2=ovf lzc=0 pop=1 tzc=7 rotl=4 rotr=16 max=5 min=-128 clamp=0 copysign=ovf maxmag=-128 minmag=5 divrem=-25,-3 bytes=1 shortest=8
Int8 a=-37 b=11 add=-26 sub=-48 mul=105 div=-3 rem=-4 neg=37 and=11 or=-37 xor=-48 not=36 shl=-40 shr=-10 inc=-36 dec=-38 lt=True ge=False eq=False ne=True abs=37 sign=-1 neg?=True pos?=False even=False odd=True zero=False pow2=False log2=ovf lzc=0 pop=6 tzc=0 rotl=-34 rotr=123 max=11 min=-37 clamp=0 copysign=37 maxmag=-37 minmag=11 divrem=-3,-4 bytes=1 shortest=7
Int8 a=37 b=-11 add=26 sub=48 mul=105 div=-3 rem=4 neg=-37 and=37 or=-11 xor=-48 not=-38 shl=40 shr=9 inc=38 dec=36 lt=False ge=True eq=False ne=True abs=37 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=5 lzc=2 pop=3 tzc=0 rotl=41 rotr=-92 max=37 min=-11 clamp=0 copysign=-37 maxmag=37 minmag=-11 divrem=-3,4 bytes=1 shortest=6
Int8 a=-127 b=-1 add=-128 sub=-126 mul=127 div=127 rem=0 neg=127 and=-127 or=-1 xor=126 not=126 shl=8 shr=-32 inc=-126 dec=-128 lt=True ge=False eq=False ne=True abs=127 sign=-1 neg?=True pos?=False even=False odd=True zero=False pow2=False log2=ovf lzc=0 pop=2 tzc=0 rotl=12 rotr=48 max=-1 min=-127 clamp=-1 copysign=-127 maxmag=-127 minmag=-1 divrem=127,0 bytes=1 shortest=8
Int8 a=0 b=7 add=7 sub=-7 mul=0 div=0 rem=0 neg=0 and=0 or=7 xor=7 not=-1 shl=0 shr=0 inc=1 dec=-1 lt=True ge=False eq=False ne=True abs=0 sign=0 neg?=False pos?=True even=True odd=False zero=True pow2=False log2=0 lzc=8 pop=0 tzc=8 rotl=0 rotr=0 max=7 min=0 clamp=0 copysign=0 maxmag=7 minmag=0 divrem=0,0 bytes=1 shortest=0
Int8 a=-1 b=-1 add=-2 sub=0 mul=1 div=1 rem=0 neg=1 and=-1 or=-1 xor=0 not=0 shl=-8 shr=-1 inc=0 dec=-2 lt=False ge=True eq=True ne=False abs=1 sign=-1 neg?=True pos?=False even=False odd=True zero=False pow2=False log2=ovf lzc=0 pop=8 tzc=0 rotl=-1 rotr=-1 max=-1 min=-1 clamp=-1 copysign=-1 maxmag=-1 minmag=-1 divrem=1,0 bytes=1 shortest=1
Int8 divrem 42 1
UInt8 min=0 max=255 zero=0 one=1 allbits=255 radix=2 addid=0 mulid=1
UInt8 unsigned=0
UInt8 a=255 b=3 add=2 sub=252 mul=253 div=85 rem=0 neg=1 and=3 or=255 xor=252 not=0 shl=248 shr=63 inc=0 dec=254 lt=False ge=True eq=False ne=True abs=255 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=7 lzc=0 pop=8 tzc=0 rotl=255 rotr=255 max=255 min=3 clamp=3 copysign=255 maxmag=255 minmag=3 divrem=85,0 bytes=1 shortest=8
UInt8 a=127 b=5 add=132 sub=122 mul=123 div=25 rem=2 neg=129 and=5 or=127 xor=122 not=128 shl=248 shr=31 inc=128 dec=126 lt=False ge=True eq=False ne=True abs=127 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=6 lzc=1 pop=7 tzc=0 rotl=251 rotr=239 max=127 min=5 clamp=5 copysign=127 maxmag=127 minmag=5 divrem=25,2 bytes=1 shortest=7
UInt8 a=37 b=11 add=48 sub=26 mul=151 div=3 rem=4 neg=219 and=1 or=47 xor=46 not=218 shl=40 shr=9 inc=38 dec=36 lt=False ge=True eq=False ne=True abs=37 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=5 lzc=2 pop=3 tzc=0 rotl=41 rotr=164 max=37 min=11 clamp=11 copysign=37 maxmag=37 minmag=11 divrem=3,4 bytes=1 shortest=6
UInt8 a=11 b=37 add=48 sub=230 mul=151 div=0 rem=11 neg=245 and=1 or=47 xor=46 not=244 shl=88 shr=2 inc=12 dec=10 lt=True ge=False eq=False ne=True abs=11 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=3 lzc=4 pop=3 tzc=0 rotl=88 rotr=97 max=37 min=11 clamp=11 copysign=11 maxmag=37 minmag=11 divrem=0,11 bytes=1 shortest=4
UInt8 a=1 b=255 add=0 sub=2 mul=255 div=0 rem=1 neg=255 and=1 or=255 xor=254 not=254 shl=8 shr=0 inc=2 dec=0 lt=True ge=False eq=False ne=True abs=1 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=True log2=0 lzc=7 pop=1 tzc=0 rotl=8 rotr=32 max=255 min=1 clamp=1 copysign=1 maxmag=255 minmag=1 divrem=0,1 bytes=1 shortest=1
UInt8 a=0 b=7 add=7 sub=249 mul=0 div=0 rem=0 neg=0 and=0 or=7 xor=7 not=255 shl=0 shr=0 inc=1 dec=255 lt=True ge=False eq=False ne=True abs=0 sign=0 neg?=False pos?=True even=True odd=False zero=True pow2=False log2=0 lzc=8 pop=0 tzc=8 rotl=0 rotr=0 max=7 min=0 clamp=0 copysign=0 maxmag=7 minmag=0 divrem=0,0 bytes=1 shortest=0
UInt8 divrem 85 0
Int16 min=-32768 max=32767 zero=0 one=1 allbits=-1 radix=2 addid=0 mulid=1
Int16 negone=-1
Int16 a=32767 b=3 add=-32766 sub=32764 mul=32765 div=10922 rem=1 neg=-32767 and=3 or=32767 xor=32764 not=-32768 shl=-8 shr=8191 inc=-32768 dec=32766 lt=False ge=True eq=False ne=True abs=32767 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=14 lzc=1 pop=15 tzc=0 rotl=-5 rotr=-4097 max=32767 min=3 clamp=3 copysign=32767 maxmag=32767 minmag=3 divrem=10922,1 bytes=2 shortest=15
Int16 a=-32768 b=5 add=-32763 sub=32763 mul=-32768 div=-6553 rem=-3 neg=-32768 and=0 or=-32763 xor=-32763 not=32767 shl=0 shr=-8192 inc=-32767 dec=32767 lt=True ge=False eq=False ne=True abs=ovf sign=-1 neg?=True pos?=False even=True odd=False zero=False pow2=False log2=ovf lzc=0 pop=1 tzc=15 rotl=4 rotr=4096 max=5 min=-32768 clamp=0 copysign=ovf maxmag=-32768 minmag=5 divrem=-6553,-3 bytes=2 shortest=16
Int16 a=-37 b=11 add=-26 sub=-48 mul=-407 div=-3 rem=-4 neg=37 and=11 or=-37 xor=-48 not=36 shl=-296 shr=-10 inc=-36 dec=-38 lt=True ge=False eq=False ne=True abs=37 sign=-1 neg?=True pos?=False even=False odd=True zero=False pow2=False log2=ovf lzc=0 pop=14 tzc=0 rotl=-289 rotr=32763 max=11 min=-37 clamp=0 copysign=37 maxmag=-37 minmag=11 divrem=-3,-4 bytes=2 shortest=7
Int16 a=37 b=-11 add=26 sub=48 mul=-407 div=-3 rem=4 neg=-37 and=37 or=-11 xor=-48 not=-38 shl=296 shr=9 inc=38 dec=36 lt=False ge=True eq=False ne=True abs=37 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=5 lzc=10 pop=3 tzc=0 rotl=296 rotr=-24572 max=37 min=-11 clamp=0 copysign=-37 maxmag=37 minmag=-11 divrem=-3,4 bytes=2 shortest=6
Int16 a=-32767 b=-1 add=-32768 sub=-32766 mul=32767 div=32767 rem=0 neg=32767 and=-32767 or=-1 xor=32766 not=32766 shl=8 shr=-8192 inc=-32766 dec=-32768 lt=True ge=False eq=False ne=True abs=32767 sign=-1 neg?=True pos?=False even=False odd=True zero=False pow2=False log2=ovf lzc=0 pop=2 tzc=0 rotl=12 rotr=12288 max=-1 min=-32767 clamp=-1 copysign=-32767 maxmag=-32767 minmag=-1 divrem=32767,0 bytes=2 shortest=16
Int16 a=0 b=7 add=7 sub=-7 mul=0 div=0 rem=0 neg=0 and=0 or=7 xor=7 not=-1 shl=0 shr=0 inc=1 dec=-1 lt=True ge=False eq=False ne=True abs=0 sign=0 neg?=False pos?=True even=True odd=False zero=True pow2=False log2=0 lzc=16 pop=0 tzc=16 rotl=0 rotr=0 max=7 min=0 clamp=0 copysign=0 maxmag=7 minmag=0 divrem=0,0 bytes=2 shortest=0
Int16 a=-1 b=-1 add=-2 sub=0 mul=1 div=1 rem=0 neg=1 and=-1 or=-1 xor=0 not=0 shl=-8 shr=-1 inc=0 dec=-2 lt=False ge=True eq=True ne=False abs=1 sign=-1 neg?=True pos?=False even=False odd=True zero=False pow2=False log2=ovf lzc=0 pop=16 tzc=0 rotl=-1 rotr=-1 max=-1 min=-1 clamp=-1 copysign=-1 maxmag=-1 minmag=-1 divrem=1,0 bytes=2 shortest=1
Int16 divrem 10922 1
UInt16 min=0 max=65535 zero=0 one=1 allbits=65535 radix=2 addid=0 mulid=1
UInt16 unsigned=0
UInt16 a=65535 b=3 add=2 sub=65532 mul=65533 div=21845 rem=0 neg=1 and=3 or=65535 xor=65532 not=0 shl=65528 shr=16383 inc=0 dec=65534 lt=False ge=True eq=False ne=True abs=65535 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=15 lzc=0 pop=16 tzc=0 rotl=65535 rotr=65535 max=65535 min=3 clamp=3 copysign=65535 maxmag=65535 minmag=3 divrem=21845,0 bytes=2 shortest=16
UInt16 a=32767 b=5 add=32772 sub=32762 mul=32763 div=6553 rem=2 neg=32769 and=5 or=32767 xor=32762 not=32768 shl=65528 shr=8191 inc=32768 dec=32766 lt=False ge=True eq=False ne=True abs=32767 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=14 lzc=1 pop=15 tzc=0 rotl=65531 rotr=61439 max=32767 min=5 clamp=5 copysign=32767 maxmag=32767 minmag=5 divrem=6553,2 bytes=2 shortest=15
UInt16 a=37 b=11 add=48 sub=26 mul=407 div=3 rem=4 neg=65499 and=1 or=47 xor=46 not=65498 shl=296 shr=9 inc=38 dec=36 lt=False ge=True eq=False ne=True abs=37 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=5 lzc=10 pop=3 tzc=0 rotl=296 rotr=40964 max=37 min=11 clamp=11 copysign=37 maxmag=37 minmag=11 divrem=3,4 bytes=2 shortest=6
UInt16 a=11 b=37 add=48 sub=65510 mul=407 div=0 rem=11 neg=65525 and=1 or=47 xor=46 not=65524 shl=88 shr=2 inc=12 dec=10 lt=True ge=False eq=False ne=True abs=11 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=3 lzc=12 pop=3 tzc=0 rotl=88 rotr=24577 max=37 min=11 clamp=11 copysign=11 maxmag=37 minmag=11 divrem=0,11 bytes=2 shortest=4
UInt16 a=1 b=65535 add=0 sub=2 mul=65535 div=0 rem=1 neg=65535 and=1 or=65535 xor=65534 not=65534 shl=8 shr=0 inc=2 dec=0 lt=True ge=False eq=False ne=True abs=1 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=True log2=0 lzc=15 pop=1 tzc=0 rotl=8 rotr=8192 max=65535 min=1 clamp=1 copysign=1 maxmag=65535 minmag=1 divrem=0,1 bytes=2 shortest=1
UInt16 a=0 b=7 add=7 sub=65529 mul=0 div=0 rem=0 neg=0 and=0 or=7 xor=7 not=65535 shl=0 shr=0 inc=1 dec=65535 lt=True ge=False eq=False ne=True abs=0 sign=0 neg?=False pos?=True even=True odd=False zero=True pow2=False log2=0 lzc=16 pop=0 tzc=16 rotl=0 rotr=0 max=7 min=0 clamp=0 copysign=0 maxmag=7 minmag=0 divrem=0,0 bytes=2 shortest=0
UInt16 divrem 21845 0
Int32 min=-2147483648 max=2147483647 zero=0 one=1 allbits=-1 radix=2 addid=0 mulid=1
Int32 negone=-1
Int32 a=2147483647 b=3 add=-2147483646 sub=2147483644 mul=2147483645 div=715827882 rem=1 neg=-2147483647 and=3 or=2147483647 xor=2147483644 not=-2147483648 shl=-8 shr=536870911 inc=-2147483648 dec=2147483646 lt=False ge=True eq=False ne=True abs=2147483647 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=30 lzc=1 pop=31 tzc=0 rotl=-5 rotr=-268435457 max=2147483647 min=3 clamp=3 copysign=2147483647 maxmag=2147483647 minmag=3 divrem=715827882,1 bytes=4 shortest=31
Int32 a=-2147483648 b=5 add=-2147483643 sub=2147483643 mul=-2147483648 div=-429496729 rem=-3 neg=-2147483648 and=0 or=-2147483643 xor=-2147483643 not=2147483647 shl=0 shr=-536870912 inc=-2147483647 dec=2147483647 lt=True ge=False eq=False ne=True abs=ovf sign=-1 neg?=True pos?=False even=True odd=False zero=False pow2=False log2=ovf lzc=0 pop=1 tzc=31 rotl=4 rotr=268435456 max=5 min=-2147483648 clamp=0 copysign=ovf maxmag=-2147483648 minmag=5 divrem=-429496729,-3 bytes=4 shortest=32
Int32 a=-37 b=11 add=-26 sub=-48 mul=-407 div=-3 rem=-4 neg=37 and=11 or=-37 xor=-48 not=36 shl=-296 shr=-10 inc=-36 dec=-38 lt=True ge=False eq=False ne=True abs=37 sign=-1 neg?=True pos?=False even=False odd=True zero=False pow2=False log2=ovf lzc=0 pop=30 tzc=0 rotl=-289 rotr=2147483643 max=11 min=-37 clamp=0 copysign=37 maxmag=-37 minmag=11 divrem=-3,-4 bytes=4 shortest=7
Int32 a=37 b=-11 add=26 sub=48 mul=-407 div=-3 rem=4 neg=-37 and=37 or=-11 xor=-48 not=-38 shl=296 shr=9 inc=38 dec=36 lt=False ge=True eq=False ne=True abs=37 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=5 lzc=26 pop=3 tzc=0 rotl=296 rotr=-1610612732 max=37 min=-11 clamp=0 copysign=-37 maxmag=37 minmag=-11 divrem=-3,4 bytes=4 shortest=6
Int32 a=-2147483647 b=-1 add=-2147483648 sub=-2147483646 mul=2147483647 div=2147483647 rem=0 neg=2147483647 and=-2147483647 or=-1 xor=2147483646 not=2147483646 shl=8 shr=-536870912 inc=-2147483646 dec=-2147483648 lt=True ge=False eq=False ne=True abs=2147483647 sign=-1 neg?=True pos?=False even=False odd=True zero=False pow2=False log2=ovf lzc=0 pop=2 tzc=0 rotl=12 rotr=805306368 max=-1 min=-2147483647 clamp=-1 copysign=-2147483647 maxmag=-2147483647 minmag=-1 divrem=2147483647,0 bytes=4 shortest=32
Int32 a=0 b=7 add=7 sub=-7 mul=0 div=0 rem=0 neg=0 and=0 or=7 xor=7 not=-1 shl=0 shr=0 inc=1 dec=-1 lt=True ge=False eq=False ne=True abs=0 sign=0 neg?=False pos?=True even=True odd=False zero=True pow2=False log2=0 lzc=32 pop=0 tzc=32 rotl=0 rotr=0 max=7 min=0 clamp=0 copysign=0 maxmag=7 minmag=0 divrem=0,0 bytes=4 shortest=0
Int32 a=-1 b=-1 add=-2 sub=0 mul=1 div=1 rem=0 neg=1 and=-1 or=-1 xor=0 not=0 shl=-8 shr=-1 inc=0 dec=-2 lt=False ge=True eq=True ne=False abs=1 sign=-1 neg?=True pos?=False even=False odd=True zero=False pow2=False log2=ovf lzc=0 pop=32 tzc=0 rotl=-1 rotr=-1 max=-1 min=-1 clamp=-1 copysign=-1 maxmag=-1 minmag=-1 divrem=1,0 bytes=4 shortest=1
Int32 divrem 715827882 1
UInt32 min=0 max=4294967295 zero=0 one=1 allbits=4294967295 radix=2 addid=0 mulid=1
UInt32 unsigned=0
UInt32 a=4294967295 b=3 add=2 sub=4294967292 mul=4294967293 div=1431655765 rem=0 neg=1 and=3 or=4294967295 xor=4294967292 not=0 shl=4294967288 shr=1073741823 inc=0 dec=4294967294 lt=False ge=True eq=False ne=True abs=4294967295 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=31 lzc=0 pop=32 tzc=0 rotl=4294967295 rotr=4294967295 max=4294967295 min=3 clamp=3 copysign=4294967295 maxmag=4294967295 minmag=3 divrem=1431655765,0 bytes=4 shortest=32
UInt32 a=2147483647 b=5 add=2147483652 sub=2147483642 mul=2147483643 div=429496729 rem=2 neg=2147483649 and=5 or=2147483647 xor=2147483642 not=2147483648 shl=4294967288 shr=536870911 inc=2147483648 dec=2147483646 lt=False ge=True eq=False ne=True abs=2147483647 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=30 lzc=1 pop=31 tzc=0 rotl=4294967291 rotr=4026531839 max=2147483647 min=5 clamp=5 copysign=2147483647 maxmag=2147483647 minmag=5 divrem=429496729,2 bytes=4 shortest=31
UInt32 a=37 b=11 add=48 sub=26 mul=407 div=3 rem=4 neg=4294967259 and=1 or=47 xor=46 not=4294967258 shl=296 shr=9 inc=38 dec=36 lt=False ge=True eq=False ne=True abs=37 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=5 lzc=26 pop=3 tzc=0 rotl=296 rotr=2684354564 max=37 min=11 clamp=11 copysign=37 maxmag=37 minmag=11 divrem=3,4 bytes=4 shortest=6
UInt32 a=11 b=37 add=48 sub=4294967270 mul=407 div=0 rem=11 neg=4294967285 and=1 or=47 xor=46 not=4294967284 shl=88 shr=2 inc=12 dec=10 lt=True ge=False eq=False ne=True abs=11 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=3 lzc=28 pop=3 tzc=0 rotl=88 rotr=1610612737 max=37 min=11 clamp=11 copysign=11 maxmag=37 minmag=11 divrem=0,11 bytes=4 shortest=4
UInt32 a=1 b=4294967295 add=0 sub=2 mul=4294967295 div=0 rem=1 neg=4294967295 and=1 or=4294967295 xor=4294967294 not=4294967294 shl=8 shr=0 inc=2 dec=0 lt=True ge=False eq=False ne=True abs=1 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=True log2=0 lzc=31 pop=1 tzc=0 rotl=8 rotr=536870912 max=4294967295 min=1 clamp=1 copysign=1 maxmag=4294967295 minmag=1 divrem=0,1 bytes=4 shortest=1
UInt32 a=0 b=7 add=7 sub=4294967289 mul=0 div=0 rem=0 neg=0 and=0 or=7 xor=7 not=4294967295 shl=0 shr=0 inc=1 dec=4294967295 lt=True ge=False eq=False ne=True abs=0 sign=0 neg?=False pos?=True even=True odd=False zero=True pow2=False log2=0 lzc=32 pop=0 tzc=32 rotl=0 rotr=0 max=7 min=0 clamp=0 copysign=0 maxmag=7 minmag=0 divrem=0,0 bytes=4 shortest=0
UInt32 divrem 1431655765 0
Int64 min=-9223372036854775808 max=9223372036854775807 zero=0 one=1 allbits=-1 radix=2 addid=0 mulid=1
Int64 negone=-1
Int64 a=9223372036854775807 b=3 add=-9223372036854775806 sub=9223372036854775804 mul=9223372036854775805 div=3074457345618258602 rem=1 neg=-9223372036854775807 and=3 or=9223372036854775807 xor=9223372036854775804 not=-9223372036854775808 shl=-8 shr=2305843009213693951 inc=-9223372036854775808 dec=9223372036854775806 lt=False ge=True eq=False ne=True abs=9223372036854775807 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=62 lzc=1 pop=63 tzc=0 rotl=-5 rotr=-1152921504606846977 max=9223372036854775807 min=3 clamp=3 copysign=9223372036854775807 maxmag=9223372036854775807 minmag=3 divrem=3074457345618258602,1 bytes=8 shortest=63
Int64 a=-9223372036854775808 b=5 add=-9223372036854775803 sub=9223372036854775803 mul=-9223372036854775808 div=-1844674407370955161 rem=-3 neg=-9223372036854775808 and=0 or=-9223372036854775803 xor=-9223372036854775803 not=9223372036854775807 shl=0 shr=-2305843009213693952 inc=-9223372036854775807 dec=9223372036854775807 lt=True ge=False eq=False ne=True abs=ovf sign=-1 neg?=True pos?=False even=True odd=False zero=False pow2=False log2=ovf lzc=0 pop=1 tzc=63 rotl=4 rotr=1152921504606846976 max=5 min=-9223372036854775808 clamp=0 copysign=ovf maxmag=-9223372036854775808 minmag=5 divrem=-1844674407370955161,-3 bytes=8 shortest=64
Int64 a=-37 b=11 add=-26 sub=-48 mul=-407 div=-3 rem=-4 neg=37 and=11 or=-37 xor=-48 not=36 shl=-296 shr=-10 inc=-36 dec=-38 lt=True ge=False eq=False ne=True abs=37 sign=-1 neg?=True pos?=False even=False odd=True zero=False pow2=False log2=ovf lzc=0 pop=62 tzc=0 rotl=-289 rotr=9223372036854775803 max=11 min=-37 clamp=0 copysign=37 maxmag=-37 minmag=11 divrem=-3,-4 bytes=8 shortest=7
Int64 a=37 b=-11 add=26 sub=48 mul=-407 div=-3 rem=4 neg=-37 and=37 or=-11 xor=-48 not=-38 shl=296 shr=9 inc=38 dec=36 lt=False ge=True eq=False ne=True abs=37 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=5 lzc=58 pop=3 tzc=0 rotl=296 rotr=-6917529027641081852 max=37 min=-11 clamp=0 copysign=-37 maxmag=37 minmag=-11 divrem=-3,4 bytes=8 shortest=6
Int64 a=-9223372036854775807 b=-1 add=-9223372036854775808 sub=-9223372036854775806 mul=9223372036854775807 div=9223372036854775807 rem=0 neg=9223372036854775807 and=-9223372036854775807 or=-1 xor=9223372036854775806 not=9223372036854775806 shl=8 shr=-2305843009213693952 inc=-9223372036854775806 dec=-9223372036854775808 lt=True ge=False eq=False ne=True abs=9223372036854775807 sign=-1 neg?=True pos?=False even=False odd=True zero=False pow2=False log2=ovf lzc=0 pop=2 tzc=0 rotl=12 rotr=3458764513820540928 max=-1 min=-9223372036854775807 clamp=-1 copysign=-9223372036854775807 maxmag=-9223372036854775807 minmag=-1 divrem=9223372036854775807,0 bytes=8 shortest=64
Int64 a=0 b=7 add=7 sub=-7 mul=0 div=0 rem=0 neg=0 and=0 or=7 xor=7 not=-1 shl=0 shr=0 inc=1 dec=-1 lt=True ge=False eq=False ne=True abs=0 sign=0 neg?=False pos?=True even=True odd=False zero=True pow2=False log2=0 lzc=64 pop=0 tzc=64 rotl=0 rotr=0 max=7 min=0 clamp=0 copysign=0 maxmag=7 minmag=0 divrem=0,0 bytes=8 shortest=0
Int64 a=-1 b=-1 add=-2 sub=0 mul=1 div=1 rem=0 neg=1 and=-1 or=-1 xor=0 not=0 shl=-8 shr=-1 inc=0 dec=-2 lt=False ge=True eq=True ne=False abs=1 sign=-1 neg?=True pos?=False even=False odd=True zero=False pow2=False log2=ovf lzc=0 pop=64 tzc=0 rotl=-1 rotr=-1 max=-1 min=-1 clamp=-1 copysign=-1 maxmag=-1 minmag=-1 divrem=1,0 bytes=8 shortest=1
Int64 divrem 3074457345618258602 1
UInt64 min=0 max=18446744073709551615 zero=0 one=1 allbits=18446744073709551615 radix=2 addid=0 mulid=1
UInt64 unsigned=0
UInt64 a=18446744073709551615 b=3 add=2 sub=18446744073709551612 mul=18446744073709551613 div=6148914691236517205 rem=0 neg=1 and=3 or=18446744073709551615 xor=18446744073709551612 not=0 shl=18446744073709551608 shr=4611686018427387903 inc=0 dec=18446744073709551614 lt=False ge=True eq=False ne=True abs=18446744073709551615 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=63 lzc=0 pop=64 tzc=0 rotl=18446744073709551615 rotr=18446744073709551615 max=18446744073709551615 min=3 clamp=3 copysign=18446744073709551615 maxmag=18446744073709551615 minmag=3 divrem=6148914691236517205,0 bytes=8 shortest=64
UInt64 a=9223372036854775807 b=5 add=9223372036854775812 sub=9223372036854775802 mul=9223372036854775803 div=1844674407370955161 rem=2 neg=9223372036854775809 and=5 or=9223372036854775807 xor=9223372036854775802 not=9223372036854775808 shl=18446744073709551608 shr=2305843009213693951 inc=9223372036854775808 dec=9223372036854775806 lt=False ge=True eq=False ne=True abs=9223372036854775807 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=62 lzc=1 pop=63 tzc=0 rotl=18446744073709551611 rotr=17293822569102704639 max=9223372036854775807 min=5 clamp=5 copysign=9223372036854775807 maxmag=9223372036854775807 minmag=5 divrem=1844674407370955161,2 bytes=8 shortest=63
UInt64 a=37 b=11 add=48 sub=26 mul=407 div=3 rem=4 neg=18446744073709551579 and=1 or=47 xor=46 not=18446744073709551578 shl=296 shr=9 inc=38 dec=36 lt=False ge=True eq=False ne=True abs=37 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=5 lzc=58 pop=3 tzc=0 rotl=296 rotr=11529215046068469764 max=37 min=11 clamp=11 copysign=37 maxmag=37 minmag=11 divrem=3,4 bytes=8 shortest=6
UInt64 a=11 b=37 add=48 sub=18446744073709551590 mul=407 div=0 rem=11 neg=18446744073709551605 and=1 or=47 xor=46 not=18446744073709551604 shl=88 shr=2 inc=12 dec=10 lt=True ge=False eq=False ne=True abs=11 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=False log2=3 lzc=60 pop=3 tzc=0 rotl=88 rotr=6917529027641081857 max=37 min=11 clamp=11 copysign=11 maxmag=37 minmag=11 divrem=0,11 bytes=8 shortest=4
UInt64 a=1 b=18446744073709551615 add=0 sub=2 mul=18446744073709551615 div=0 rem=1 neg=18446744073709551615 and=1 or=18446744073709551615 xor=18446744073709551614 not=18446744073709551614 shl=8 shr=0 inc=2 dec=0 lt=True ge=False eq=False ne=True abs=1 sign=1 neg?=False pos?=True even=False odd=True zero=False pow2=True log2=0 lzc=63 pop=1 tzc=0 rotl=8 rotr=2305843009213693952 max=18446744073709551615 min=1 clamp=1 copysign=1 maxmag=18446744073709551615 minmag=1 divrem=0,1 bytes=8 shortest=1
UInt64 a=0 b=7 add=7 sub=18446744073709551609 mul=0 div=0 rem=0 neg=0 and=0 or=7 xor=7 not=18446744073709551615 shl=0 shr=0 inc=1 dec=18446744073709551615 lt=True ge=False eq=False ne=True abs=0 sign=0 neg?=False pos?=True even=True odd=False zero=True pow2=False log2=0 lzc=64 pop=0 tzc=64 rotl=0 rotr=0 max=7 min=0 clamp=0 copysign=0 maxmag=7 minmag=0 divrem=0,0 bytes=8 shortest=0
UInt64 divrem 6148914691236517205 0

*/

import System;

using System;
using System.Numerics;

class Util
{
    public static String Constants<T>() where T : IBinaryInteger<T>, IMinMaxValue<T>, IStringable
    {
        return "min=" + T.MinValue.ToString() + " max=" + T.MaxValue.ToString() + " zero=" + T.Zero.ToString()
            + " one=" + T.One.ToString() + " allbits=" + T.AllBitsSet.ToString() + " radix=" + T.Radix
            + " addid=" + T.AdditiveIdentity.ToString() + " mulid=" + T.MultiplicativeIdentity.ToString();
    }

    public static String NegativeOne<T>() where T : ISignedNumber<T>, IStringable
    {
        return "negone=" + T.NegativeOne.ToString();
    }

    public static String Unsigned<T>() where T : IUnsignedNumber<T>, IStringable
    {
        return "unsigned=" + T.Zero.ToString();
    }

    public static String Ops<T>(T a, T b) where T : IBinaryInteger<T>, IStringable
    {
        String r = "a=" + a.ToString() + " b=" + b.ToString();

        r += " add=" + (a + b).ToString();
        r += " sub=" + (a - b).ToString();
        T t;
        t = a * b;
        r += " mul=" + t.ToString();
        t = a / b;
        r += " div=" + t.ToString();
        t = a % b;
        r += " rem=" + t.ToString();
        r += " neg=" + (-a).ToString();
        t = a & b;
        r += " and=" + t.ToString();
        t = a | b;
        r += " or=" + t.ToString();
        t = a ^ b;
        r += " xor=" + t.ToString();
        t = ~a;
        r += " not=" + t.ToString();
        t = a << 3;
        r += " shl=" + t.ToString();
        t = a >> 2;
        r += " shr=" + t.ToString();

        t = a;
        t++;
        r += " inc=" + t.ToString();
        t = a;
        t--;
        r += " dec=" + t.ToString();

        r += " lt=" + (a < b) + " ge=" + (a >= b) + " eq=" + (a == b) + " ne=" + (a != b);

        try
        {
            t = T.Abs(a);
            r += " abs=" + t.ToString();
        }
        catch (OverflowException)
        {
            r += " abs=ovf";
        }

        r += " sign=" + T.Sign(a) + " neg?=" + T.IsNegative(a) + " pos?=" + T.IsPositive(a) + " even=" + T.IsEvenInteger(a)
            + " odd=" + T.IsOddInteger(a) + " zero=" + T.IsZero(a) + " pow2=" + T.IsPow2(a);

        try
        {
            t = T.Log2(a);
            r += " log2=" + t.ToString();
        }
        catch (OverflowException)
        {
            r += " log2=ovf";
        }

        r += " lzc=" + T.LeadingZeroCount(a).ToString();
        t = T.PopCount(a);
        r += " pop=" + t.ToString();
        t = T.TrailingZeroCount(a);
        r += " tzc=" + t.ToString();
        t = T.RotateLeft(a, 3);
        r += " rotl=" + t.ToString();
        t = T.RotateRight(a, 3);
        r += " rotr=" + t.ToString();

        t = T.Max(a, b);
        r += " max=" + t.ToString();
        t = T.Min(a, b);
        r += " min=" + t.ToString();

        T lo = T.Min(T.Zero, b);
        T hi = T.Max(T.Zero, b);
        t = T.Clamp(a, lo, hi);
        r += " clamp=" + t.ToString();

        try
        {
            t = T.CopySign(a, b);
            r += " copysign=" + t.ToString();
        }
        catch (OverflowException)
        {
            r += " copysign=ovf";
        }

        t = T.MaxMagnitude(a, b);
        r += " maxmag=" + t.ToString();
        t = T.MinMagnitude(a, b);
        r += " minmag=" + t.ToString();

        Tuple<T, T> dr = T.DivRem(a, b);
        T quotient = dr.Item1;
        T remainder = dr.Item2;
        r += " divrem=" + quotient.ToString() + "," + remainder.ToString();

        r += " bytes=" + a.GetByteCount() + " shortest=" + a.GetShortestBitLength();
        return r;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("Int8 " + Util.Constants<Int8>());
        Console.WriteLine("Int8 " + Util.NegativeOne<Int8>());
        Console.WriteLine("Int8 " + Util.Ops(Int8.MaxValue, (Int8)3));
        Console.WriteLine("Int8 " + Util.Ops(Int8.MinValue, (Int8)5));
        Console.WriteLine("Int8 " + Util.Ops((Int8)(-37), (Int8)11));
        Console.WriteLine("Int8 " + Util.Ops((Int8)37, (Int8)(-11)));
        Console.WriteLine("Int8 " + Util.Ops((Int8)(-127), (Int8)(-1)));
        Console.WriteLine("Int8 " + Util.Ops((Int8)0, (Int8)7));
        Console.WriteLine("Int8 " + Util.Ops((Int8)(-1), (Int8)(-1)));
        Tuple<Int8, Int8> Int8_dr = Int8.DivRem(Int8.MaxValue, (Int8)3);
        Console.WriteLine("Int8 divrem " + Int8_dr.Item1 + " " + Int8_dr.Item2);
        Console.WriteLine("UInt8 " + Util.Constants<UInt8>());
        Console.WriteLine("UInt8 " + Util.Unsigned<UInt8>());
        Console.WriteLine("UInt8 " + Util.Ops(UInt8.MaxValue, (UInt8)3));
        Console.WriteLine("UInt8 " + Util.Ops((UInt8)127, (UInt8)5));
        Console.WriteLine("UInt8 " + Util.Ops((UInt8)37, (UInt8)11));
        Console.WriteLine("UInt8 " + Util.Ops((UInt8)11, (UInt8)37));
        Console.WriteLine("UInt8 " + Util.Ops((UInt8)1, UInt8.MaxValue));
        Console.WriteLine("UInt8 " + Util.Ops(UInt8.MinValue, (UInt8)7));
        Tuple<UInt8, UInt8> UInt8_dr = UInt8.DivRem(UInt8.MaxValue, (UInt8)3);
        Console.WriteLine("UInt8 divrem " + UInt8_dr.Item1 + " " + UInt8_dr.Item2);
        Console.WriteLine("Int16 " + Util.Constants<Int16>());
        Console.WriteLine("Int16 " + Util.NegativeOne<Int16>());
        Console.WriteLine("Int16 " + Util.Ops(Int16.MaxValue, (Int16)3));
        Console.WriteLine("Int16 " + Util.Ops(Int16.MinValue, (Int16)5));
        Console.WriteLine("Int16 " + Util.Ops((Int16)(-37), (Int16)11));
        Console.WriteLine("Int16 " + Util.Ops((Int16)37, (Int16)(-11)));
        Console.WriteLine("Int16 " + Util.Ops((Int16)(-32767), (Int16)(-1)));
        Console.WriteLine("Int16 " + Util.Ops((Int16)0, (Int16)7));
        Console.WriteLine("Int16 " + Util.Ops((Int16)(-1), (Int16)(-1)));
        Tuple<Int16, Int16> Int16_dr = Int16.DivRem(Int16.MaxValue, (Int16)3);
        Console.WriteLine("Int16 divrem " + Int16_dr.Item1 + " " + Int16_dr.Item2);
        Console.WriteLine("UInt16 " + Util.Constants<UInt16>());
        Console.WriteLine("UInt16 " + Util.Unsigned<UInt16>());
        Console.WriteLine("UInt16 " + Util.Ops(UInt16.MaxValue, (UInt16)3));
        Console.WriteLine("UInt16 " + Util.Ops((UInt16)32767, (UInt16)5));
        Console.WriteLine("UInt16 " + Util.Ops((UInt16)37, (UInt16)11));
        Console.WriteLine("UInt16 " + Util.Ops((UInt16)11, (UInt16)37));
        Console.WriteLine("UInt16 " + Util.Ops((UInt16)1, UInt16.MaxValue));
        Console.WriteLine("UInt16 " + Util.Ops(UInt16.MinValue, (UInt16)7));
        Tuple<UInt16, UInt16> UInt16_dr = UInt16.DivRem(UInt16.MaxValue, (UInt16)3);
        Console.WriteLine("UInt16 divrem " + UInt16_dr.Item1 + " " + UInt16_dr.Item2);
        Console.WriteLine("Int32 " + Util.Constants<Int32>());
        Console.WriteLine("Int32 " + Util.NegativeOne<Int32>());
        Console.WriteLine("Int32 " + Util.Ops(Int32.MaxValue, (Int32)3));
        Console.WriteLine("Int32 " + Util.Ops(Int32.MinValue, (Int32)5));
        Console.WriteLine("Int32 " + Util.Ops((Int32)(-37), (Int32)11));
        Console.WriteLine("Int32 " + Util.Ops((Int32)37, (Int32)(-11)));
        Console.WriteLine("Int32 " + Util.Ops((Int32)(-2147483647), (Int32)(-1)));
        Console.WriteLine("Int32 " + Util.Ops((Int32)0, (Int32)7));
        Console.WriteLine("Int32 " + Util.Ops((Int32)(-1), (Int32)(-1)));
        Tuple<Int32, Int32> Int32_dr = Int32.DivRem(Int32.MaxValue, (Int32)3);
        Console.WriteLine("Int32 divrem " + Int32_dr.Item1 + " " + Int32_dr.Item2);
        Console.WriteLine("UInt32 " + Util.Constants<UInt32>());
        Console.WriteLine("UInt32 " + Util.Unsigned<UInt32>());
        Console.WriteLine("UInt32 " + Util.Ops(UInt32.MaxValue, (UInt32)3));
        Console.WriteLine("UInt32 " + Util.Ops((UInt32)2147483647, (UInt32)5));
        Console.WriteLine("UInt32 " + Util.Ops((UInt32)37, (UInt32)11));
        Console.WriteLine("UInt32 " + Util.Ops((UInt32)11, (UInt32)37));
        Console.WriteLine("UInt32 " + Util.Ops((UInt32)1, UInt32.MaxValue));
        Console.WriteLine("UInt32 " + Util.Ops(UInt32.MinValue, (UInt32)7));
        Tuple<UInt32, UInt32> UInt32_dr = UInt32.DivRem(UInt32.MaxValue, (UInt32)3);
        Console.WriteLine("UInt32 divrem " + UInt32_dr.Item1 + " " + UInt32_dr.Item2);
        Console.WriteLine("Int64 " + Util.Constants<Int64>());
        Console.WriteLine("Int64 " + Util.NegativeOne<Int64>());
        Console.WriteLine("Int64 " + Util.Ops(Int64.MaxValue, (Int64)3));
        Console.WriteLine("Int64 " + Util.Ops(Int64.MinValue, (Int64)5));
        Console.WriteLine("Int64 " + Util.Ops((Int64)(-37), (Int64)11));
        Console.WriteLine("Int64 " + Util.Ops((Int64)37, (Int64)(-11)));
        Console.WriteLine("Int64 " + Util.Ops((Int64)(-9223372036854775807), (Int64)(-1)));
        Console.WriteLine("Int64 " + Util.Ops((Int64)0, (Int64)7));
        Console.WriteLine("Int64 " + Util.Ops((Int64)(-1), (Int64)(-1)));
        Tuple<Int64, Int64> Int64_dr = Int64.DivRem(Int64.MaxValue, (Int64)3);
        Console.WriteLine("Int64 divrem " + Int64_dr.Item1 + " " + Int64_dr.Item2);
        Console.WriteLine("UInt64 " + Util.Constants<UInt64>());
        Console.WriteLine("UInt64 " + Util.Unsigned<UInt64>());
        Console.WriteLine("UInt64 " + Util.Ops(UInt64.MaxValue, (UInt64)3));
        Console.WriteLine("UInt64 " + Util.Ops((UInt64)9223372036854775807, (UInt64)5));
        Console.WriteLine("UInt64 " + Util.Ops((UInt64)37, (UInt64)11));
        Console.WriteLine("UInt64 " + Util.Ops((UInt64)11, (UInt64)37));
        Console.WriteLine("UInt64 " + Util.Ops((UInt64)1, UInt64.MaxValue));
        Console.WriteLine("UInt64 " + Util.Ops(UInt64.MinValue, (UInt64)7));
        Tuple<UInt64, UInt64> UInt64_dr = UInt64.DivRem(UInt64.MaxValue, (UInt64)3);
        Console.WriteLine("UInt64 divrem " + UInt64_dr.Item1 + " " + UInt64_dr.Item2);
    }
}
