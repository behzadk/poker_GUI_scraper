#ifndef __POKERTABLE_H_INCLUDED__
#define __POKERTABLE_H_INCLUDED__

#include "table_class.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <tesseract/baseapi.h>
#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED 
#include <boost/filesystem.hpp>


class PokerTable 
{
	boost::filesystem::path num_template_dir = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/cropped_num/";
	boost::filesystem::path suit_template_dir = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/cropped_suit/";
	boost::filesystem::path btn_template_dir = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/cropped_btn/";
	boost::filesystem::path card_back_template_dir = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/card_back/";

 	char* table_name;
	int max_seats;
	Window table_win;
	int pot_ROI[4] = {313, 137, 180, 35}; //x1, y1, width, height

	int p1_card_num_L[4] = {349, 340, 12, 18};
	int p1_card_num_R[4] = {397, 340, 12, 18};
	int p1_card_suit_L[4] = {348, 357, 14, 17};
	int p1_card_suit_R[4] = {396, 357, 14, 17};
	int p1_username_ROI[4] = {318, 382, 158, 22};
	int p1_stack_ROI[4] = {318, 404, 158, 22};


	int p2_card_num_L[4] = {123, 298, 12, 18};
	int p2_card_num_R[4] = {171, 298, 12, 18};
	int p2_card_suit_L[4] = {122, 315, 14, 17};
	int p2_card_suit_R[4] = {170, 315, 14, 17};
	int p2_username_ROI[4] = {91, 339, 158, 22};
	int p2_stack_ROI[4] = {91, 361, 158, 22};

	int p3_card_num_L[4] = {30, 189, 12, 18};
	int p3_card_num_R[4] = {78, 189, 12, 18};
	int p3_card_suit_L[4] = {30, 206, 14, 17};
	int p3_card_suit_R[4] = {78, 206, 14, 17};
	int p3_username_ROI[4] = {0, 223, 158, 22};
	int p3_stack_ROI[4] = {0, 245, 158, 22};

	int p4_card_num_L[4] = {74, 75, 12, 18};
	int p4_card_num_R[4] = {122, 75, 12, 18};
	int p4_card_suit_L[4] = {73, 92, 14, 17};
	int p4_card_suit_R[4] = {121, 92, 14, 17};
	int p4_username_ROI[4] = {41, 110, 158, 22};
	int p4_stack_ROI[4] = {41, 132, 158, 22};

	int p5_card_num_L[4] = {229, 20, 12, 18};
	int p5_card_num_R[4] = {277, 20, 12, 18};
	int p5_card_suit_L[4] = {229, 36, 14, 17};
	int p5_card_suit_R[4] = {277, 36, 14, 17};
	int p5_username_ROI[4] = {198, 54, 158, 22};
	int p5_stack_ROI[4] = {198, 76, 158, 22};

	int p6_card_num_L[4] = {466, 20, 12, 18};
	int p6_card_num_R[4] = {514, 20, 12, 18};
	int p6_card_suit_L[4] = {466, 36, 14, 17};
	int p6_card_suit_R[4] = {514, 36, 14, 17};
	int p6_username_ROI[4] = {434, 55, 158, 22};
	int p6_stack_ROI[4] = {434, 77, 158, 22};

	int p7_card_num_L[4] = {622, 76, 12, 18};
	int p7_card_num_R[4] = {670, 76, 12, 18};
	int p7_card_suit_L[4] = {622, 93, 14, 17};
	int p7_card_suit_R[4] = {670, 93, 14, 17};
	int p7_username_ROI[4] = {590, 111, 158, 22};
	int p7_stack_ROI[4] = {590, 133, 158, 22};

	int p8_card_num_L[4] = {665, 189, 12, 18};
	int p8_card_num_R[4] = {713, 189, 12, 18};
	int p8_card_suit_L[4] = {665, 206, 14, 17};
	int p8_card_suit_R[4] = {713, 206, 14, 17};
	int p8_username_ROI[4] = {634, 224, 158, 22};
	int p8_stack_ROI[4] = {634, 246, 158, 22};

	int p9_card_num_L[4] = {575, 298, 12, 18};
	int p9_card_num_R[4] = {623, 298, 12, 18};
	int p9_card_suit_L[4] = {574, 315, 14, 17};
	int p9_card_suit_R[4] = {622, 315, 14, 17};
	int p9_username_ROI[4] = {542, 340, 158, 22};
	int p9_stack_ROI[4] = {542, 362, 158, 22};

	int dealer_btn_p1[4] = {454, 327, 24, 21};
	int dealer_btn_p2[4] = {243, 313, 35, 26};
	int dealer_btn_p3[4] = {137, 198, 35, 26};
	int dealer_btn_p4[4] = {167, 159, 35, 26};
	int dealer_btn_p5[4] = {272, 111, 35, 26};
	int dealer_btn_p6[4] = {501, 111, 35, 26};
	int dealer_btn_p7[4] = {595, 160, 35, 26};
	int dealer_btn_p8[4] = {622, 195, 35, 26};
	int dealer_btn_p9[4] = {529, 306, 35, 26};

