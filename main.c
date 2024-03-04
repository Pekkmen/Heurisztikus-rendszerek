#include <stdio.h>

typedef struct Points
{
    int x;
    int y;
} Points;


int main(){
    // Opening the data file in "read" mode
    FILE *data_file = fopen("data.txt", "r");
    if(data_file == NULL){
        perror("File couldn't be opened! Problem");
        return 1;
    }
    
    int number_of_points = 0, number_of_poly_sides = 0;
    // Reading data from the file while checking for errors and not legal values
    if(fscanf(data_file, "N = %d, K = %d", &number_of_points, &number_of_poly_sides) == EOF){
        fprintf(stderr, "Data couldn't be read!\n");
        fclose(data_file);
        return 1;
    }
    if(number_of_points == 0 || number_of_poly_sides == 0){
        fprintf(stderr, "Data can not be 0! Check the 'data.txt' file for correct formatting or initialize values with greater numbers than 0!\n");
        fprintf(stderr, "N = %d\nK = %d\n", number_of_points, number_of_poly_sides);
        fclose(data_file);
        return 1;
    }
    // Reading the coordinates of the points
    Points points[number_of_points];
    for(int i = 0; i < number_of_points; i++){
        // Move the file position to the next line (it turns out fscanf reads until "\n" is found as expected, but it does NOT move beyond that point automatically) 
        CHECK FOR INVALID VALUES!
        fseek(data_file, 1, SEEK_CUR);
        fscanf(data_file, "x:%d, y:%d", &points[i].x, &points[i].y);
    }
    fclose(data_file);

    // Printing data for checking the read values (I should put this in a separate function maybe)
    printf("Körbeírandó pontok darabszáma = %d\nKörbeíró poligon fokszáma = %d\n", number_of_points, number_of_poly_sides);
    for(int i = 0; i < number_of_points; i++){
        printf("%d. pont -> x: %d, y: %d\n", i, points[i].x, points[i].y);
    }

    return 0;
}