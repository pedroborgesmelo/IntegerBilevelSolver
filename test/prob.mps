* ENCODING=ISO-8859-1
NAME          IloCplex
ROWS
 N  obj     
 G  c1      
 G  c2      
 G  c3      
COLUMNS
    MARK0000  'MARKER'                 'INTORG'
    x1        obj                             1
    x1        c1                              1
    x1        c3                             -2
    x2        obj                           0.5
    x2        c2                              1
    x2        c3                              1
    MARK0001  'MARKER'                 'INTEND'
RHS
    rhs       c1                            2.2
BOUNDS
 LI bnd       x1        0
 LI bnd       x2        0
ENDATA
