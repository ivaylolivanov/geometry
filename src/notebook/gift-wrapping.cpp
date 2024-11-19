#include <cstddef>
#include <cstdlib>
#include <stdio.h>
#include "types.h"
#include "../../dep/vecta.h"
#include "../platform/cmd-parser.h"
#include "../sp-parser.h"

/* Task 3:
   Implementation of Jarvis' convex hull algorithm, aka. 'Gift wrapping'

   Steps:
     - TODO: Read from STDIN pair of real numbers until EOF
     - TODO: Mark the left-most and down-most point
     - TODO: Continuously get the most suitable point and store it until the
     next suitable point is the initial one
     - TODO: Create visual output using the sp format
 */

struct PointsList
{
    int Count;
    int Capacity;
    Point2D* Points;
    const int ResizeFactor = 4;
};

void PointsListFreeMemory(PointsList* list)
{
    free(list->Points);
    list->Points = NULL;
    list->Count = 0;
    list->Capacity = 0;
}

void PointsListResize(PointsList* list)
{
    list->Capacity *= list->ResizeFactor;
    if (!list->Capacity)
        list->Capacity = list->ResizeFactor;

    Point2D* points_reallocated = (Point2D*)realloc(list->Points, list->Capacity * sizeof(Point2D));
    if (!points_reallocated)
    {
        perror("Failed to resize PointsList!\n");
        exit(EXIT_FAILURE);
    }

    list->Points = points_reallocated;
}

void PointsListAdd(PointsList* list, const Point2D& point)
{
    if (list->Count >= list->Capacity)
        PointsListResize(list);

    list->Points[list->Count++] = point;
}

Point2D& PointsListGetAt(PointsList* list, int index)
{
    if ((index < 0) || (index >= list->Count))
        exit(EXIT_FAILURE);

    return list->Points[index];
}

bool PointsListContains(PointsList* list, const Point2D& point)
{
    bool result = false;

    for (int i = 0; i < list->Count; ++i)
    {
        result = (PointsListGetAt(list, i) == point);
        if (result)
            break;
    }

    return result;
}

bool IsMoreToTheBottom(const Point2D& a, const Point2D& w)
{
    bool result = false;

    result = a.y >= w.y;

    return result;
}

int ReadStdin(PointsList* all_points)
{
    int start_point_index = 0;

    Point2D current_start_point;
    Point2D temp_point;

    bool is_input_x = true;
    float input;
    while (scanf("%f", &input) != EOF)
    {
        // NOTE: Read Point.X
        if (is_input_x)
        {
            temp_point.x = input;
            is_input_x = false;
            continue;
        }
        // NOTE: Read Point.Y and put in the array
        else
        {
            temp_point.y = input;
            if (PointsListContains(all_points, temp_point))
            {
                is_input_x = true;
                continue;
            }

            PointsListAdd(all_points, temp_point);

            int current_index = all_points->Count - 1;
            if (IsMoreToTheBottom(current_start_point, temp_point))
            {
                start_point_index = current_index;
                current_start_point = temp_point;
            }

            is_input_x = true;
        }
    }

    return start_point_index;
}

int GetNextSuitablePoint(PointsList* points, int index_current)
{
    Point2D point_current = PointsListGetAt(points, index_current);
    int index_guess = (index_current + 1) % points->Count;
    Point2D point_guess = PointsListGetAt(points, index_guess);
    for (int i = 0; i < points->Count; ++i)
    {
        Point2D point_checking = PointsListGetAt(points, i);
        V2r v1 = point_guess - point_current;
        V2r v2 = point_checking - point_current;
        float v1_x_v2 = v1 ^ v2;

        bool is_better = (v1_x_v2 < 0)
            || ((v1_x_v2 == 0) && (vecta::len(v2) > vecta::len(v1)));
        if (is_better)
        {
            index_guess = i;
            point_guess = PointsListGetAt(points, index_guess);
        }
    }

    return index_guess;
}

void PointsListPrint(PointsList* points, int start_index, OutputFormat format)
{
    if (format == OutputFormat_SP)
        SetUnit(1);

    for (int i = 0; i < points->Count; ++i)
    {
        Point2D point = PointsListGetAt(points, i);

        if (format == OutputFormat_None)
        {
            printf("(%7.2f; %7.2f)\n", point.x, point.y);
        }
        else if (format == OutputFormat_SP)
        {
            SpColor color = SpColor_Black;
            if (i == start_index)
                color = SpColor_Yellow;

            PrintPoint(point, color);
        }
    }
}

void CreateHull(PointsList* points, int hull[], int index_start)
{
    int index_current = index_start;
    while (!hull[index_current])
    {
        int index_next = GetNextSuitablePoint(points, index_current);
        hull[index_current] = index_next;
        index_current = index_next;
    }
}

void PrintHull(PointsList* points, int hull[], int index_start,
    OutputFormat format)
{
    int index_traverse = index_start;
    do
    {
        Point2D a = PointsListGetAt(points, index_traverse);
        Point2D b = PointsListGetAt(points, hull[index_traverse]);
        index_traverse = hull[index_traverse];

        if (format == OutputFormat_None)
            printf("(%.2f; %.2f) -> (%.2f; %.2f)\n", a.x, a.y, b.x, b.y);
        else
            PrintLine(a, b, SpColor_BlueViolet);
    } while (index_start != index_traverse);
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

    PointsList points = {};
    int index_start = ReadStdin(&points);
    int hull[points.Count - 1];
    for (int i = 0; i < points.Count; ++i)
        hull[i] = 0;

    CreateHull(&points, hull, index_start);
    PointsListPrint(&points, index_start, cmd.Format);
    PrintHull(&points, hull, index_start, cmd.Format);

    PointsListFreeMemory(&points);

    return 0;
}
