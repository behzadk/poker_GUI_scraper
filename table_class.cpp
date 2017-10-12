#include "table_class.h"
#include <iostream>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <tesseract/baseapi.h>
#include <opencv2/text.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <vector>
#include <string>
#include <ctime>
#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED 
#include <boost/filesystem.hpp>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>

PokerTable::PokerTable (char* name, int num_seats, Window win)
{
	table_name = name;
	max_seats = num_seats;
	table_win = win;
}

void PokerTable::MatType( cv::Mat inputMat )
{
    int inttype = inputMat.type();

    std::string r, a;
    uchar depth = inttype & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (inttype >> CV_CN_SHIFT);
    switch ( depth ) {
        case CV_8U:  r = "8U";   a = "Mat.at<uchar>(y,x)"; break;  
        case CV_8S:  r = "8S";   a = "Mat.at<schar>(y,x)"; break;  
        case CV_16U: r = "16U";  a = "Mat.at<ushort>(y,x)"; break; 
        case CV_16S: r = "16S";  a = "Mat.at<short>(y,x)"; break; 
        case CV_32S: r = "32S";  a = "Mat.at<int>(y,x)"; break; 
        case CV_32F: r = "32F";  a = "Mat.at<float>(y,x)"; break; 
        case CV_64F: r = "64F";  a = "Mat.at<double>(y,x)"; break; 
        default:     r = "User"; a = "Mat.at<UKNOWN>(y,x)"; break; 
    }
    r += "C";
    r += (chans+'0');
    std::cout << "Mat is of type " << r << " and should be accessed with " << a << std::endl;
}

void PokerTable::set_name(char* name)
{
	table_name = name;
}

void PokerTable::set_max_seats(int num_seats)
{
	max_seats = num_seats;
}

char* PokerTable::get_table_name()
{
	return table_name;
}

void PokerTable::get_table_image(Display *disp)
{
	using namespace cv;
	std::vector<uint8_t> Pixels;

	XWindowAttributes attributes = {0};
    XGetWindowAttributes(disp, table_win, &attributes);

	int& Width = attributes.width;
	int& Height = attributes.height;
	XImage* img = XGetImage(disp, table_win, 0, 0, Width, Height, AllPlanes, ZPixmap);

	//std::cout << (Width) << ", " <<  (Height) << std::endl;

	int& BitsPerPixel = img->bits_per_pixel;
    Pixels.resize(Width * Height * 4);
    memcpy(&Pixels[0], img->data, Pixels.size());

    cv::Mat mat_img = Mat(Height, Width,  BitsPerPixel > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]); //Mat(Size(Height, Width), Bpp > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]); 

    this->table_img = mat_img.clone();

	XDestroyImage(img);
    return;
}

cv::Mat PokerTable::get_pot_text_img()
{
	cv::Rect myROI(pot_ROI[0], pot_ROI[1], pot_ROI[2], pot_ROI[3]);
	cv::Mat cropped_img = table_img(myROI);
	return cropped_img;
}

void PokerTable::set_pot_size()
{
	/**
		Gets the size of pot from current table image loaded.
			First converts to img compatible with Teseract
			Runs tesseract against whitelist containing numbers only 
			Converts to int....
	*/
	using namespace cv;
    using namespace cv::text;
    using namespace std;
	cv::Mat pot_img;
	cv::Mat resize_pot;

	cvtColor(get_pot_text_img(), pot_img, CV_BGRA2GRAY);
	resize(pot_img, resize_pot, Size(), 2, 2);

	cv::Mat thres_pot_img;
	//double thresh = 100;
	//double max_value = 255;
	//(resize_pot, thres_pot_img, thresh, max_value, THRESH_BINARY);

	const std::string char_string_list = ":0123456789";
    Ptr<OCRTesseract> ocr = cv::text::OCRTesseract::create(NULL, NULL, NULL, 0);
    ocr->setWhiteList(char_string_list);

    std::vector<cv::Rect> tess_rects;

    std::string tess_out;
    ocr->run(resize_pot, tess_out, &tess_rects);

    std::string num_path = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/text_imgs/pot/";

    std::size_t colon_pos = tess_out.find(":");

    tess_out.erase(0, colon_pos+1);//Remove all chars before colon
   	tess_out.erase(std::remove(tess_out.begin(), tess_out.end(), ' '), tess_out.end()); //Remove all whitespace

   	try {
   		pot_size =  std::stoi(tess_out);
   	}

   	catch(std::invalid_argument& e){
   		time_t timestamp = time(NULL);
		to_string(timestamp);
		string error_img_path = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/errors/_pot_stoi_";
  		save_table_img(error_img_path);
	}


   
}

