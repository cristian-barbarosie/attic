
import random, math

f = open ("random.eps","w")
f .write ("%!PS-Adobe-3.0 EPSF-3.0\n")
f .write ("%%BoundingBox: 0 0 600 400\n")
f .write ("30 30 translate\n")
f .write ("200 dup scale\n")
f .write ("0.005 setlinewidth\n")
f. write ("-0.1 0 moveto 2.5 0 lineto stroke\n")
f. write ("0 -0.1 moveto 0 1.6 lineto stroke\n")
f. write ("-0.02 1.57 moveto 0 1.6 lineto stroke\n")
f. write ("0.02 1.57 moveto 0 1.6 lineto stroke\n")
f. write ("2.47 0.02 moveto 2.5 0 lineto stroke\n")
f. write ("2.47 -0.02 moveto 2.5 0 lineto stroke\n")
f. write ("1 -0.01 moveto 1 0.01 lineto stroke\n")
f. write ("2 -0.01 moveto 2 0.01 lineto stroke\n")
f. write ("-0.01 1 moveto 0.01 1 lineto stroke\n")
f .write ("1 0 0 setrgbcolor\n")

x = random.random()
y = random.random()
for i in range ( 600 ):
   s = random.random()
   t = random.random()
   x = s
   y = t
   if (x-0.5)*(x-0.5) + (y-0.5)*(y-0.5) < 0.25 :
      f .write ( str(x) + ' ' + str(y) + ' 0.01 0 360 arc fill\n' )

f .write ("showpage\n")

