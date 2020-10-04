#include"glass.hpp"

void fileList(const char* defectDir, std::vector<std::string>& files);

int main(int argc, char** argv)
{
	try
	{
		std::cout << "================================================================================" << std::endl;
		std::cout << "Program to convert images from 24 or 32 bit to 32, 24, 8, 4, 1 bit Bitmap images" << std::endl;
		std::cout << "and to overlay a mask random times at random positions for specified iterations." << std::endl;
		std::cout << "================================================================================" << std::endl;

		// location of defect free image and mask image
		const char* const defectFreeImage = "../../../images/defectFreeImages/Punkte 0.3.bmp",
			* dfWAlpha = "../../../images/maskWAlpha/Punkte 0.3Alpha.bmp",
			* maskImage = "../../../images/maskImages/Punkte 0.3mask.bmp",
			* maskWAlpha = "../../../images/maskWAlpha/Punkte 0.3AlphaMask.bmp",
			* maskedImage = "../../../images/maskedImages/masked.bmp",
			* maskedImageDir = "../../../images/maskedImages/",
			* maskedImageAnnDir = "../../../images/maskedImages/annotation/",
			* sample8bit = "../../../images/greyImages/df24to8.bmp",
			* sample4bit = "../../../images/greyImages/df24to4.bmp",
			* sample1bit = "../../../images/greyImages/df24to1.bmp",
			* testImage = "../../../images/testImage.bmp",
			// glass directory
			* petraGlass101Dir = "../../../images/defectImages/petraGlass1/petraGlass1_0.1",
			* petraGlass2Dir = "../../../images/defectImages/petraGlass2",
			* petraGlass3_S1Dir = "../../../images/defectImages/petraGlass3/petraGlass3_S1",
			* petraGlass3_S2Dir = "../../../images/defectImages/petraGlass3/petraGlass3_S2",
			* petraGlass4Dir = "../../../images/defectImages/petraGlass4",
			* petraGlass4DFDir = "../../../images/defectFreeImages/petraGlass4DF",
			* petraGlass5Dir = "../../../images/defectImages/petraGlass5",
			* petraGlass5DFDir = "../../../images/defectFreeImages/petraGlass5DF",
			* petraGlass5DFEDir = "../../../images/defectFreeImages/petraGlass5DF/petraGlass5DFE",
			* petraGlass6Dir = "../../../images/defectImages/petraGlass6/petraGlass6",
			* petraGlass6PADir = "../../../images/defectImages/petraGlass6/petraGlass6P",
			* temporalDir = "../../../images/temporal averaging",
		// array of files list for selection
			* file[] = {
			maskImage,
			sample8bit,
			sample4bit,
			sample1bit,
			defectFreeImage,
			testImage
		};

		std::string synthethicImages, fileName = "s";
		std::error_code error;
		bool runFlag = true;
		
		while (runFlag) {
			int mode = 0;
			std::cout << "Mode:" << std::endl;
			std::cout << "	1. Testing," << std::endl;
			std::cout << "	2. Temporal averaging," << std::endl;
			std::cout << "	3. Masking," << std::endl;
			std::cout << "	4. Test function," << std::endl;
			std::cout << std::endl;
			std::cout << "	0. Exit\t\t";
			std::cin >> mode;

			switch (mode) {
				// Exit
			case 0: {
				runFlag = false;
				break;
			}
				  // Testing
			case 1: {
				while (true) {
					//cv::Mat image, BGR2G;
					//image = cv::imread(defectFreeImage, cv::IMREAD_COLOR);
					//cv::cvtColor(image, BGR2G, cv::COLOR_BGR2GRAY);
					//BMPOp->showImage("Image", BGR2G);
					//cv::imwrite(greyImage, BGR2G);
					int bitSelection, user_in;
					std::cout << std::endl;
					std::cout << "		Select image:" << std::endl;
					std::cout << "			1. maskImage," << std::endl;
					std::cout << "			2. sample8bit," << std::endl;
					std::cout << "			3. sample4bit," << std::endl;
					std::cout << "			4. sample1bit," << std::endl;
					std::cout << "			5. defectFreeImage," << std::endl;
					std::cout << "			6. testImage," << std::endl;
					std::cout << std::endl;
					std::cout << "			0. Go back\t\t";
					std::cin >> user_in;

					if (user_in == 0)
						break;
					if (0 > user_in || user_in > (sizeof(file) / sizeof(file[0]) )) {
						std::cout << "			Invalid selection. Try again." << std::endl;
						continue;
					}

					BMP* BMPOp = new BMP();
					BMPOp->read(file[user_in - 1]);
					std::cout << std::endl;
					std::cout << "			Test 1, 4, 8, 24, 32 bit:\t";
					std::cin >> bitSelection;
					switch (bitSelection)
					{

					case 1: {
						BMPOp->bwThreshold = 20;
						BMPOp->colour1bit();		// test 1 bit
						break;
					}
					case 4:
						BMPOp->colour4bit();		// test 4 bit
						break;
					case 8:
						BMPOp->bwThreshold = 20;
						BMPOp->colour8bit();		// test 8 bit
						break;
					case 24:
						break;
					case 32:
						BMPOp->add_alpha(0);		// test 32 bit
						break;
					default:
						std::cout << "			Invalid bit size. Try again" << std::endl;
						break;
					}
					BMPOp->write(maskWAlpha);
					//BMPOp->showImage(maskWAlpha);//**
					delete(BMPOp);
				}
				break;
			}// end test
				// Temporal Averaging
			case 2: {
				bool runFlag2 = true, inpFlag = true, taLoop = false;
				do {
					inpFlag = true;
					int defectType = 0;
					std::cout << std::endl;
					std::cout << "		Select defect type:" << std::endl;
					std::cout << "			1. Petra Glass 1 0.1," << std::endl;
					std::cout << "			2. Petra Glass 2," << std::endl;
					std::cout << "			3. Petra Glass 3 S1," << std::endl;
					std::cout << "			4. Petra Glass 3 S2," << std::endl;
					std::cout << "			5. Petra Glass 4," << std::endl;
					std::cout << "			6. Petra Glass 4 defect free," << std::endl;
					std::cout << "			7. Petra Glass 5," << std::endl;
					std::cout << "			8. Petra Glass 5 defect free," << std::endl;
					std::cout << "			9. Petra Glass 5 defect free edge," << std::endl;
					std::cout << "			10. Petra Glass 6," << std::endl;
					std::cout << "			11. Petra Glass 6 PA," << std::endl;
					std::cout << std::endl;
					std::cout << "			12. Temporal average all," << std::endl;
					std::cout << std::endl;
					std::cout << "			0. Go back\t";
					std::cin >> defectType;
					switch (defectType) {
					case 0: {
						runFlag2 = false;
						break;
					}
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
						break;
					case 12:
						taLoop = true;
						break;
					default:
						std::cout << "			Invalid input. Try again" << std::endl;
						continue;
					}
					std::vector<std::string> files;
					std::string fileDir[] = {
						petraGlass101Dir,
						petraGlass2Dir,
						petraGlass3_S1Dir,
						petraGlass3_S2Dir,
						petraGlass4Dir,
						petraGlass4DFDir,
						petraGlass5Dir,
						petraGlass5DFDir,
						petraGlass5DFEDir,
						petraGlass6Dir,
						petraGlass6PADir };
					while (inpFlag && runFlag2) {
						do {
							if (taLoop == false)
								fileList(fileDir[defectType - 1].c_str(), files);
							else {
								defectType--;
								fileList(fileDir[defectType - 1].c_str(), files);
							}
							BMP* BMPOp = new BMP();
							std::vector<BMP*> img;
							int bitSelection = 0;
							if (taLoop == false) {
								std::cout << std::endl;
								std::cout << "			Use as 1, 8, or 24 bit:\t";
								std::cin >> bitSelection;
							}
							else
								bitSelection = 8;
							switch (bitSelection)
							{
							case 1:
							case 8:
							case 24:
								break;
							default:
								std::cout << "			Invalid input. Try again." << std::endl;
								continue;
							}
							img.resize(files.size());
							for (int i = 0; i < files.size(); i++) {
								BMPOp->read((char*)files[i].c_str());
								switch (bitSelection) {
								case 8:
									BMPOp->bwThreshold = 2;
									BMPOp->colour8bit();
									break;
								case 1:
									BMPOp->bwThreshold = 20;
									BMPOp->colour1bit();
									break;
								}
								img[i] = new BMP(*BMPOp);
							}
							BMPOp->data.clear();
							switch (bitSelection) {
							case 1: {
								BMPOp->data.resize(img[0]->data.size(), 0xFF);
								for (int i = 0; i < BMPOp->data.size(); i++)
									for (int j = 0; j < img.size(); j++)
										BMPOp->data[i] &= img[j]->data[i];
								BMPOp->write(maskWAlpha);
								break;
							}
							case 8: {
								int hist[256] = { 0 };//**
								BMPOp->data.resize(img[0]->data.size(), 0xFF);
								for (int i = 0; i < BMPOp->data.size(); i++) {
									for (int j = 0; j < img.size(); j++)
										BMPOp->data[i] &= img[j]->data[i];
									hist[BMPOp->data[i]]++;//**
								}
								std::ofstream csv;//**
								csv.open("histogram.csv");//**
								for (int i = 0; i < 256; i++)//**
									csv << hist[i] << "\n";//**
								csv.close();
								break;
							}
							case 24:
							case 32: {
								uint8_t new_size = 0;
								float mean = 0.0f, sigma = 0.0f, new_mean = 0.0f;
								BMPOp->data.resize(img[0]->data.size());
								for (int i = 0; i < BMPOp->data.size(); i++) {
									new_size = (uint8_t)img.size();
									mean = 0.0f, sigma = 0.0f, new_mean = 0.0f;
									for (int j = 0; j < img.size(); j++)
										mean += img[j]->data[i] / img.size();
									for (int j = 0; j < img.size(); j++)
										sigma += pow((img[j]->data[i] - mean), 2) / img.size();
									sigma = sqrt(sigma);
									for (int j = 0; j < img.size(); j++)
										if ((mean - sigma) < img[j]->data[i] && img[j]->data[i] < (mean + sigma))
											new_mean += img[j]->data[i];
										else
											new_size--;
									if (new_size != 0)
										BMPOp->data[i] = (uint8_t)(new_mean / new_size);
									else
										BMPOp->data[i] = 0;
								}
								BMPOp->write(maskWAlpha);
								break;
							}
							}
							if (bitSelection != 1 && taLoop == false) {
								do {
									switch (bitSelection) {
									case 24:
										std::cout << std::endl;
										std::cout << "			Convert to 1 or 8 bit? 0 for no:\t";
										break;
									case 8:
										std::cout << std::endl;
										std::cout << "			Convert to 1 bit? 0 for no:\t";
										break;
									}
									std::cin >> bitSelection;
									switch (bitSelection) {
									case 1:
										BMPOp->bwThreshold = 15;
										BMPOp->colour1bit();
										break;
									case 8:
										BMPOp->colour8bit();
										break;
									case 0:
										break;
									default:
										std::cout << "			Invalid input. Try again." << std::endl;
										continue;
									}
								} while (bitSelection != 1 && bitSelection != 8 && bitSelection != 0);
								std::string temporalAvg = temporalDir + fileDir[defectType - 1].substr(fileDir[defectType - 1].rfind("/")) + "TA.bmp";
								BMPOp->blob(9);
								BMPOp->write(temporalAvg.c_str());
								//if (bitSelection != 0)
								//	BMPOp->write(maskWAlpha);
							}
							for (int i = 0; i < files.size(); i++)
								delete (img[i]);
							inpFlag = false;
							delete(BMPOp);
							if (defectType == 1)
								taLoop = false;
						} while (taLoop == true);
						break;
					} // end while
				} while (runFlag2);
				break;
			}
				  // masking
			case 3: {
				bool runFlag2 = true, inpFlag = true;
				do {
					int bitSelection = 0;
					std::cout << std::endl;
					std::cout << "		Select glass to be masked:" << std::endl;
					std::cout << "			1. Petra ceramic glass" << std::endl;
					std::cout << "			2. Petra transparent glass" << std::endl;
					std::cout << "			3. Petra transparent glass edge"<< std::endl;
					std::cout << std::endl;
					std::cout << "			0. Go back:\t";
					std::cin >> bitSelection;
					if (bitSelection == 0) {
						runFlag2 = false;
						break;
					}
					std::string TAImage;
					BMP* BMPOp = new BMP();
					switch (bitSelection) {

					case 1: {
						TAImage = temporalDir + std::string("/petraGlass4DFTA.bmp");
						break;
					}
					case 2: {
						TAImage = temporalDir + std::string("/petraGlass5DFTA.bmp");
						break;
					}
					case 3: {
						TAImage = temporalDir + std::string("/petraGlass5DFETA.bmp");
						break;
					}

					//case 1: {
					//	BMPOp->bwThreshold = 25;
					//	BMPOp->read(maskImage);				// read mask image
					//	BMPOp->colour1bit();				// convert colour to 1 bit bw
					//	BMPOp->write(maskWAlpha);			// save mask image with alpha channel
					//	BMPOp->read(defectFreeImage);		// read defect free image
					//	BMPOp->colour1bit();				// convert colour to 1 bit bw
					//	BMPOp->write(dfWAlpha);				// save defect free image with alpha channel
					//	break;
					//}

					//case 8: {
					//	BMPOp->bwThreshold = 25;
					//	BMPOp->read(maskImage);				// read mask image
					//	BMPOp->colour8bit();				// convert colour to 8 bit grayscale
					//	BMPOp->write(maskWAlpha);			// save mask image with alpha channel
					//	BMPOp->read(defectFreeImage);		// read defect free image
					//	BMPOp->colour8bit();				// convert colour to 8 bit grayscale
					//	BMPOp->write(dfWAlpha);				// save defect free image with alpha channel
					//	break;
					//}

					//case 24: {
					//	BMPOp->bwThreshold = 70;
					//	BMPOp->read(maskImage);				// read mask image
					//	BMPOp->write(maskWAlpha);			// save mask image with alpha channel
					//	BMPOp->read(defectFreeImage);		// read defect free image
					//	BMPOp->write(dfWAlpha);				// save defect free image with alpha channel
					//	break;
					//}

					//case 32: {
					//	BMPOp->bwThreshold = 70;
					//	BMPOp->redThreshold = 30;
					//	BMPOp->read(maskImage);				// read mask image
					//	BMPOp->add_alpha(0);				// add alpha channel to mask
					//	BMPOp->setAlpha(150);					// set opacity to 0 in mask for insignificant pixels
					//	BMPOp->write(maskWAlpha);			// save mask image with alpha channel
					//	BMPOp->read(defectFreeImage);		// read defect free image
					//	BMPOp->add_alpha(255);				// add alpha channel to defect free image
					//	BMPOp->write(dfWAlpha);				// save defect free image with alpha channel
					//	break;
					//}

					default: {
						std::cout << "			Invalid input. Try again" << std::endl;
						inpFlag = true;
						continue;
					}
					
					}
					BMPOp->read(maskImage);				// read mask image
					BMPOp->colour8bit();				// convert colour to 8 bit grayscale
					BMPOp->write(maskWAlpha);			// save mask image with alpha channel

					srand(1);							// initialize seed to 1
					
					std::cout << std::endl;
					std::cout << "			Clearing directory..." << std::endl;			// clear directory of masked images
					while (!std::filesystem::is_empty(maskedImageAnnDir))
						for (const auto& entry : std::filesystem::directory_iterator(maskedImageAnnDir)) {
							if (!std::filesystem::remove(entry.path(), error))
								std::cout << error.message() << std::endl;
						}
					while (!std::filesystem::is_empty(maskedImageDir))
						for (const auto& entry : std::filesystem::directory_iterator(maskedImageDir)) {
							if (!std::filesystem::remove(entry.path(), error))
								std::cout << error.message() << std::endl;
						}
					if (!std::filesystem::exists(maskedImageDir))			// create directory for masked images if it does not exist
						std::filesystem::create_directory(maskedImageDir);
					if (!std::filesystem::exists(maskedImageAnnDir))			// create directory for masked images if it does not exist
						std::filesystem::create_directory(maskedImageAnnDir);

					std::cout << "			Writing masked images..." << std::endl;		// start creating synthethic data
					BMP trial;//**
					trial.createDefect();//**
					BMP* mask = new BMP();
					mask->read("lineDefect.bmp");
					BMPOp->read(TAImage.c_str());								// read defect free image with alpha channel
					std::vector<uint8_t> originalData;
					originalData.resize(BMPOp->data.size());
					originalData = BMPOp->data;
					for (int iCount = 0; iCount < 1000; iCount++) {
						int iMax = rand() % 5 + 1;							// maximum defects to create
						BMPOp->data = originalData;
						for (int i = 0; i < iMax; i++)						// overlay random number of masks to defect free image at random positions
							BMPOp->overlay(mask);
						synthethicImages = maskedImageDir + fileName + std::to_string(iCount + 1) + ".bmp";		// masked image file name and location
						BMPOp->write(synthethicImages.c_str());				// save synthethic image
						std::string ann = maskedImageAnnDir + fileName + std::to_string(iCount + 1) + ".txt";
						std::ofstream annotation(ann, std::ios::out | std::ios::binary);
						annotation.write((char*)BMPOp->classMap.data(), BMPOp->classMap.size());
						annotation.close();
					}
					std::cout << "			Write success... " << std::endl;
					delete(mask);
					delete(BMPOp);
				} while (runFlag2);
				break;
			}
				// test function
			case 4: {
				BMP* BMPOp = new BMP();
				BMPOp->read(testImage);
				BMPOp->blob(7);
				BMPOp->write(maskWAlpha);
				delete(BMPOp);
				break;
			}
				  // Invalid input
			default: {
				std::cout << "Invalid mode. Try again." << std::endl;
				std::cout << std::endl;
				continue;
			}
			}
			std::cout << std::endl;
		}
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << std::endl;
	}
	return 0;
}

void fileList(const char* defectDir, std::vector<std::string>& files) {
	files.clear();
	for (const auto& entry : std::filesystem::directory_iterator(defectDir)) {
		if (!entry.is_directory())
			files.push_back(entry.path().u8string());
	}
}