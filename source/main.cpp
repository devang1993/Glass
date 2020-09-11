#include"../include/glass.h"

int main()
{
    byte* pixels_df = NULL, * pixels_m = NULL;
    int32 width_df, width_m;
    int32 height_df, height_m;
    int32 bytesPerPixel_df, bytesPerPixel_m;

	int x_offset = 0;
	int y_offset = 0;

	//readFile* file = new readFile();

	// read defect free images
    
	//file->readImage("../../../images/defectFreeImages/Punkte 0.3.bmp", &pixels_df, &width_df, &height_df, &bytesPerPixel_df);
	//free(pixels_df);
	BMP bmp("../../../images/defectFreeImages/Punkte 0.3.bmp");

	// read defect mask

	//file->readImage("../../../images/defectImages/Punkte 0.3 mask.bmp", &pixels_m, &width_m, &height_m, &bytesPerPixel_m);
	//free(pixels_m);
	BMP bmp1("../../../images/defectImages/Punkte 0.3 mask.bmp");

	// initialize random seed
	srand(1);

	//x_offset = rand() % (width_df - width_m);
	//y_offset = rand() % (height_df - height_m);
	//std::cout << x_offset << " " << y_offset << std::endl;

	//delete(file);
	return 0;
}