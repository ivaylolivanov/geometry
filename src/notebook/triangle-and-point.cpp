#include <cmath>
#include <cstdio>
#include <iostream>
#include "../../dep/vecta.h"
#include "../platform/cmd-parser.h"
#include "../sp-parser.h"
#include "types.h"

using namespace std;

/*Task 1
  Take from the input 4 points, A, B, C and P. There is a triangle
  (ABC) that is formed of A, B and C. Determine:

  * DONE is ABC a valid triangle - Collinearity, vector lengths, sums
  * DONE Is P = A
  * DONE Is P = B
  * DONE Is P = C
  * DONE Is P on the line AB - Collinearity checks
  * DONE Is P on the line BC - Collinearity checks
  * DONE Is P on the line CA - Collinearity checks
  * DONE Is P an inner Point for the triangle ABC - Crossproducts' signs
  * DONE Is P an outer Point for the triangle ABC - Method of excluding
 */

float Min(float a, float b)
{
    float result = a > b ? b : a;
    return result;
}

float Max(float a, float b)
{
    float result = a > b ? a : b;
    return result;
}

void ReadPoint(Point2D* p, bool stdout)
{
    if (stdout)
        printf("New point:\n");

    scanf("%f", &p->x);
    scanf("%f", &p->y);
}

bool ArePointsCollinear(Point2D p1, Point2D p2, Point2D p3)
{
    bool result = false;

    V2r p1p2 = p2 - p1;
    V2r p1p3 = p3 - p1;
    float cross_product = p1p2 ^ p1p3;
    result = cross_product == 0;

    return result;
}

bool IsPointOnSegment(Point2D p, Point2D segment_a, Point2D segment_b)
{
    bool result = false;

    bool are_collinear = ArePointsCollinear(p, segment_a, segment_b);
    result = are_collinear
        && (p.x > Min(segment_a.x, segment_b.x))
        && (p.x < Max(segment_a.x, segment_b.x))
        && (p.y > Min(segment_a.y, segment_b.y))
        && (p.y < Max(segment_a.y, segment_b.y));

    return result;
}

bool IsTriangleInvalid(Point2D a, Point2D b, Point2D c)
{
    bool result = false;
    V2r ab = b - a;
    V2r bc = c - b;
    V2r ca = a - c;

    float cross_product = ab ^ ca;
    float side_ab = vecta::len(ab);
    float side_bc = vecta::len(bc);
    float side_ca = vecta::len(ca);

    result = (cross_product == 0)
        || ((side_ab == 0) || (side_bc == 0) || (side_ca == 0))
        || (((side_ab + side_bc) <= side_ca)
            || ((side_ca + side_ab) <= side_bc)
            || ((side_bc + side_ca) <= side_ab));

    return result;
}

int Sign(int n)
{
    int int_bits = sizeof(int) * 8 - 1;
    int sign_bit = (n >> int_bits);

    bool is_negative = sign_bit & 1;
    bool is_non_zero = !!n;
    int sign = is_non_zero * (1 - 2 * is_negative);

    return sign;
}

int Abs(int n)
{
    int int_bits = sizeof(int) * 8 - 1;
    int sign_bit = (n >> int_bits);

    int result = n ^ sign_bit - sign_bit;

    return result;
}

bool IsPointInnerForTriangle(Point2D p, Point2D a, Point2D b, Point2D c)
{
    bool result = false;

    V2r ab = b - a;
    V2r bc = c - b;
    V2r ca = a - c;
    V2r ap = p - a;
    V2r bp = p - b;
    V2r cp = p - c;

    int ab_x_ap = ab * ap;
    int bc_x_bp = bc * bp;
    int ca_x_cp = ca * cp;

    int sign = Sign(ab_x_ap) + Sign(bc_x_bp) + Sign(ca_x_cp);
    result = Abs(sign) == 3;

    return result;
}


int main(int arguments_count, char** arguments)
{
    CommandLine cmd = {};
    ParseCmd(arguments_count, arguments, &cmd);
    if (cmd.Help)
    {
        printf("%s", cmd.HelpMessage);
        return 0;
    }

    Point2D a, b, c, p;
    ReadPoint(&a, cmd.Format == OutputFormat_None);
    ReadPoint(&b, cmd.Format == OutputFormat_None);
    ReadPoint(&c, cmd.Format == OutputFormat_None);
    ReadPoint(&p, cmd.Format == OutputFormat_None);

    if (IsTriangleInvalid(a, b, c))
    {
        printf(
            "\nERROR: The points A(%.2f,%.2f), B(%.2f,%.2f) and C(%.2f,%.2f) %s!\n\n",
            a.x, a.y,
            b.x, b.y,
            c.x, c.y,
            "are not forming a valid triangle");

        return 1;
    }

    if (a == p)
    {
        if (cmd.Format == OutputFormat_None)
            printf("A(%.2f, %.2f) and P(%.2f, %.2f) are the same!\n",
                   a.x, a.y, p.x, p.y);
        return 0;
    }

    if (b == p)
    {
        if (cmd.Format == OutputFormat_None)
            printf("B(%.2f, %.2f) and P(%.2f, %.2f) are the same!\n",
                   b.x, b.y, p.x, p.y);

        return 0;
    }

    if (c == p)
    {
        if (cmd.Format == OutputFormat_None)
            printf("C(%.2f, %.2f) and P(%.2f, %.2f) are the same!\n",
                   c.x, c.y, p.x, p.y);

        return 0;
    }

    if (IsPointOnSegment(p, a, b))
    {
        if (cmd.Format == OutputFormat_None)
            printf("P(%.2f, %.2f) %s A(%.2f, %.2f) and B(%.2f, %.2f)\n!",
                   p.x, p.y, "lies on the line, passing through", a.x, a.y,
                   b.x, b.y);
        return 0;
    }

    if (IsPointOnSegment(p, b, c))
    {
        if (cmd.Format == OutputFormat_None)
            printf("P(%.2f, %.2f) %s A(%.2f, %.2f) and B(%.2f, %.2f)\n!",
                   p.x, p.y, "lies on the line, passing through", b.x, b.y,
                   c.x, c.y);
        return 0;
    }

    if (IsPointOnSegment(p, c, a))
    {
        if (cmd.Format == OutputFormat_None)
            printf("P(%.2f, %.2f) %s C(%.2f, %.2f) and A(%.2f, %.2f)\n!",
                   p.x, p.y, "lies on the line, passing through", c.x, c.y,
                   a.x, a.y);
        return 0;
    }

    if (IsPointInnerForTriangle(p, a, b, c))
    {
        if (cmd.Format == OutputFormat_None)
            printf("P(%.2f, %.2f) is an inner point for triangle ABC!\n",
                   p.x, p.y);
    }
    else
    {
        if (cmd.Format == OutputFormat_None)
            printf("P(%.2f, %.2f) is an outer point for triangle ABC!\n",
                   p.x, p.y);
    }

    if (cmd.Format == OutputFormat_SP)
    {
        SetUnit(2);

        PrintPoint(a, SpColor_Black);
        PrintPoint(b, SpColor_Black);
        PrintPoint(c, SpColor_Black);
        PrintPoint(p, SpColor_Orange);

        PrintLine(a, b, SpColor_Green);
        PrintLine(b, c, SpColor_Green);
        PrintLine(c, a, SpColor_Green);
    }

    return 0;
}
