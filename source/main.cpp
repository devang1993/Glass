#include"glass.h"

int main()
{
	try
	{
		BMP* BMPOp = new BMP();

		// location of defect free image and mask image
		char* defectFreeImage = "../../../images/defectFreeImages/Punkte 0.3.bmp",
			* dfWAlpha = "../../../images/maskWAlpha/Punkte 0.3Alpha.bmp",
			* maskImage = "../../../images/maskImages/Punkte 0.3mask.bmp",
			* maskWAlpha = "../../../images/maskWAlpha/Punkte 0.3AlphaMask.bmp",
			* maskedImage = "../../../images/maskedImages/test.bmp",
			* maskedImageDir = "../../../images/maskedImages";

		std::string synthethicImages;

		// add alpha
		BMPOp->read(maskImage);
		BMPOp->add_alpha(255);
		BMPOp->setAlpha();
		BMPOp->write(maskWAlpha);
		BMPOp->read(defectFreeImage);
		BMPOp->add_alpha(255);
		BMPOp->write(dfWAlpha);

		srand(1);

		int iCount = 0;

		while (iCount < 1000) {

			int iMax = rand() % 5 + 1;
			
			BMPOp->read(dfWAlpha);
			
			synthethicImages = "../../../images/maskedImages/s" + std::to_string(iCount + 1) + ".bmp";

			if(!std::filesystem::remove_all(maskedImageDir))
				std::cout << "Error deleting file" << std::endl;

			for (int i = 0; i < iMax; i++)
				BMPOp->overlay(maskWAlpha);
			
				
			BMPOp->write(synthethicImages.c_str());
			
			iCount++;
		}

		delete(BMPOp);

	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << std::endl;
	}
	return 0;
}