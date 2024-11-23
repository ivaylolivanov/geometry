#include <cmath>
#include <cstdio>
#include <iostream>
#include "../../dep/vecta.h"
#include "../platform/cmd-parser.h"
#include "../sp-parser.h"
#include "types.h"

/*
  Midterm - Task 2
  Input: A, B, C, D; Not always different!
  Find the non-oriented area of the polygon, formed by A, B, C, D, A

  Ideas(Guidelines)
  Areas and their signs; [DAB], [ABC], [BCD], [CDA].

  Approach:
    - Check for point duplicates
    - Formula for the doubled area of a polygon [ABCD] = AB x CD
    - Corner case, ABCD might be a self intersecting one
      - Self intersecting polygon if:
        - Diagonals are not intersecting
        AND
        - Diagonals are outer for the polygon
        - Then use the triangles and find their area
    - For a convex and concave polygon, use the formula:
    [ABCD] = 1/2 (AC x BD)
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

bool GetIntersection(Point2D a, Point2D b, Point2D c, Point2D d,
    Point2D* intersection)
{
    V2r ab = b - a;
    V2r cd = d - c;
    V2r ca = c - a;

    // Calculate determinant
    float det = ab.x * (-cd.y) - ab.y * (-cd.x);

    // If determinant is close to zero, the segments are parallel or collinear
    if (fabs(det) <= 0.001f)
        return false;

    // Calculate t and u parameters
    float t = (ca.x * (-cd.y) - ca.y * (-cd.x)) / det;
    float u = (ab.x * (ca.y) - ab.y * (ca.x)) / det;

    // Check if the intersection is within the segments
    if (t < 0 || t > 1 || u < 0 || u > 1)
        return false;

    // Compute the intersection point
    *(intersection) = a + t * ab;
    return true;
}

void PrintSpFormat(Point2D &a, Point2D &b, Point2D &c, Point2D &d)
{
    SetUnit(2);
    PrintPoint(a, SpColor_Orange);
    PrintPoint(b, SpColor_Orange);
    PrintPoint(c, SpColor_Orange);
    PrintPoint(d, SpColor_Orange);

    PrintLine(a, b, SpColor_Gray);
    PrintLine(b, c, SpColor_Gray);
    PrintLine(c, d, SpColor_Gray);
    PrintLine(d, a, SpColor_Gray);
}

float CalculateSelfIntersectingPolygon(Point2D &a, Point2D &b, Point2D &c, Point2D &d)
{
    float area = 0;

    Point2D i = {};
    bool are_intersecting = GetIntersection(a, b, c, d, &i);
    if (are_intersecting)
    {
        float adi = fabs(0.5f * ((d - a) ^ (i - d)));
        float ibc = fabs(0.5f * ((b - i) ^ (c - b)));
        area = adi + ibc;
    }

    return area;
}

int main(int arguments_count, char **arguments)
{
    CommandLine cmd = {};
    ParseCmd(arguments_count, arguments, &cmd);
    if (cmd.Help)
    {
        printf("%s", cmd.HelpMessage);
        return 0;
    }

    Point2D a, b, c, d;
    ReadPoint(&a);
    ReadPoint(&b);
    ReadPoint(&c);
    ReadPoint(&d);

    V2r ac = c - a;
    V2r ad = d - a;
    float acd = ac ^ ad;

    V2r cb = b - c;
    float acb = ac ^ cb;

    V2r bd = d - b;
    V2r ba = a - b;
    float bda = bd ^ ba;

    V2r dc = c - d;
    float bdc = bd ^ dc;

    bool ac_crosses_bd = (((acd > 0) && (acb < 0)) || ((acd < 0) && (acb > 0)))
        && (((bda > 0) && (bdc < 0)) || ((bda < 0) && (bdc > 0)));

    float area = 0;
    if (ac_crosses_bd)
        area = 0.5f * (ac ^ bd);
    else
        area = CalculateSelfIntersectingPolygon(a, b, c, d);

    if (cmd.Format == OutputFormat_SP)
        PrintSpFormat(a, b, c, d);
    else
        printf("%f\n", area);

    return 0;
}