	std::vector<int> player_statuses = {0, 0, 0, 0, 0, 0, 0, 0, 0};


	std::string match_img_to_template(boost::filesystem::path, cv::Mat);
	std::vector<std::string> table_cards{"NA", "NA", "NA", "NA", "NA"};
	std::vector< std::vector<std::string> > player_cards{
		{"NA", "NA"}, {"NA", "NA"}, {"NA", "NA"}, 
		{"NA", "NA"}, {"NA", "NA"}, {"NA", "NA"},
		{"NA", "NA"}, {"NA", "NA"}, {"NA", "NA"}
	};

	std::vector<std::string> player_names{
		"NA", "NA", "NA",
		"NA", "NA", "NA",
		"NA", "NA", "NA"
	};

	std::vector<std::string> player_stacks{
		"NA", "NA", "NA",
		"NA", "NA", "NA",
		"NA", "NA", "NA"
	};

	int dealer_pos = 0;

	std::vector<std::string> player_cards_test{"NA"};
	double pot_size = 0;
	//An array of seats
	//table status e.g phase of game
	//blinds and ante
	//pot
	//dealer chip position


 public:
 	PokerTable(char*, int, Window);

	int table_card_num_1[4] = {266, 180, 12, 18};
	int table_card_suit_1[4] = {265, 199, 14, 17};

	int table_card_num_2[4] = {320, 180, 12, 18};
	int table_card_suit_2[4] = {319, 199, 14, 17};

	int table_card_num_3[4] = {374, 180, 12, 18};
	int table_card_suit_3[4] = {373, 199, 14, 17};

	int table_card_num_4[4] = {428, 180, 12, 18};
	int table_card_suit_4[4] = {427, 199, 14, 17};

	int table_card_num_5[4] = {482, 180, 12, 18};
	int table_card_suit_5[4] = {481 ,199, 14, 17};

	int * table_card_rois[5][2] = { {table_card_num_1, table_card_suit_1}, {table_card_num_2, table_card_suit_2}, {table_card_num_3, table_card_suit_3}, {table_card_num_4, table_card_suit_4}, {table_card_num_5, table_card_suit_5} };
 	cv::Mat table_img;
	std::string a_card_id;

	int exp = 0;

	int * player_card_rois[9][4] = {
		{p1_card_num_L, p1_card_suit_L, p1_card_num_R, p1_card_suit_R},
		{p2_card_num_L, p2_card_suit_L, p2_card_num_R, p2_card_suit_R},
		{p3_card_num_L, p3_card_suit_L, p3_card_num_R, p3_card_suit_R},
		{p4_card_num_L, p4_card_suit_L, p4_card_num_R, p4_card_suit_R},
		{p5_card_num_L, p5_card_suit_L, p5_card_num_R, p5_card_suit_R},
		{p6_card_num_L, p6_card_suit_L, p6_card_num_R, p6_card_suit_R},
		{p7_card_num_L, p7_card_suit_L, p7_card_num_R, p7_card_suit_R},
		{p8_card_num_L, p8_card_suit_L, p8_card_num_R, p8_card_suit_R},
		{p9_card_num_L, p9_card_suit_L, p9_card_num_R, p9_card_suit_R}
	};

	int * player_username_rois[9] = {
		p1_username_ROI, p2_username_ROI, p3_username_ROI,
		p4_username_ROI, p5_username_ROI, p6_username_ROI,
		p7_username_ROI, p8_username_ROI, p9_username_ROI
	};

	int * player_stack_rois[9] = {
		p1_stack_ROI, p2_stack_ROI, p3_stack_ROI,
		p4_stack_ROI, p5_stack_ROI, p6_stack_ROI,
		p7_stack_ROI, p8_stack_ROI, p9_stack_ROI
	};

	int * dealer_btn_rois[9] = {
		dealer_btn_p1, dealer_btn_p2, dealer_btn_p3, 
		dealer_btn_p4, dealer_btn_p5, dealer_btn_p6, 
		dealer_btn_p7, dealer_btn_p8, dealer_btn_p9,  
	};

	bool first_run = true;
	bool first_hand = true;

 	//Setters
 	void set_name(char*);
 	void set_max_seats(int);

 	//Getters
 	void MatType( cv::Mat inputMat );
 	char* get_table_name();
 	void get_table_image(Display*);
 	cv::Mat get_pot_text_img();
 	void set_pot_size();

 	void set_board_cards();
 	std::vector<std::string> get_board_cards();
 	void list_board_cards();


 	void set_player_cards();
 	void set_player_names();
 	void set_player_stacks();

 	void save_table_img(std::string save_path);
 	void save_ROI(int ROI[], std::string);
 	void template_match_test(std::string template_path);
 	cv::Mat get_ROI_mat(int ROI[]);
 	void template_itr(boost::filesystem::path, cv::Mat, std::string);

 	void set_dealer_btn_pos();
 	int get_dealer_btn_pos(){ 
 		return dealer_pos;
 	}

 	void set_active_players();
 	int get_number_active_players();
 	std::string get_game_phase();

 	void new_hand();
 	bool check_for_new_hand();
 	void table_report();
};

#endif