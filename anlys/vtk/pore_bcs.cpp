#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <time.h>

int num_px = 100;
int num_py = 100;
int num_pz = 40;
double x_edge = -50;
double y_edge = -50;
double z_edge = 0;
double l_pixl = 1;

void output_pixel()
{
	char ofs_name[64];
	sprintf( ofs_name, "test.vtk" );
	FILE *f_out;
	f_out = fopen( ofs_name, "w" );
	if( f_out == NULL ){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

	fprintf( f_out, "# vtk DataFile Version 2.0\n" );
	fprintf( f_out, "pixel\n" );
	fprintf( f_out, "ASCII\n" );
	fprintf( f_out, "DATASET RECTILINEAR_GRID\n" );
	fprintf( f_out, "DIMENSIONS %d %d %d\n", num_px, num_py, num_pz );
	fprintf( f_out, "X_COORDINATES %d float\n", num_px );
	for( int i=0; i<num_px; i++ ){
		fprintf( f_out, "%lf ", x_edge + l_pixl/2.0 + l_pixl*i );
	}
	fprintf( f_out, "\n" );
	fprintf( f_out, "Y_COORDINATES %d float\n", num_py );
	for( int i=0; i<num_py; i++ ){
		fprintf( f_out, "%lf ", y_edge + l_pixl/2.0 + l_pixl*i );
	}
	fprintf( f_out, "\n" );
	fprintf( f_out, "Z_COORDINATES %d float\n", num_pz );
	for( int i=0; i<num_pz; i++ ){
		fprintf( f_out, "%lf ", z_edge + l_pixl/2.0 + l_pixl*i );
	}
	fprintf( f_out, "\n" );

	// fprintf( f_out, "POINT_DATA %lu\n", pix_pos.size() );
	// fprintf( f_out, "SCALARS rad float\n" );
	// fprintf( f_out, "LOOKUP_TABLE default\n" );
	// // for( int i=0; i<pix_pos.size(); i++ ){
	// // 	fprintf( f_out, "%d\n", pixel[i] );
	// // }

	// for( int k=0; k<num_pz; k++ ){ // reverse order
	// for( int j=0; j<num_py; j++ ){
	// for( int i=0; i<num_px; i++ ){
	// 	int id = i*num_py*num_pz + j*num_pz + k;
	// 	fprintf( f_out, "%lf\n", pixel[id] );
	// }}}

	fclose(f_out);
	return;
}



int main(int argc, char *argv[]){

	output_pixel();

	return 0;
}
