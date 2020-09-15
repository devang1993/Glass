#include"glass.h"

int main(int argc, char** argv)
{
	try
	{
		BMP* BMPOp = new BMP();

		// location of defect free image and mask image
		const char* defectFreeImage = "../../../images/defectFreeImages/Punkte 0.3.bmp",
			* dfWAlpha = "../../../images/maskWAlpha/Punkte 0.3Alpha.bmp",
			* maskImage = "../../../images/maskImages/Punkte 0.3mask.bmp",
			* maskWAlpha = "../../../images/maskWAlpha/Punkte 0.3AlphaMask.bmp",
			* maskedImage = "../../../images/maskedImages/test.bmp",
			* maskedImageDir = "../../../images/maskedImages/",
			* greyImage = "../../../images/greyImages/test.bmp",
			* sample8bit = "../../../images/maskImages/barbara_gray.bmp";

		std::string synthethicImages, fileName = "/s";
		std::error_code error;
		
		bool testing = false;
		
		if (testing == true)
		{
			cv::Mat image, BGR2G;
			//image = cv::imread(defectFreeImage, cv::IMREAD_COLOR);
			//cv::cvtColor(image, BGR2G, cv::COLOR_BGR2GRAY);
			//BMPOp->showImage("Image", BGR2G);
			//cv::imwrite(greyImage, BGR2G);

			const char* testing = defectFreeImage;
			BMPOp->read(testing);		// test 8 bit
			BMPOp->colour2grey();
			BMPOp->write(maskWAlpha);	
			BMPOp->showImage(maskWAlpha, cv::IMREAD_ANYCOLOR);//**
			BMPOp->read(testing);		// test 24 bit
			BMPOp->write(maskWAlpha);
			BMPOp->showImage(maskWAlpha, cv::IMREAD_ANYCOLOR);//**
			BMPOp->read(testing);		// test 32 bit
			BMPOp->add_alpha(0);
			BMPOp->write(maskWAlpha);
		}// end test
		else {
			BMPOp->read(maskImage);				// read mask image
			BMPOp->add_alpha(0);				// add alpha channel to mask
			BMPOp->setAlpha();					// set opacity to 0 in mask for insignificant pixels
			//BMPOp->colour2grey();				// convert colour to 8 bit grayscale
			BMPOp->write(maskWAlpha);			// save mask image with alpha channel
			BMPOp->read(defectFreeImage);		// read defect free image
			BMPOp->add_alpha(255);				// add alpha channel to defect free image
			//BMPOp->colour2grey();				// convert colour to 8 bit grayscale
			BMPOp->write(dfWAlpha);				// save defect free image with alpha channel

			srand(1);							// initialize seed to 1

			std::cout << "Clearing directory..." << std::endl;			// clear directory of masked images

			while (!std::filesystem::is_empty(maskedImageDir))
				for (const auto& entry : std::filesystem::directory_iterator(maskedImageDir)) {
					if (!std::filesystem::remove(entry.path(), error))
						std::cout << error.message() << std::endl;
				}

			if (!std::filesystem::exists(maskedImageDir))			// create directory for masked images if it does not exist
				std::filesystem::create_directory(maskedImageDir);

			std::cout << "Writing masked images..." << std::endl;		// start creating synthethic data

			for (int iCount = 0; iCount < 1000; iCount++) {
				int iMax = rand() % 5 + 1;							// maximum defects to create
				BMPOp->read(dfWAlpha);								// read defect free image with alpha channel
				synthethicImages = maskedImageDir + fileName + std::to_string(iCount + 1) + ".bmp";		// masked image file name and location

				for (int i = 0; i < iMax; i++)						// overlay random number of masks to defect free image at random positions
					BMPOp->overlay(maskWAlpha);

				BMPOp->write(synthethicImages.c_str());				// save synthethic image
			}
			std::cout << "Write success... " << std::endl;
		}
		delete(BMPOp);

	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << std::endl;
	}
	return 0;
}