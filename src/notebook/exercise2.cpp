#include <iostream>
#include "../../dep/vecta.h"

using namespace std;
using namespace vecta;

typedef vecta::vec2d<float> v2;

int main ()
{
    v2 a, p, u;

    scanf("%f", &a.x); scanf("%f", &a.y);
    scanf("%f", &p.x); scanf("%f", &p.y);
    scanf("%f", &u.x); scanf("%f", &u.y);

    v2 ap = p - a;
    float eq = u * ap;

    if (eq != 0)
        printf ("P(%f, %f) is not on the line with point A(%f, %f) and vector u(%f, %f)!\n",
                p.x, p.y, a.x, a.y, u.x, u.y);
    else
        printf ("P(%f, %f) is ON the line with point A(%f, %f) and vector u(%f, %f)!\n",
                p.x, p.y, a.x, a.y, u.x, u.y);

    printf("unit 2\n");
    printf("colour green\n");
    printf("points\n");
    printf("%f %f\n", a.x, a.y);
    printf("%f %f\n", u.x, u.y);
    printf("colour yellow\n");
    printf("points\n");
    printf("%f %f\n", p.x, p.y);
    printf("colour red\n");
    printf("lines>\n");
    printf("%f %f %f %f\n", p.x, p.y, a.x, a.y);
    printf("colour blue\n");
    printf("lines>\n");
    printf("%f %f %f %f\n", a.x, a.y, u.x, u.y);

    return 0;
}
