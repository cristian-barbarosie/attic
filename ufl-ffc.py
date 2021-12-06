
# use of FFC + UFL

from ufl import FiniteElement, triangle, TestFunction, TrialFunction, dx, Index
from ffc import compile_form

element = FiniteElement("Lagrange", triangle, 1)

#  triangle = ABC
#  A ( coordinate_dofs[0], coordinate_dofs[1] )
#  B ( coordinate_dofs[2], coordinate_dofs[3] )
#  C ( coordinate_dofs[4], coordinate_dofs[5] )

v = TestFunction(element)
u = TrialFunction(element)
i = Index()

a = u.dx(1) * dx
form = compile_form( [ a ] )

f = open ("form.h",'w')
for l in form : f.write(l)
