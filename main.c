#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Point
{
    float x;
    float y;
} Point;

bool is_point_inside_poly(Point *, Point *, float, int, int);

int main(){
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

    if(is_point_inside_poly(&points[0], poly_verticies, max_x, number_of_points, number_of_poly_vertices)) printf("\nDEBUG: INSIDE\n");
    else printf("\nDEBUG: OUTSIDE\n");

    return 0;
}

bool is_point_inside_poly(Point *point_a, Point *poly_verticies, float max_x, int number_of_points, int number_of_poly_vertices){
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
        // if(i != number_of_poly_vertices -1){
        //     d_1 = (poly_verticies[i].x - point_a->x) * (point_b.y - point_a->y) - (poly_verticies[i].y - point_a->y) * (point_b.x - point_a->x);
        //     d_2 = (poly_verticies[i+1].x - point_a->x) * (point_b.y - point_a->y) - (poly_verticies[i+1].y - point_a->y) * (point_b.x - point_a->x);
        //     d_3 = (point_a->x - poly_verticies[i].x) * (poly_verticies[i+1].y - poly_verticies[i].y) - (point_a->y - poly_verticies[i].y) * (poly_verticies[i+1].x - poly_verticies[i].x);
        //     d_4 = (point_b.x - poly_verticies[i].x) * (poly_verticies[i+1].y - poly_verticies[i].y) - (point_b.y - poly_verticies[i].y) * (poly_verticies[i+1].x - poly_verticies[i].x);
        //     printf("d_1 = %f, d_2 = %f, d_3 = %f, d_4 = %f\n", d_1, d_2, d_3, d_4);
        // } else {
        //     d_1 = (poly_verticies[i].x - point_a->x) * (point_b.y - point_a->y) - (poly_verticies[i].y - point_a->y) * (point_b.x - point_a->x);
        //     d_2 = (poly_verticies[0].x - point_a->x) * (point_b.y - point_a->y) - (poly_verticies[0].y - point_a->y) * (point_b.x - point_a->x);
        //     d_3 = (point_a->x - poly_verticies[i].x) * (poly_verticies[0].y - poly_verticies[i].y) - (point_a->y - poly_verticies[i].y) * (poly_verticies[0].x - poly_verticies[i].x);
        //     d_4 = (point_b.x - poly_verticies[i].x) * (poly_verticies[i+1].y - poly_verticies[i].y) - (point_b.y - poly_verticies[i].y) * (poly_verticies[0].x - poly_verticies[i].x);
        //     printf("Utolso kor d_1 = %f, d_2 = %f, d_3 = %f, d_4 = %f\n", d_1, d_2, d_3, d_4);
        // }
        // if(((d_1 < 0 && d_2 > 0) || (d_1 > 0 && d_2 < 0)) && ((d_3 > 0 && d_4 < 0) || (d_3 < 0 && d_4 > 0))){
        //     counter++;
        //     printf("METSZES\n");
        // } 
        // https://www.youtube.com/watch?v=bvlIYX9cgls
        float alpha = -10000, beta = -10000;
        float x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4;
        if(i != number_of_poly_vertices - 1){
            x_1 = poly_verticies[i].x, y_1 = poly_verticies[i].y;
            x_2 = poly_verticies[i+1].x, y_2 = poly_verticies[i+1].y;
            x_3 = point_a->x, y_3 = point_a->y; 
            x_4 = point_b.x, y_4 = point_b.y;
            printf("DEBUG: poly_verticies[%d], poly_verticies[%d]\n", i, i+1);
        } else {
            x_1 = poly_verticies[i].x, y_1 = poly_verticies[i].y;
            x_2 = poly_verticies[0].x, y_2 = poly_verticies[0].y;
            x_3 = point_a->x, y_3 = point_a->y; 
            x_4 = point_b.x, y_4 = point_b.y;
            printf("DEBUG: poly_verticies[%d], poly_verticies[%d]\n", i, 0);
        }
            alpha = ((x_4 - x_3) * (y_3 - y_1) - (y_4 - y_3) * (x_3 - x_1)) / ((x_4 - x_3) * (y_2 - y_1) - (y_4 - y_3)*(x_2 - x_1));
            beta = ((x_2 - x_1) * (y_3 - y_1) - (y_2 - y_1) * (x_3-x_1)) / ((x_4 - x_3) * (y_2 - y_1) - (y_4 - y_3)*(x_2 - x_1));
            // printf("point_a : x=%f, y=%f\npoint_b : x=%f, y=%f\npolyv[%d] : x=%f, y=%f\npolyv[%d] : x=%f, y=%f\n", x_3, y_3, x_4, y_4, i, x_1, y_1, 0, x_2, y_2);
            printf("DEBUG: Alpha: %f, Beta: %f\n", alpha, beta);

        if(alpha == - 10000 || beta == -10000){
            fprintf(stderr, "Az alpha és béta értéke nem módosult a rajzolt vektor és a poligon éleinek metszeteinek keresésekor!");
            exit(1);
        } else if(0.0 <= alpha && alpha <= 1.0 && 0.0 <= beta && beta <= 1.0){
            // If alpha and beta is between the values of 0 and 1, the two lines intersect
            printf("DEBUG: intersect!\n");
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
    else if(counter % 2 == 0) return true;
    else return false;
}