a + b 
a - b
a * b 
a / b
-a

expr = expr1 expr_r

expr_r = + expr1 expr_r
expr_r = - expr1 expr_r
expr_r = e

expr1 = expr2 expr1_r

expr1_r = * expr2 expr1_r
expr1_r = / expr2 expr1_r
expr1_r = e

expr2 = (expr)
expr2 = -expr1
expr2 = poly

poly = [ terms ]

terms = term
terms = term + terms
terms = term - terms

term = number x ^ number

