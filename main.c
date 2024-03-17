#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef struct Point{
    float x;
    float y;
} Point;

bool is_point_inside_poly(Point *, Point *, float, int);
void hill_climbing_steepest_asc(Point *, Point *, int, int, float, float, Point *, float);
Point find_min(Point *, float, int, Point *);
float perimeter_of_polygon(Point *, int);

int main(){
    srand(time(NULL));
    // Opening the data file in "read" mode
    FILE *data_file = fopen("data.txt", "r");
    if(data_file == NULL){
        perror("\nFile couldn't be opened! Problem");
        return 1;
    }
    
    int number_of_points = 0, number_of_poly_vertices = 0;
    // Reading data from the file while checking for errors and not legal values
    if(fscanf(data_file, "N = %d, K = %d", &number_of_points, &number_of_poly_vertices) == EOF){
        fprintf(stderr, "\nData couldn't be read!\n");
        fclose(data_file);
        return 1;
    }
    if(number_of_points == 0 || number_of_poly_vertices < 3){
        fprintf(stderr, "\nNumber of points can not be 0 OR the number of polygon verticies can not be less than 3! Initialize these values with greater numbers or check the 'data.txt' file for correct formatting!\n");
        fprintf(stderr, "N = %d\nK = %d\n", number_of_points, number_of_poly_vertices);
        fclose(data_file);
        return 1;
    }

    // Reading the coordinates of the points
    fseek(data_file, 1, SEEK_CUR); // Before reading, move the file position to the next line, so the coordinates of the points can be separated by blank lines
    Point points[number_of_points];
    for(int i = 0; i < number_of_points; i++){
        // Move the file position to the next line. Note to self: it turns out fscanf reads until "\n" is found as expected, but it does NOT move beyond that point automatically
        fseek(data_file, 1, SEEK_CUR);
        fscanf(data_file, "x:%f, y:%f", &points[i].x, &points[i].y);
        // Checking for negative values for the coordinates of the points
        if(points[i].x < 0 || points[i].y < 0){
            fprintf(stderr, "\nThe coordinates can not be negative numbers! Error with point %d\n", i);
            return 1;
        }
    }
    fclose(data_file);

    // Printing data to check the values read from the text file
    printf("\nKEZDŐÉRTÉKEK:\nKörbeírandó pontok darabszáma = %d\nKörbeíró poligon fokszáma = %d\n", number_of_points, number_of_poly_vertices);
    for(int i = 0; i < number_of_points; i++){
        printf("%d. pont -> x: %f, y: %f\n", i, points[i].x, points[i].y);
    }

    // Calculating the center point of the area the points encircle
    Point center_point = {0.0f};
    for(int i = 0; i < number_of_points; i++){
        center_point.x += points[i].x;
        center_point.y += points[i].y;
    }
    center_point.x = center_point.x / number_of_points;
    center_point.y = center_point.y / number_of_points;
    // Printing the coordinates of the center points for checkup
    printf("\nA pontok által közrezárt terület középpontja: x: %f, y: %f\n", center_point.x, center_point.y);

    // Searching for the point farthest from the center point
    int farthest_index = -1;
    double longest_distance_between_points = 0.0f;
    for(int i = 0; i < number_of_points; i++){
        // Calculating distance between points (sqrt((x2 - x1)^2 + (y2 - y1)^2))
        double distance_between_points = sqrt(pow(center_point.x - points[i].x, 2) + pow(center_point.y - points[i].y, 2));
        // If the checked distance is greater than the current one, update the longest distance accordingly
        if(longest_distance_between_points < distance_between_points){
            longest_distance_between_points = distance_between_points;
            farthest_index = i;
        }
    }
    // Checking if the farthest_index was update or not
    if(farthest_index < 0) {
        fprintf(stderr, "The farthest_index variable was not updated! Something went wrong!");
        return 1;
    }
    // Printing the data of the farthest point with the distance from the center point for checkup
    printf("Legtávolabbi pont indexe: %d az alábbi koordinátákkal: x: %f, y: %f. A pont távolsága a középponttól: %lf\n", farthest_index, points[farthest_index].x, points[farthest_index].y, longest_distance_between_points);

    // Generating the points of the polygon (placed on a circle around the center point)
    // x = r * cos(degree) + x_center;
    // y = r * sin(degree) + y_center;
    Point poly_verticies[number_of_poly_vertices];
    double degree =  (2 * M_PI) / number_of_poly_vertices;
    // radius = radius_of_farthest_point + shift (to place the points a bit farther from the radius of the farthest point)
    float max_x = 0.0f;
    double radius = longest_distance_between_points + 5.0; 
    for(int i = 0; i < number_of_poly_vertices; i++){
        poly_verticies[i].x = (float)(radius * cos(degree*(i+1)) + center_point.x);
        poly_verticies[i].y = (float)(radius * sin(degree*(i+1)) + center_point.y);

        // Saving the farthest X coodinate. These will be needed to draw a long vector from the points we read from the data.txt file
        if(max_x < poly_verticies[i].x) max_x = poly_verticies[i].x;
    }
    // Printing the generated coordinates of the polygon's verticies for checkup
    printf("\nA pontok köré írt poligon csúcspontjainak kezdő koordinátái:\n");
    for(int i = 0; i < number_of_poly_vertices; i++){
        printf("%d. csúcs = x: %f, y: %f\n", i, poly_verticies[i].x, poly_verticies[i].y);
    }

    if(is_point_inside_poly(&points[0], poly_verticies, max_x, number_of_poly_vertices)) printf("\nDEBUG: INSIDE\n");
    else printf("\nDEBUG: OUTSIDE\n");

    // The hill climbing algorithm with the steepest ascent implementation
    float distance = 0.5;
    float stop_threshold = 0.05;
    // The radius of the area around the vertex point of the poly
    float search_area_radius = 1.0f;
    hill_climbing_steepest_asc(points, poly_verticies, number_of_points, number_of_poly_vertices, distance, stop_threshold, &center_point, search_area_radius);

    return 0;
}

