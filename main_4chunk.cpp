
//
// Created by chris on 10/28/2016.
//


#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <pthread.h>
#include <chrono>
#include <vector>
#include <fstream>
#include <string>


using namespace::std;
using namespace::chrono;

const int matrix_size = 512;
const int num_of_threads = 8;
int input_matrix[matrix_size][matrix_size];
int input_vector[matrix_size];
int results_vector[matrix_size];
int thread_bounds[num_of_threads][2];
long total_elapsed = 0;
ofstream myfile(to_string(num_of_threads) + "_threads_" + to_string(matrix_size) + "_matrix.csv");
pthread_mutex_t mut;


struct position_struct {
    int position;
};

void* multiply_row(void* args){
    position_struct* curr_pos = (position_struct*)args;
    int pos = curr_pos->position;
    //cout << pos << endl;
    //cout << "Currently doing mult for " << thread_bounds[pos][0] << "," << thread_bounds[pos][1] << endl;
    for(int row_position = thread_bounds[pos][0]; row_position < thread_bounds[pos][1]; row_position++){
        int new_row = 0;
        for (int ind_value = 0; ind_value < matrix_size; ind_value++){
            new_row = new_row + (input_matrix[row_position][ind_value] * input_vector[ind_value]);
        }
        results_vector[row_position] = new_row;
    }
    //cout << "Finished doing work for  " << thread_bounds[pos][0] << thread_bounds[pos][1] <<endl;
}



long* Two_d_matrixgen(){
    srand (time(NULL));
    for (int x = 0; x < matrix_size; x++){
        for (int y = 0; y < matrix_size; y++){
            input_matrix[x][y] = rand() % 1024 + 1;
        }
    }
    for (int x = 0; x < matrix_size; x++){
        input_vector[x] = rand() % 1024 + 1;
    }
}


void generate_bounds() {
    for (int i = 0; i < num_of_threads; i++) {
        thread_bounds[i][0] = (matrix_size / num_of_threads) * i;
        thread_bounds[i][1] = (matrix_size / num_of_threads) * (i + 1);
    }
}

int run_cycle(int cycle) {
    pthread_t thread_id[matrix_size];
    Two_d_matrixgen();
    generate_bounds();
    struct position_struct array_of_args[num_of_threads];
    for (int j = 0; j < num_of_threads; j++){
        array_of_args[j].position = j;
    }
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for (int i = 0; i < num_of_threads; i++){
        cout << "Currently doing mult for " << thread_bounds[i][0] << thread_bounds[i][1] << endl;
        pthread_create( &thread_id[i], NULL, multiply_row, &array_of_args[i]);
        cout << "Finished doing work for  " << thread_bounds[i][0] << thread_bounds[i][1] <<endl;
    }

    for (int i = 0; i < num_of_threads; i++){
        pthread_join(thread_id[i], NULL);
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>( t2 - t1 ).count();
    total_elapsed = duration + total_elapsed;
    myfile << cycle+1 << "," << (long)duration<< "," << total_elapsed << endl;
    return 0;
}


int main() {
    struct timespec start, finish;
    for (int cycle = 0; cycle < 100; cycle++) {
        run_cycle(cycle);
    }
    myfile.close();
}
