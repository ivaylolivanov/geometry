#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "types.h"

typedef vecta::vec2d<float> V2r;
typedef vecta::vec3d<float> V3r;
typedef vecta::vec2d<float> Point2D;

void PrintPoint(Point2D a)
{
    printf("(%.2f; %.2f)\n", a.x, a.y);
}

void ReadPoint2D(Point2D* v)
{
    scanf("%f", &v->x);
    scanf("%f", &v->y);
}

Point2D* ReadPolygon(int count)
{
    Point2D* polygon = (Point2D*)malloc(sizeof(Point2D) * count);

    for (int i = 0; i < count; ++i)
        ReadPoint2D(&polygon[i]);

    return polygon;
}

Point2D* CalculateNormals(Point2D* polygon, int vertex_count)
{
    int normals_count = vertex_count + 1;
    Point2D* normals = (Point2D*)malloc(sizeof(Point2D) * normals_count);

    int normal_index = 0;
    for (int i = 0; i < vertex_count; ++i)
    {
        Point2D a = polygon[i];
        Point2D b = polygon[(i + 1) % (vertex_count - 1)];
        V2r ab = b - a;
        V2r normal = ~ab;
        normals[normal_index++] = normal;
    }

    return normals;
}

Point2D IntersectionPoint(Point2D a1, Point2D b1, Point2D a2, Point2D b2)
{
    V2r a1b1 = b1 - a1;
    V2r a2b2 = b2 - a2;
    V2r cross = (a1b1 ^ a2b2);

    Point2D intersection = ((a1 ^ a1b1) * a2b2 - (a2 ^ a2b2) * a1b1)
        / cross;

    return intersection;
}

int main()
{
    int length_concave = 0;
    scanf("%d", &length_concave);
    Point2D* concave = ReadPolygon(length_concave);
    V2r* concave_normals = CalculateNormals(concave, length_concave);

    int length_convex = 0;
    scanf("%d", &length_convex);
    Point2D* convex = ReadPolygon(length_convex);
    V2r* convex_normals = CalculateNormals(convex, length_convex);

    for (int i = 0; i < length_concave; ++i)
    {
        Point2D a1 = concave[i];
        Point2D b1 = concave[i];
        for (int j = 0; j < length_convex; ++j)
        {
            Point2D a2 = convex[j];
            Point2D b2 = convex[j];

            Point2D intersection = IntersectionPoint(a1, b1, a2, b2);
        }
        break;
    }

    free(concave_normals);
    free(convex_normals);
    free(concave);
    free(convex);

    return 0;
}