bool is_point_inside_poly(Point *point_a, Point *poly_verticies, float max_x, int number_of_poly_vertices){
    // The ray casting algorithm is used for checking whether a point is inside the polygon (https://en.wikipedia.org/wiki/Point_in_polygon)
    // First, we create a long vector: point_a will be the point we are checking to see if it's inside the polygon; point_b will be the end of this vector, drawn beyond the farthest x point, ensuring it will definitely intersect one of the polygon's edges at least once
    Point point_b = (Point){
        .x = point_a->x + 2*max_x,
        .y = point_a->y
    };
    // Count how many times the drawn vector including the inspected point intersects the edges of the polygon
    int counter = -1;
    // Check if the drawn vector containing the inspected point intersects any edge of the polygon (https://algorithmtutor.com/Computational-Geometry/Check-if-two-line-segment-intersect/)
    float d_1 = 0.0f, d_2 = 0.0f, d_3 = 0.0f, d_4 = 0.0f;
    for(int i = 0; i < number_of_poly_vertices; i++){
        // https://www.youtube.com/watch?v=bvlIYX9cgls
        float alpha = -10000, beta = -10000;
        float x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4;
        if(i != number_of_poly_vertices - 1){
            x_1 = poly_verticies[i].x, y_1 = poly_verticies[i].y;
            x_2 = poly_verticies[i+1].x, y_2 = poly_verticies[i+1].y;
            x_3 = point_a->x, y_3 = point_a->y; 
            x_4 = point_b.x, y_4 = point_b.y;
            //printf("DEBUG: poly_verticies[%d], poly_verticies[%d]\n", i, i+1);
        } else {
            x_1 = poly_verticies[i].x, y_1 = poly_verticies[i].y;
            x_2 = poly_verticies[0].x, y_2 = poly_verticies[0].y;
            x_3 = point_a->x, y_3 = point_a->y; 
            x_4 = point_b.x, y_4 = point_b.y;
            //printf("DEBUG: poly_verticies[%d], poly_verticies[%d]\n", i, 0);
        }
            alpha = ((x_4 - x_3) * (y_3 - y_1) - (y_4 - y_3) * (x_3 - x_1)) / ((x_4 - x_3) * (y_2 - y_1) - (y_4 - y_3)*(x_2 - x_1));
            beta = ((x_2 - x_1) * (y_3 - y_1) - (y_2 - y_1) * (x_3-x_1)) / ((x_4 - x_3) * (y_2 - y_1) - (y_4 - y_3)*(x_2 - x_1));
            // printf("point_a : x=%f, y=%f\npoint_b : x=%f, y=%f\npolyv[%d] : x=%f, y=%f\npolyv[%d] : x=%f, y=%f\n", x_3, y_3, x_4, y_4, i, x_1, y_1, 0, x_2, y_2);
            //printf("DEBUG: Alpha: %f, Beta: %f\n", alpha, beta);

        if(alpha == - 10000 || beta == -10000){
            fprintf(stderr, "Az alpha és béta értéke nem módosult a rajzolt vektor és a poligon éleinek metszeteinek keresésekor!");
            exit(1);
        } else if(0.0 <= alpha && alpha <= 1.0 && 0.0 <= beta && beta <= 1.0){
            // If alpha and beta is between the values of 0 and 1, the two lines intersect
            //printf("DEBUG: intersect!\n");
            counter++;
        }

    }
    // Checking for errors
    if(counter == -1){
        fprintf(stderr, "\nHiba történt a húzott vektor és a poligon egyes oldalainak metszéseinek vizsgálatakor!\n");
        exit(1);
    }
    // If the point is on the outside of the polygon the vector will intersect its edge an odd number of times (BECAUSE OUR POINTS START INSIDE THE POLYGON!)
    // The counter starts at -1, and odd + odd = even
    else if(counter % 2 == 0) return true; // inside
    else return false; // outside
}

