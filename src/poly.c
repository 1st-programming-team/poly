#include "poly.h"

poly make_poly(int a, int p)
{
	poly pt = (poly)malloc(sizeof(poly_node));
	pt->a = a; pt->p = p; pt->next = 0;
	return pt;
}

void add_poly(poly lhs, poly rhs)
{
	poly term = rhs;
	for (; term != 0; term = term->next) {
		if (term->a != 0) {
			poly p = lhs, plast = 0;
			for (; p != 0; plast = p, p = p->next) {
				if (p->p >= term->p) break;
			}
			if (p != 0 && p->p == term->p) {
				if ((p->a += term->a) == 0) {
					if (plast != 0) {
						plast->next = p->next;
						free(p);
					}
				}
			} else if (plast == 0) {
				if (lhs->a != 0) {
					poly pp = (poly)malloc(sizeof(poly_node));
					memcpy(pp, lhs, sizeof(poly_node));
					lhs->next = pp;
				}
				lhs->a = term->a; lhs->p = term->p;
			} else {
				poly pp = (poly)malloc(sizeof(poly_node));
				pp->a = term->a; pp->p = term->p; pp->next = p;
				plast->next = pp;
			}
		}
	}
}

void sub_poly(poly lhs, poly rhs)
{
	poly term = rhs;
	for (; term != 0; term = term->next) {
		if (term->a != 0) {
			poly p = lhs, plast = 0;
			for (; p != 0; plast = p, p = p->next) {
				if (p->p >= term->p) break;
			}
			if (p != 0 && p->p == term->p) {
				if ((p->a -= term->a) == 0) {
					if (plast != 0) {
						plast->next = p->next;
						free(p);
					}
				}
			} else if (plast == 0) {
				if (lhs->a != 0) {
					poly pp = (poly)malloc(sizeof(poly_node));
					memcpy(pp, lhs, sizeof(poly_node));
					lhs->next = pp;
				}
				lhs->a = -term->a; lhs->p = term->p;
			} else {
				poly pp = (poly)malloc(sizeof(poly_node));
				pp->a = -term->a; pp->p = term->p; pp->next = p;
				plast->next = pp;
			}
		}
	}
}

void mul_poly(poly lhs, poly rhs)
{
    if (lhs) {
        poly src = copy_poly(lhs);
        {
            poly ptr, pp;
            for (ptr = lhs->next; ptr != 0; ptr = pp) {
                pp = ptr->next; free(ptr);
            }
            memset(lhs, 0, sizeof(poly_node));
        }
        for (; rhs != 0; rhs = rhs->next) {
            if (rhs->a != 0) {
                poly temp = copy_poly(src), ptr = temp;
                for (; ptr != 0; ptr = ptr->next) {
                    ptr->a *= rhs->a; ptr->p += rhs->p;
                }
                add_poly(lhs, temp);
                dispose_poly(temp);
            }
        }
        dispose_poly(src);
    }
}

poly copy_poly(poly p)
{
    poly ret = 0, ret_ptr = ret;
    for (; p != 0; p = p->next) {
        if (!ret) {
            ret_ptr = ret = (poly)malloc(sizeof(poly_node));
            memcpy(ret_ptr, p, sizeof(poly_node));
        } else {
            ret_ptr->next = (poly)malloc(sizeof(poly_node));
            memcpy(ret_ptr->next, p, sizeof(poly_node));
            ret_ptr = ret_ptr->next;
        }
    }
    return ret;
}

void dispose_poly(poly p)
{
	poly pp;
	for (; p != 0; p = pp) {
		pp = p->next; free(p);
	}
}

void reverse_poly(poly p)
{
	for (; p != 0; p = p->next) {
		p->a = -p->a;
	}
}

void put_poly(poly p)
{
	int first = 0;
	for (; p != 0; p = p->next) {
		if (p->a != 0) {
			if (!first) {
				first = 1;
			} else {
				if (p->a > 0) printf("+");
			}
			switch (p->p) {
				case 0: {
					printf("%d", p->a);
				} break;
				case 1: {
					printf(p->a == 1 ? "x" : p->a == -1 ? "-x" : "%dx", p->a);
				} break;
				default: {
					printf(p->a == 1 ? "x^%*d" : p->a == -1 ? "-x^%*d" : "%dx^%d", p->a, p->p);
				} break;
			}
		}
	}
	puts(first ? "" : "0");
}
