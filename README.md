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