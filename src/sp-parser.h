#include <stdio.h>
#include "../dep/vecta.h"
#include "./notebook/types.h"

// TODO: Optimize to not print the color each time

void SetUnit(float unit)
{
    printf("unit %f\n", unit);
}

void PrintPoint(Point2D a, char* color)
{
    printf("colour %s\n", color);
    printf("points\n");
    printf("%f %f\n", a.x, a.y);
}

void PrintLine(Point2D a, Point2D b, char* color, float width = 1)
{
    printf("width %f\n", width);
    printf("lines\n");
    printf("%f %f %f %f\n", a.x, a.y, b.x, b.y);
}