void PokerTable::save_table_img(std::string save_path){
	using namespace cv;
	using namespace std;
	time_t timestamp = time(NULL);
	to_string(timestamp);
	imwrite( save_path + to_string(timestamp) + ".png", table_img);
}

void PokerTable::save_ROI(int ROI[], std::string path){
	using namespace cv;
	using namespace std;

	cv::Rect myROI(ROI[0], ROI[1], ROI[2], ROI[3]);
	cv::Mat cropped_img = table_img(myROI);

	time_t timestamp = time(NULL);
	to_string(timestamp);
	imwrite( path + to_string(timestamp) + ".png", cropped_img);
}

cv::Mat PokerTable::get_ROI_mat(int ROI[]){
	using namespace cv;
	using namespace std;

	cv::Rect myROI(ROI[0], ROI[1], ROI[2], ROI[3]);
	cv::Mat cropped_img = table_img(myROI);

	return cropped_img;
}


void PokerTable::template_match_test(std::string template_path)
{
	using namespace cv;
	using namespace std;

	Mat template_img = imread(template_path, 1);
	Mat roi_img = get_ROI_mat(table_card_num_1);
	Mat adjust_img;
	cvtColor(roi_img, adjust_img, CV_BGRA2BGR);

	MatType(adjust_img);
	MatType(template_img);
	imshow("display", adjust_img);
	waitKey(0);
	imshow("display", template_img);
	waitKey(0);

	
	Mat match_temp_output;
	matchTemplate(adjust_img, template_img, match_temp_output, CV_TM_CCOEFF_NORMED);
	double minval, maxval, threshold = 0.8;
	cv::Point minloc, maxloc;
	minMaxLoc(match_temp_output, &minval, &maxval, &minloc, &maxloc);

	if (maxval >= threshold)
	{
		std::cout << "is a heart" << std::endl;
		imshow("display window", adjust_img);
		waitKey(0);
	}
	
}

void PokerTable::set_board_cards()
{
	using namespace cv;
	using namespace std;

	time_t timestamp = time(NULL);
	std::string time_string =  to_string(timestamp);
	vector<string> tc_vector{"xx", "xx", "xx", "xx", "xx"};
	
	for (unsigned int card_num = 0; card_num < table_cards.size(); ++card_num)
	{
		cv::Rect num_ROI_rect(table_card_rois[card_num][0][0], table_card_rois[card_num][0][1], table_card_rois[card_num][0][2], table_card_rois[card_num][0][3]);
		cv::Mat num_cropped_img = table_img(num_ROI_rect);
		cv::Mat gray_num_img;

		cvtColor(num_cropped_img, gray_num_img, CV_BGRA2GRAY);
		string card_num_string = match_img_to_template(num_template_dir, gray_num_img);
    
		cv::Rect suit_ROI_rect(table_card_rois[card_num][1][0], table_card_rois[card_num][1][1], table_card_rois[card_num][1][2], table_card_rois[card_num][1][3]);
		cv::Mat suit_cropped_img = table_img(suit_ROI_rect);
		cv::Mat gray_suit_img;
		cvtColor(suit_cropped_img, gray_suit_img, CV_BGRA2GRAY);

		string card_suit_string = match_img_to_template(suit_template_dir, gray_suit_img);
		std::string card_id = card_num_string + card_suit_string;
		tc_vector[card_num] = card_id;
	}
	table_cards = tc_vector;
}

std::vector<std::string> PokerTable::get_board_cards()
{
	return table_cards;
}

void PokerTable::list_board_cards()
{
    
    for (unsigned int i = 0; i < (table_cards.size()); ++i)
    {
        std::cout << table_cards[i];
        std::cout << ", ";
    }
}


