Polynomials annihilator.
	Attempt to find for given list of multi-variables polynomials f1, f2, ... another polynomial F which composed with them will reduct to 0. That means F(f1, f2, ...)=0.
This exemplifies general fact algebraic dependency between polynomials. For example for polynomials f1(x)=x*x, f2(x)=x another polynomial F(y,z)=y-z*z is annihilator.
Composition F(f1(x),g1(x)) reduct to 0. For two polynomials f1(x,y)=x, f2(x,y)=y there is no annihilator but it is maximal count for 2 variables polynomials to be independed.
Searching for annihilator uses simple greedy checking succesive exponents of polynomials ( eg. f1(x)^m * f2(x)^n ) to eliminates succesively occured powers of x.