float perimeter_of_polygon(Point *poly_vertices, int number_of_poly_vertices){
    float perimeter = 0.0f;
    float a_x, a_y, b_x, b_y;
    for(int i = 0; i < number_of_poly_vertices - 1; i++){
        a_x = poly_vertices[i].x;
        a_y = poly_vertices[i].y;
        b_x = poly_vertices[i+1].x;
        b_y = poly_vertices[i+1].y;
        // Calculate the distance between the vertices of the poly and sum them to the perimeter
        perimeter += sqrt(pow(b_x - a_x, 2) + pow(b_y - a_y, 2));
    }
    // Calculate the distance between the "last two" vertices manually
    a_x = poly_vertices[number_of_poly_vertices-1].x;
    a_y = poly_vertices[number_of_poly_vertices-1].y;
    b_x = poly_vertices[0].x;
    b_y = poly_vertices[0].y;
    perimeter += sqrt(pow(b_x - a_x, 2) + pow(b_y - a_y, 2));

    return perimeter;
}

// Point find_min(Point *inspected_point, float radius, int sector, Point *center_point){
//     // This function returns the point within a radius of the inspected point that is closer to the center point
//     // The sector defines how many pieces the circle should be sliced into to check out each possible candidate solution
//     double degree =  (2 * M_PI) / sector;
//     // The min will store the point closer to the center point, while the relevant store the inspected candidate solution
//     Point min;
//     // Draw a circle sector around the point (each new point will be a candidate solution)
//     for(int i = 0; i < sector; i++){
//         float x = (float)(radius * cos(degree*(i+1)) + center_point->x);
//         float y = (float)(radius * sin(degree*(i+1)) + center_point->y);
//         // Initialize the closest point to the center with the first candidate solution
//         if(i == 0) min = (Point){
//             .x = x,
//             .y = y
//         };

//         // If the new coordinates are closer to the center point, the new coordinates will be selected as the minimum point
//         if(abs(center_point->x - x) <= abs(center_point->x - min.x) && abs(center_point->y - y) <= abs(center_point->y - min.y)){
//             min.x = x;
//             min.y = y;
//         }
//     }
//     // Print values for checking
//     printf("A poligon vizsgált csúcsának koordinátája, x: %f, y: %f\nVizsgált sugár: %f\nA kör szeleteinek darabszáma %d\nA pontok középpontjának koordinátája, x: %f, y: %f\n", inspected_point->x, inspected_point->y, radius, sector, center_point->x, center_point->y);
//     return min;
// }