std::string PokerTable::match_img_to_template(boost::filesystem::path template_dir, cv::Mat unkown_img)
{
	using namespace cv;
	using namespace std;
	
	boost::filesystem::recursive_directory_iterator it(template_dir);
	boost::filesystem::recursive_directory_iterator endit;
	std::string ext = ".png";


	while(it != endit)
	{
		if(boost::filesystem::is_regular_file(*it) && it->path().extension() == ext)
		{
			Mat template_img = imread((it->path()).string(), 0);
			Mat match_temp_output;

			matchTemplate(unkown_img, template_img, match_temp_output, CV_TM_CCOEFF_NORMED);
			double minval, maxval, threshold = 0.85;
			cv::Point minloc, maxloc;
			minMaxLoc(match_temp_output, &minval, &maxval, &minloc, &maxloc);

			if (maxval >= threshold)
			{
				return (it->path().stem().string());
			}

		}
		it++;
	}
	return (std::string("X"));
}



void PokerTable::template_itr(boost::filesystem::path template_dir, cv::Mat unkown_img, std::string file_prefix)
{
	/**
	Takes an image and iterates through directory of templates. 
	Image must be in 8CU3 format
		If it matches, save the image as the same name as the template
		If it does not match, save the image as timestamp
	*/
	using namespace cv;
	using namespace std;
	std::string ext = ".png";
	std::string match_path = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/num_investig/correct/";
	std::string unknown_path = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/num_investig/unknown/";

	time_t timestamp = time(NULL);
	to_string(timestamp);

	//Iterate through templates at dir
	boost::filesystem::recursive_directory_iterator it(template_dir);
	boost::filesystem::recursive_directory_iterator endit;

	while(it != endit)
	{
		if(boost::filesystem::is_regular_file(*it) && it->path().extension() == ext)
		{
			Mat template_img = imread((it->path()).string(), 0);
			Mat match_temp_output;

			matchTemplate(unkown_img, template_img, match_temp_output, CV_TM_CCOEFF_NORMED);
			double minval, maxval, threshold = 0.85;
			cv::Point minloc, maxloc;
			minMaxLoc(match_temp_output, &minval, &maxval, &minloc, &maxloc);

			if (maxval >= threshold)
			{
				imwrite(match_path + (it->path().filename().string()) + "__"  + file_prefix + to_string(timestamp) + "__" + to_string(maxval) +  ".png", unkown_img);
				return;
			}

		}
		it++;
	}
	imwrite(unknown_path + "unknown" + file_prefix + to_string(timestamp) + "__" +  ".png", unkown_img);
}

