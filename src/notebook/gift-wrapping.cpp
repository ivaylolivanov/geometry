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

bool IsMoreSuitablePoint(const Point2D& a, const Point2D& w)
{
    bool result = false;

    result = (a.x >= w.x) && (a.y >= w.y);

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
            if (IsMoreSuitablePoint(current_start_point, temp_point))
            {
                start_point_index = current_index;
                current_start_point = temp_point;
            }

            is_input_x = true;
        }
    }

    return start_point_index;
}

int GetNextSuitablePoint(PointsList* points, int visited[], int current_point_index)
{
    Point2D current_point = PointsListGetAt(points, current_point_index);

    return 0;
}

void PointsListPrint(PointsList* points, OutputFormat format, int start_index)
{
    if (format == OutputFormat_SP)
        SetUnit(1);

    for (int i = 0; i < points->Count; ++i)
    {
        Point2D point = PointsListGetAt(points, i);

        if (format == OutputFormat_None)
        {
            printf("(%f, %f)\n", point.x, point.y);
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
    int start_index = ReadStdin(&points);
    int visited[points.Count];

    // start_index = GetNextSuitablePoint(&points, visited, start_index);

    PointsListPrint(&points, cmd.Format, start_index);

    return 0;
}