Point find_min(Point *inspected_point, float radius, int sector, Point *center_point) {
    Point min;
    // Initialize minimum distance to a very large value
    float min_distance = 100000.f; 

    double degree = (2 * M_PI) / sector;

    // Iterate through sectors
    for (int i = 0; i < sector; i++) {
        float x = inspected_point->x + radius * cos(degree * i);
        float y = inspected_point->y + radius * sin(degree * i);

        // Calculate distance to center point
        float distance_to_center = sqrt(pow(center_point->x - x, 2) + pow(center_point->y - y, 2));

        // If the distance is smaller than the minimum distance found so far, update min
        if (distance_to_center < min_distance) {
            min_distance = distance_to_center;
            min.x = x;
            min.y = y;
        }
    }

    return min;
}

// MODIFY IT FOR THE STEEPEST ASCENT IMPLEMENTATION
void hill_climbing_steepest_asc(Point *points, Point *poly_vertices, int number_of_points, int number_of_poly_vertices, float distance, float stop_threshold, Point *center_point, float radius){
    bool stuck = false;
    // Randomly select a vertex
    int index = rand () % number_of_poly_vertices, min_index;
    Point *inspected_point = &poly_vertices[index];

    // The algorithm will run until either stuck or this value is below the stop threshold
    float difference_between_perimeters = 2 * stop_threshold;
    // Counter ti be incremented each iteration
    int counter = 0;

    while(!stuck && stop_threshold < difference_between_perimeters){
        printf("\n%d. kör\n", counter++);
        Point min_vertex;
        // Save the current perimeter
        float perimeter_old = perimeter_of_polygon(poly_vertices, number_of_poly_vertices), perimeter_new = 0.0f, perimeter_min = -1.0f;
        // Print data regarding the inspected point
        printf("Poligon módosítás előtti kerülete: %f\n", perimeter_old);

        // The radius around the inspected point will be sliced up into <sector> number of parts
        int sector = 10;

        for(int poly_vertex = 0; poly_vertex < number_of_poly_vertices; poly_vertex++){
            printf("Vizsgált poligon csúcs: %d, x: %f, y: %f\n", index, inspected_point->x, inspected_point->y);
            // Save the old position in case the new position of the vertex is illegal
            Point old_pos = *inspected_point;
            // Find the min neighbour of the inspected point
            *inspected_point = find_min(inspected_point, radius, sector, center_point);

            // Check if the new poly vertex position is legal
            for(int i = 0; i < number_of_points; i++){
                if(!is_point_inside_poly(&points[i], poly_vertices, 100.0f, number_of_poly_vertices) /*|| perimeter_old < perimeter_new*/){
                    // If a point is outside of the polygon, revert the modified point's coordinates
                    *inspected_point = old_pos;
                    continue;
                }
            }

            // If the new position is legal, save the new perimeter
            perimeter_new = perimeter_of_polygon(poly_vertices, number_of_poly_vertices);

            // If the new perimeter is smaller than the know smallest perimeter, save the change (or if the new perimeter hasn't been initialize yet)
            if(perimeter_new < perimeter_min || perimeter_min < 0){
                perimeter_min = perimeter_new;
                min_vertex = *inspected_point;
                min_index = index;
            }

            if(index < number_of_poly_vertices - 1) index++;
            else index = 0;
            // Revert back the changes for the remaining tests
            *inspected_point = old_pos;
            // Check the next vertex
            inspected_point = &poly_vertices[index];
        }

        // Change the point at which the biggest upgrade could be made
        poly_vertices[min_index] = min_vertex;
        // Print its value
        printf("A poligon %d. csúcsának új koordinátája: x = %f, y = %f\n", min_index, inspected_point->x, inspected_point->y);
        // Check the next vertex
        inspected_point = &poly_vertices[index];

        printf("\nPoligon módosítás utáni kerülete: %f\n", perimeter_min);
        difference_between_perimeters = perimeter_old - perimeter_min;
    }
}