void PokerTable::set_player_cards()
{
	using namespace cv;
	using namespace std;

	time_t timestamp = time(NULL);
	std::string match_path = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/num_investig/correct/";
	std::string mismatch_path = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/num_investig/mismatch/";
	std::string time_string =  to_string(timestamp);


	for (unsigned int player_pos = 0; player_pos < player_cards.size(); ++player_pos)
	{
		if (player_statuses[player_pos] == 0) {
			continue;
		}
		vector<string> p_cards_vector{"xx", "xx"};
		cv::Rect L_num_ROI_rect(player_card_rois[player_pos][0][0], player_card_rois[player_pos][0][1], player_card_rois[player_pos][0][2], player_card_rois[player_pos][0][3]);
		cv::Rect L_suit_ROI_rect(player_card_rois[player_pos][1][0], player_card_rois[player_pos][1][1], player_card_rois[player_pos][1][2], player_card_rois[player_pos][1][3]);
		cv::Rect R_num_ROI_rect(player_card_rois[player_pos][2][0], player_card_rois[player_pos][2][1], player_card_rois[player_pos][2][2], player_card_rois[player_pos][2][3]);
		cv::Rect R_suit_ROI_rect(player_card_rois[player_pos][3][0], player_card_rois[player_pos][3][1], player_card_rois[player_pos][3][2], player_card_rois[player_pos][3][3]);


		cv::Mat L_num_cropped_img = table_img(L_num_ROI_rect);
		cv::Mat L_suit_cropped_img = table_img(L_suit_ROI_rect);
		cv::Mat R_num_cropped_img = table_img(R_num_ROI_rect);
		cv::Mat R_suit_cropped_img = table_img(R_suit_ROI_rect);

		cv::Mat L_num_gray;
		cv::Mat L_suit_gray;
		cv::Mat R_num_gray;
		cv::Mat R_suit_gray;

		cvtColor(L_num_cropped_img, L_num_gray, CV_BGRA2GRAY);
		cvtColor(L_suit_cropped_img, L_suit_gray, CV_BGRA2GRAY);
		cvtColor(R_num_cropped_img, R_num_gray, CV_BGRA2GRAY);
		cvtColor(R_suit_cropped_img, R_suit_gray, CV_BGRA2GRAY);

		string L_num_string = match_img_to_template(num_template_dir, L_num_gray);
		string L_suit_string = match_img_to_template(suit_template_dir, L_suit_gray);
		string R_num_string = match_img_to_template(num_template_dir, R_num_gray);
		string R_suit_string = match_img_to_template(suit_template_dir, R_suit_gray);

		string L_card_ID = L_num_string + L_suit_string;
		string R_card_ID = R_num_string + R_suit_string;
		p_cards_vector[0] = L_card_ID;
		p_cards_vector[1] = R_card_ID;

		player_cards[player_pos][0] = L_card_ID;
		player_cards[player_pos][1] = R_card_ID;

		//Analytical options
		/**
		Save matches
		*/
		/**
		if ( (L_num_string != "X") ){
			imwrite(match_path + to_string(player_pos) + "__"  + L_num_string + to_string(timestamp) + "__" + ".png", L_num_cropped_img);
		}
		if ( (L_suit_string != "X") ){
			imwrite(match_path + to_string(player_pos) + "__"  + "incorr_" + L_suit_string +"__" + to_string(timestamp) + "__" + ".png", L_suit_cropped_img);
			imwrite(match_path + to_string(player_pos) + "__"  + "incorr_" + L_suit_string +"__" + to_string(timestamp) + "__" + ".png", L_suit_cropped_img);
			//imwrite(match_path + to_string(player_pos) + "__"  + L_suit_string + to_string(timestamp) + "__" + ".png", L_suit_cropped_img);
		}
		if (R_num_string != "X") {
			imwrite(match_path + to_string(player_pos) + "__"  + R_num_string + to_string(timestamp) + "__" + ".png", R_num_cropped_img);
		}
		if (R_suit_string != "X") {
			imwrite(match_path + to_string(player_pos) + "__"  + R_suit_string + to_string(timestamp) + "__" + ".png", R_suit_cropped_img);
		}
		*/

		/*
		Save mismatches (when we recognise one part of the card but not the other)
		*/
		if ( ( (L_num_string != "X") && (L_suit_string == "X") ) or ((L_num_string == "X") && (L_suit_string!= "X")) )	{
			cout << "set_player_cards imwrite";
			imwrite(mismatch_path + to_string(player_pos) + "__"  + L_num_string + "__" +to_string(timestamp) + "__" + ".png", L_num_cropped_img);
			imwrite(mismatch_path + to_string(player_pos) + "__"  + L_suit_string +"__" + to_string(timestamp) + "__" + ".png", L_suit_cropped_img);
			imwrite(mismatch_path + "table_img" + "__" +  to_string(timestamp) + "__" + ".png", table_img);
		}

		if ( ( (R_num_string != "X") && (R_suit_string == "X") ) or ((R_num_string == "X") && (R_suit_string!= "X")) ){
			cout << "set_player_cards imwrite";
			imwrite(mismatch_path + to_string(player_pos) + "__"  + R_num_string + "__" + to_string(timestamp) + "__" + ".png", R_num_cropped_img);
			imwrite(mismatch_path + to_string(player_pos) + "__"  + R_suit_string + "__" + to_string(timestamp) + "__" + ".png", R_suit_cropped_img);
			imwrite(mismatch_path + "table_img" + "__" +  to_string(timestamp) + "__" + ".png", table_img);
		}

	}
	/*
	for (unsigned int player_pos = 0; card_num < player_cards_test.size(); ++card_num)
	{
		vector<string> player_vector{"xx", "xx"};
		cv::Rect num_ROI_rect(player_card_rois[card_num][0][0], player_card_rois[card_num][0][1], player_card_rois[card_num][0][2], player_card_rois[card_num][0][3]);
		cv::Mat num_cropped_img = table_img(num_ROI_rect);
		cv::Mat gray_num_img;
		cvtColor(num_cropped_img, gray_num_img, CV_BGRA2GRAY);
		string card_num_string = match_img_to_template(num_template_dir, gray_num_img);

    
		cv::Rect suit_ROI_rect(player_card_rois[card_num][1][0], player_card_rois[card_num][1][1], player_card_rois[card_num][1][2], player_card_rois[card_num][1][3]);
		cv::Mat suit_cropped_img = table_img(suit_ROI_rect);
		cv::Mat gray_suit_img;
		cvtColor(suit_cropped_img, gray_suit_img, CV_BGRA2GRAY);
		string card_suit_string = match_img_to_template(suit_template_dir, gray_suit_img);

		
		std::string card_id = card_num_string + card_suit_string;
		cout << card_id;
	}
	*/
}

