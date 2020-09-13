#include"glass.h"

int main()
{
    byte* pixels_df = NULL, * pixels_m = NULL;
    int32 width_df, width_m;
    int32 height_df, height_m;
    int32 bytesPerPixel_df, bytesPerPixel_m;
	
	BMP* BMPOp = new BMP();
	
	// location of defect free image and mask image
	char* defectFreeImage = "../../../images/defectFreeImages/Punkte 0.3.bmp",
		* dfWAlpha = "../../../images/maskWAlpha/Punkte 0.3Alpha.bmp",
		* maskImage = "../../../images/maskImages/Punkte 0.3mask.bmp",
		* maskWAlpha = "../../../images/maskWAlpha/Punkte 0.3AlphaMask.bmp",
		* maskedImage = "../../../images/maskedImages/test.bmp";
	
	std::string synthethicImages;

	// add alpha
	BMPOp->add_alpha(maskImage, 255);
	BMPOp->filter_channel(0, 0, 1);
	BMPOp->bw();
	BMPOp->write(maskWAlpha);
	BMPOp->add_alpha(defectFreeImage, 255);
	BMPOp->write(dfWAlpha);
	//BMPOp->overlay(dfWAlpha, maskWAlpha);
	//BMPOp->write(maskedImage);
	srand(1);
	int iCount = 1;
	while (iCount < 21) {
		int iMax = rand() % 5 + 1;
		BMPOp->read(dfWAlpha);
		for (int i = 0; i < iMax; i++) {
			synthethicImages = "../../../images/maskedImages/s" + std::to_string(iCount) + ".bmp";
			BMPOp->overlay(maskWAlpha);
		}
		BMPOp->write(synthethicImages.c_str());
		iCount++;
	}

	delete(BMPOp);
	return 0;
}