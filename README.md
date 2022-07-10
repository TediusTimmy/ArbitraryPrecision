Arbitrary Precision
===================

I wrote this in college. Many of the algorithms are not the best, but it should have some educational value.

This is an Integer class that can handle arbitrarily large integers. It should handle numbers into the millions of digits. Upon this is build a decimal floating point class that should be able to handle computations up to 256 digits of precision (this limitation is due to magic numbers in the library routines). That's a far cry from the 17 you get from the hardware with IEEE-754. Read Description.txt for more information.

Yes, this is also hosted on BitBucket. I forgot the password to that account years ago.

Programs included:
* AltSlowCalc - An RPN calculator with decimal floating-point numbers. It is the "SlowCalc" that is mentioned in the files under GPL3. "SlowCalc" was a college project that had a less efficient decimal floating-point number implementation.
* DB12 - A crappy language interpreter, made from my compiler class project.
* Dice - A tool for generating the actual number of cases for building the probabilities with very large rolls of dice.
* Example - Another crappy language interpreter.
* FENZero4Float - A tool for finding the zeros of a one-dimensional functions. No, it doesn't use Newton's method (I've never successfully written code to do differentiation), so it is MUCH slower, but can find zeros that Newton's method cannot.
* IntCalc - An integer calculator that is only notable in that it implements Dijkstra's Shunting-Yard Algorithm, an algorithm I will never again implement because I understand recursive descent parsers now.

NOTE (3 Mar 2020) : The Integer code has failed to run on GCC for a while now (except with -O0), and I finally fixed it. The problem was that younger me decided that it was safe to call the destructor of an object as any other function. I was treating the destructor of BitField as a "clear" function. GCCs lifetime-dse optimization considers the object invalid after the destructor runs, and was optimizing out the NULL pointer stores. That is now fixed (by me not calling the destructor).  
After far more research than I should have done... The change to GCC that breaks my code was commit February 2015 (five years ago!) and appeared in GCC 4.9.3.

NOTE (3 Jul 2022) : I fixed an obscure bug in the division code that would sometimes result in a horribly wrong answer. It would help if I had actually read the chapter on doing the math before trying to implement it. Also, AddressSanitizer found a mismatched new [] / delete bug.