void PokerTable::set_player_names()
{
	using namespace std;
	using namespace cv;
	using namespace cv::text;

	for (unsigned int player_pos = 0; player_pos < sizeof(player_username_rois)/sizeof(player_username_rois[0]); ++player_pos) {
		cv::Rect name_ROI_rect(player_username_rois[player_pos][0], player_username_rois[player_pos][1], player_username_rois[player_pos][2], player_username_rois[player_pos][3]);

		cv::Mat original_img = table_img(name_ROI_rect);
		cv::Mat gray_name_img;
		cv::Mat resize_name;


		cvtColor(original_img, gray_name_img, CV_BGRA2RGB);
		resize(gray_name_img, resize_name, Size(), 4, 4);

		cv::Mat thres_name_img;
		double thresh = 130;
		double max_value = 255;
		threshold(resize_name, thres_name_img, thresh, max_value, THRESH_BINARY);


		vector<float> confidences;
	    const std::string char_string_list = ":0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	    Ptr<OCRTesseract> ocr = cv::text::OCRTesseract::create(NULL, NULL, NULL, 0, 7);
	    ocr->setWhiteList(char_string_list);
	    std::string tess_out;
	    //MatType(resize_name);
	    ocr->run(thres_name_img, tess_out);
	    //std::cout << "Name: " << tess_out << endl;
	    //imshow("Display window", thres_name_img);
	   	//waitKey(0);
	    /*
	    for(unsigned int i = 0; i < confidences.size(); ++i){
	    	cout << confidences[i];
	    	cout <<"_";
	    }
	    */
		tess_out.erase(std::remove(tess_out.begin(), tess_out.end(), '\n'), tess_out.end());
		player_names[player_pos] = tess_out;
	}

}

void PokerTable::set_player_stacks()
{
	using namespace std;
	using namespace cv;
	using namespace cv::text;


	for (unsigned int player_pos = 0; player_pos < sizeof(player_stack_rois)/sizeof(player_stack_rois[0]); ++player_pos) {

		if (player_statuses[player_pos] == 0) {
			continue;
		}

		cv::Rect stack_ROI_rect(player_stack_rois[player_pos][0], player_stack_rois[player_pos][1], player_stack_rois[player_pos][2], player_stack_rois[player_pos][3]);
		cv::Mat original_img = table_img(stack_ROI_rect);
		cv::Mat gray_stack_img;
		cv::Mat resize_stack;


		cvtColor(original_img, gray_stack_img, CV_BGRA2RGB);
		resize(gray_stack_img, resize_stack, Size(), 4, 4  );

		cv::Mat thres_stack_img;
		double thresh = 130;
		double max_value = 255;
		threshold(resize_stack, thres_stack_img, thresh, max_value, THRESH_BINARY);
	//	imshow("Display window", thres_name_img);
	//   waitKey(0);

		vector<float> confidences;
	    const std::string char_string_list = "0123456789,";
	    Ptr<OCRTesseract> ocr = cv::text::OCRTesseract::create(NULL, NULL, NULL, 0, 7);
	    ocr->setWhiteList(char_string_list);
	    std::string tess_out;
	    //MatType(resize_name);
	    //ocr->run(thres_name_img, tess_out, NULL, NULL, &confidences);
	    ocr->run(thres_stack_img, tess_out);\
	    //std::cout << tess_out << std::endl;
	   	//imshow("Display window", thres_stack_img);
		//waitKey(0);
	    /*
	    for(unsigned int i = 0; i < confidences.size(); ++i){
	    	cout << confidences[i];
	    	cout <<"_";
	    }
	    */
	    tess_out.erase(remove_if(tess_out.begin(), tess_out.end(), ::isspace), tess_out.end() );

	    player_stacks[player_pos] = tess_out;
	}
}

