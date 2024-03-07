#include <stdio.h>
#include <math.h>

typedef struct Points
{
    float x;
    float y;
} Points;

void printValues(Points *, int , int);
int farthestPointFromCenter(Points *, Points, int);

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
    Points points[number_of_points];
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
    Points center_point = {0.0f};
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
    Points poly_verticies[number_of_poly_vertices];
    double degree =  (2 * M_PI) / number_of_poly_vertices;
    // radius = radius_of_farthest_point + shift (to place the points a bit farther from the radius of the farthest point)
    double radius = longest_distance_between_points + 5.0; 
    for(int i = 0; i < number_of_poly_vertices; i++){
        poly_verticies[i].x = (float)(radius * cos(degree*(i+1)) + center_point.x);
        poly_verticies[i].y = (float)(radius * sin(degree*(i+1)) + center_point.y);
    }
    // Printing the generated coordinates of the polygon's verticies for checkup
    printf("\nA pontok köré írt poligon csúcspontjainak koordinátái:\n");
    for(int i = 0; i < number_of_poly_vertices; i++){
        printf("%d. csúcs = x: %f, y: %f\n", i, poly_verticies[i].x, poly_verticies[i].y);
    }

    return 0;
}
