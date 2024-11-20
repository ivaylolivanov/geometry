#include <cmath>
#include <cstdio>
#include <iostream>
#include "../../dep/vecta.h"
#include "../platform/cmd-parser.h"
#include "../sp-parser.h"
#include "types.h"

/*
  Midterm - Task 1
  Input: A, B, C, D, E
  Define a rectangle, based on:
    - 1 of the sides is on the line AB
    - The opposite of AB, goes through D
    - The rest of the sides are respectively through C and E, and are
    perpendicular to the first 2

    Approach:
    - Project E on the line AB in vertex M
    - Project C on the line AB in vertex N
    - Project D on the line ME in vertex P
    - Project D on the line NC in vertex q
*/

void ReadPoint(Point2D* p)
{
    scanf("%f", &p->x);
    scanf("%f", &p->y);
}

void PrintPoint(Point2D a, SpColor color, OutputFormat format)
{
    if (format == OutputFormat_None)
        printf("(%.2f; %.2f)\n", a.x, a.y);
    else if (format == OutputFormat_SP)
        PrintPoint(a, color);
}

V2r ProjectPoint2DToVector(Point2D point, Point2D vector_a, Point2D vector_b)
{
    V2r projection;

    V2r ab = vector_b - vector_a;
    V2r a_point = point - vector_a;

    projection = ((ab * a_point) / (ab * ab)) * ab;

    return projection;
}

void PrintRectangle(Point2D a, Point2D b, Point2D c, Point2D d,
    OutputFormat format)
{
    PrintPoint(a, SpColor_BlueViolet, format);
    PrintPoint(b, SpColor_BlueViolet, format);
    PrintPoint(c, SpColor_BlueViolet, format);
    PrintPoint(d, SpColor_BlueViolet, format);

    PrintLine(a, b, SpColor_Black);
    PrintLine(b, c, SpColor_Black);
    PrintLine(c, d, SpColor_Black);
    PrintLine(d, a, SpColor_Black);
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

    Point2D a, b, c, d, e;
    ReadPoint(&a);
    ReadPoint(&b);
    ReadPoint(&c);
    ReadPoint(&d);
    ReadPoint(&e);

    V2r e_projected_ab = ProjectPoint2DToVector(e, a, b);
    Point2D m = a + e_projected_ab;

    V2r c_projected_ab = ProjectPoint2DToVector(c, a, b);
    Point2D n = a + c_projected_ab;

    V2r d_projected_me = ProjectPoint2DToVector(d, m, e);
    Point2D p = m + d_projected_me;

    V2r d_projected_nc = ProjectPoint2DToVector(d, n, c);
    Point2D q = n + d_projected_nc;

    SetUnit(2);

    PrintPoint(a, SpColor_Gray, cmd.Format);
    PrintPoint(b, SpColor_Gray, cmd.Format);
    PrintPoint(c, SpColor_Gray, cmd.Format);
    PrintPoint(d, SpColor_Gray, cmd.Format);
    PrintPoint(e, SpColor_Gray, cmd.Format);

    PrintRectangle(m, n, q, p, cmd.Format);
    return 0;
}