void PokerTable::set_active_players()
{
	using namespace std;
	using namespace cv;
	using namespace cv::text;

	std::string error_path = "/home/behzad/Documents/cPlusPlus_projects/poker/error_tables/card_back";
	for (unsigned int player_pos = 0; player_pos < player_cards.size(); ++player_pos) {
		cv::Rect card_back_rec(player_card_rois[player_pos][0][0], player_card_rois[player_pos][0][1], player_card_rois[player_pos][0][2] + 40, player_card_rois[player_pos][0][3] + 40);

		cv::Mat card_back_crop = table_img(card_back_rec);

		cv::Mat card_back_gray;
		cvtColor(card_back_crop, card_back_gray, CV_BGRA2GRAY);
		string match_string = match_img_to_template(card_back_template_dir, card_back_gray);
		
		time_t timestamp = time(NULL);
		to_string(timestamp);

		if (match_string == "back") {
			player_statuses[player_pos] = 1;
		}
		else {
			player_statuses[player_pos] = 0;
		}

	}
}

int PokerTable::get_number_active_players()
{
	using namespace std;
	using namespace cv;
	using namespace cv::text;

	int sum = 0;
	for (unsigned int i = 0; i < player_statuses.size(); ++i) {
		sum+=player_statuses[i];
	}

	return sum;
}


void PokerTable::set_dealer_btn_pos()
{
	using namespace std;
	using namespace cv;
	using namespace cv::text;

	std::string error_path = "/home/behzad/Documents/cPlusPlus_projects/poker/error_tables/no_dealer_btn_";

	for (unsigned int player_pos = 0; player_pos < sizeof(player_username_rois)/sizeof(player_username_rois[0]); ++player_pos) {
		cv::Rect btn_ROI_rect(dealer_btn_rois[player_pos][0], dealer_btn_rois[player_pos][1], dealer_btn_rois[player_pos][2], dealer_btn_rois[player_pos][3]);

		cv::Mat btn_cropped_img = table_img(btn_ROI_rect);

		cv::Mat btn_gray;
		cvtColor(btn_cropped_img, btn_gray, CV_BGRA2GRAY);
		string match_string = match_img_to_template(btn_template_dir, btn_gray);
		if (match_string == "btn") {
			dealer_pos = player_pos;
			return;
		}
	}

	cout << "No dealer button found, saving table Image" << endl;
	save_table_img(error_path);
}


std::string PokerTable::get_game_phase()
{
	using namespace std;

	int card_num = 0;

	for (unsigned int i = 0; i < table_cards.size(); ++i) {
		if (table_cards[i] == "XX") {
			card_num = i;
			break;
		}
	}

	if (card_num == 0) {
		return "Pre-flop";
	}

	else if (card_num == 2) {
		return "Flop";
	}

	else if (card_num == 3) {
		return "Turn";
	}

	else if (card_num == 4) {
		return "River";
	}

	return "incorrect";
}

void PokerTable::table_report()
{
	using namespace std;

	time_t rawtime = time(NULL);
	string timestamp = to_string(rawtime);

	ofstream outfile;
	outfile.open("test", ios_base::app);

	outfile << timestamp<< endl;
	outfile << left << 
	setw(10) <<
	setw(10) << "Pot" <<
	setw(10) << "Board" << endl << flush;

	outfile << setw(10) << pot_size <<
	setw(10) << table_cards[0] + table_cards[1] + table_cards[2] + table_cards[3] + table_cards[4] << endl;
	outfile << endl;
	outfile << endl;
	outfile << left << setw(30) << "Pos"<< 
	setw(30) << left << "Name" << 
	setw(30) << left << "Status" << 
	setw(30) << left << "Stack" << 
	setw(30) << left << "Hand" << endl << flush;

	for (unsigned int i = 0; i < player_cards.size(); ++i) {
		//Player Name


		//Get Player Status
		int player_active = player_statuses[i];
		string player_status;
		if (player_active == 1) {
			player_status = "Active";
		}

		else if (player_active == 0) {
			player_status = "Inactive";
		}

		outfile << left << 
		setw(30) << left << i << 
		setw(30) << left << player_names[i] <<
		setw(30) << left << player_status <<
		setw(30) << left << player_stacks[i] <<
		setw(30) << left << player_cards[i][0] + player_cards[i][1] << endl << endl << flush;
	}
}

void PokerTable::new_hand()
{
	using namespace std;
	cout <<"starting new hand" << endl;
	for (unsigned int i=0; i <9; i++) {
		player_cards[i] = {"NA", "NA"};
	}
	cout << "new hand" << endl;
}

bool PokerTable::check_for_new_hand()
{
	int prev_btn = this -> get_dealer_btn_pos();
    this -> set_dealer_btn_pos();
    int new_btn = this -> get_dealer_btn_pos();

    if (new_btn != prev_btn) {
    	return true;
    }

    else {
    	return false;
    }
}