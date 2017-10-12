#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <tesseract/baseapi.h>
#include <opencv2/text.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdlib.h>     /* malloc, calloc, realloc, free */
#include <list>
#include <typeinfo>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <cstring>
#include <vector>
#include "table_class.h"
#include <stdlib.h>     //for using the function sleep
#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED 
#include <boost/filesystem.hpp>
#include <pokerstove/peval/CardSet.h>
#include <pokerstove/peval/HoldemHandEvaluator.h>
#include <pokerstove/penum/CardDistribution.h>
#include <pokerstove/penum/ShowdownEnumerator.h>
#include <fstream>
#include "sys/types.h"
#include "sys/sysinfo.h"
#include <unistd.h>
#include "pokerstove/penum/SimpleDeck.hpp"
#include "pokerstove/penum/Odometer.h"
#include "pokerstove/penum/PartitionEnumerator.h"

//! A function
/*! Takes a display type and returns all open windows in the display. */
Window *getAllWindowsList (Display *disp, unsigned long *len) {
    Atom prop = XInternAtom(disp,"_NET_CLIENT_LIST",False), type;
    int form;
    unsigned long remain;
    unsigned char *list;

    if (XGetWindowProperty(disp,XDefaultRootWindow(disp),prop,0,1024,False,XA_WINDOW,
                &type, &form, len, &remain, &list) != Success) {
        return 0;
    }  

    return (Window*)list;
}

//! A function
/*! Takes display type and Window type and returns the name of the window*/
char *getWindowName (Display *disp, Window win) {
    Atom prop = XInternAtom(disp,"WM_NAME",False), type;
    int form;
    unsigned long remain, len;
    unsigned char *winName;

    if (XGetWindowProperty(disp, win, prop, 0, 1024, False, XA_STRING,
                &type, &form, &len, &remain, &winName) != Success) {

        return NULL;
    }
  	return (char*)winName;
}

//! A function
/*! Takes a display type and character type reference to the name of the windows we want to extract.
    For example: char poker_name_ref[] = "No Limit Hold'em";
*/
std::list<Window> getPokerWindowIDs(Display *disp, char *poker_name_ref)
{
    int i;
    unsigned long len;
    char *name;
    std::list<char*> poker_window_name_list;
    std::list<Window> poker_windows_ID_list;
    Window *all_windows_list;
    all_windows_list = (Window*)getAllWindowsList(disp, &len);

    for (i=0;i<(int)len;i++) {
      	name = getWindowName(disp, all_windows_list[i]);

      	if(strstr((char*)name, poker_name_ref)){
      		poker_windows_ID_list.push_front(all_windows_list[i]);
	      }
	    //free(name);
    }
    return poker_windows_ID_list;
}

//  A function
/*! Takes the display and window and returns an image of the window in the opencv Mat type.
    
    TODO - Document mat type returned,
*/
cv::Mat getWindowImage(Display *disp, Window win)
{
	using namespace cv;
	std::vector<uint8_t> Pixels;

	XWindowAttributes attributes = {0};
    XGetWindowAttributes(disp, win, &attributes);

	int& Width = attributes.width;
	int& Height = attributes.height;
	XImage* img = XGetImage(disp, win, 0, 0, Width, Height, AllPlanes, ZPixmap);

	//std::cout << (Width) << ", " <<  (Height) << std::endl;

	int& BitsPerPixel = img->bits_per_pixel;
    Pixels.resize(Width * Height * 4);
    memcpy(&Pixels[0], img->data, Pixels.size());

    cv::Mat mat_img = Mat(Height, Width, BitsPerPixel > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]); //Mat(Size(Height, Width), Bpp > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]); 

    //namedWindow("WindowTitle", WINDOW_AUTOSIZE);
    //imshow("Display window", mat_img);
    //waitKey(0);
    char *win_name =  getWindowName(disp, win);

    //std::cout << win_name << std::endl;

    return mat_img.clone();
}


//! A function
/*! Takes display and window types. Raises the specified window to the foreground of GUI */
void windowRaiser(Display* disp, Window win)
{
	XRaiseWindow(disp, win);
	XEvent event = { 0 };
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = XInternAtom( disp, "_NET_ACTIVE_WINDOW", False);
    event.xclient.window = win;
    event.xclient.format = 32;
	XSendEvent( disp, win, False, SubstructureRedirectMask | SubstructureNotifyMask, &event );

}


//! A function
/*! Draws a rectangle on the GUI. This function is not being used at the moment*/
void draw_rectangle(cv::Mat &frame) {
	using namespace cv;

	cv::rectangle(frame, cv::Point(199, 52), cv::Point(356, 76),  cvScalar(0, 255, 0));
	cv::rectangle(frame, cv::Point(199, 78), cv::Point(356, 102),  cvScalar(255, 0, 0));

	namedWindow("WindowTitle", WINDOW_AUTOSIZE);
    imshow("Display window", frame);
    imwrite("test.jpg", frame);
    waitKey(0);
}

//! A function
/*! Takes a vector containing type PokerTable. The function iterates over the vector and  */
void getOpenTables (std::vector<PokerTable*> tablesVect) {
	for (std::vector<PokerTable*>::const_iterator i = tablesVect.begin(); i!= tablesVect.end(); i++)
		(*i)-> get_table_name();
}

//! A function
/*! An example of the Showdown_enum from pokerstove. I don't think this will be included in release. */
void Showdow_enum_example() {
    using namespace pokerstove;
    using namespace std;
    CardSet completeDeck;
    completeDeck.fill();
    cout << "The whole deck has " << completeDeck.size() << " cards" << endl;

    CardDistribution anyTwo;
    anyTwo.fill(completeDeck, 2);
    cout << "There are " << anyTwo.size() << " two card combinations"  << endl;

    //CardDistribution anotherTwo;
    //anotherTwo.fill(completeDeck, 2);

    CardDistribution holeCards;
    holeCards.parse("As6s");


    ShowdownEnumerator showdown;
    vector<EquityResult> result = showdown.calculateEquity(
        vector<CardDistribution>{anyTwo, holeCards},
        CardSet("Ad7d9S"),
        PokerHandEvaluator::alloc("h")
    );

    double shareRandom = result.at(0).winShares + result.at(0).tieShares;
    double shareHand   = result.at(1).winShares + result.at(1).tieShares;
    double total       = shareRandom + shareHand;

    cout << "A random hand has "  << shareRandom / total * 100  << " % equity (" << result.at(0).str() << ")" << endl;    
    cout << "The hand As6s has "  << shareHand   / total * 100  << " % equity (" << result.at(1).str() << ")" << endl;

    time_t timestamp = time(NULL);

    cout << to_string(timestamp) << endl;
}

//! A function
/*! I've forgotten what this actually does, but will leave it here for now. It's just a copy from the pokerstove example */
void shadowEnumCopy()     
{
    using namespace pokerstove;
    using namespace std;

    //Bits that need including
    CardSet completeDeck;
    completeDeck.fill();
    cout << "The whole deck has " << completeDeck.size() << " cards" << endl;

    CardDistribution holeCards;
    holeCards.parse("As6s");

    CardDistribution anyTwo;
    anyTwo.fill(completeDeck, 2);
    cout << "There are " << anyTwo.size() << " two card combinations"  << endl;

    CardSet board = CardSet("Ad7d9s");
    boost::shared_ptr<PokerHandEvaluator> peval = {PokerHandEvaluator::alloc("h")};

    vector<CardDistribution> dists = {anyTwo, holeCards};
    //End

    if (peval.get() == NULL)
        throw runtime_error("ShowdownEnumerator, null evaluator");
    assert(dists.size() > 1);
    const size_t ndists = dists.size();
    vector<EquityResult> results(ndists, EquityResult());
    size_t handsize = peval->handSize();

    // the dsizes vector is a list of the sizes of the player hand
    // distributions
    vector<size_t> dsizes;
    for (size_t i=0; i<ndists; i++)
    {
        assert(dists[i].size() > 0);
        dsizes.push_back (dists[i].size());
    }

    // need to figure out the board stuff, we'll be rolling the board into
    // the partitions to make enumeration easier down the line.
    size_t nboards = 0;
    size_t boardsize = peval->boardSize();
    if (boardsize > 0)
        nboards++;

    // for the most part, these are allocated here to avoid contant stack
    // reallocation as we cycle through the inner loops
    SimpleDeck deck;
    CardSet dead;
    double weight;
    vector<CardSet>             ehands         (ndists+nboards);
    vector<size_t>              parts          (ndists+nboards);
    vector<CardSet>             cardPartitions (ndists+nboards);
    vector<PokerHandEvaluation> evals          (ndists);         // NO BOARD

    // copy quickness
    CardSet * copydest = &ehands[0];
    CardSet * copysrc = &cardPartitions[0];
    size_t ncopy = (ndists+nboards)*sizeof(CardSet);
    Odometer o(dsizes);
    do
    {
        // colect all the cards being used by the players, skip out in the
        // case of card duplication
        bool disjoint = true;
        dead.clear ();
        weight = 1.0;
        for (size_t i=0; i<ndists+nboards; i++)
        {
            if (i<ndists)
            {
                cardPartitions[i] = dists[i][o[i]];
                parts[i]          = handsize-cardPartitions[i].size();
                weight           *= dists[i][cardPartitions[i]];
            }
            else
            {
                // this allows us to have board distributions in the future
                cardPartitions[i] = board;
                parts[i]          = boardsize-cardPartitions[i].size(); //parts is the number of size of board - supplied board cards
                cout << parts[i] << endl;
            }
            disjoint = disjoint && dead.disjoint(cardPartitions[i]);
            dead |= cardPartitions[i];
        }

        if (disjoint)
        {
            deck.reset ();
            deck.remove (dead);
            PartitionEnumerator2 pe(deck.size(), parts);
            cout << deck.size() << endl;
            do
            {
                // we use memcpy here for a little speed bonus
                memcpy (copydest, copysrc, ncopy);
                for (size_t p=0; p<ndists+nboards; p++)
                    ehands[p] |= deck.peek(pe.getMask (p));

                // TODO: do we need this if/else, or can we just use the if
                // clause? A: need to rework tracking of whether a board is needed
                if (nboards > 0)
                {
                    peval->evaluateShowdown (ehands, ehands[ndists], evals, results, weight);
                    cout << ehands[ndists] << endl;
                }
                else
                    peval->evaluateShowdown (ehands, board, evals, results, weight);
            }
            while (pe.next ());
        }
    }
    while (o.next ());

}


int parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

//! A function
/*! A diagnostic tool, returns memory usage of the program*/
int getMemoryValue(){ //Note: this value is in KB!
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmSize:", 7) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
}

//! A function
/*! Work in progress template for how to run...*/
void standard_run(PokerTable * table, Display * disp){
    using namespace std; 
    /*
    if (table -> first_run == true) {
        table -> get_table_image(disp);
        table -> set_dealer_btn_pos();
        table -> first_run = false;
    }
    */
    //Wait for button to maiove before starting run
    /*
    if (table -> first_hand == true) {
        table -> get_table_image(disp);

        if (table -> check_for_new_hand()) {
            cout << "starting first recording..." << endl;
            table -> first_hand = false;
        }
        return;
    }
    */
    //tablesVector[table_num] -> save_table_img(); 

    table -> get_table_image(disp);

    /*
    if (table -> check_for_new_hand()) {  //Resets player variables if there is a new hand. Think about adding in set_player_names here? Also this is when we want to write the information to sql?
        cout << "submitting table report" << endl;
        table -> table_report();
        table -> new_hand();
        table -> set_player_names();
    }
    */
    //tablesVector[table_num] -> save_table_img(table_imgs_path);
    //tablesVector[table_num] -> save_ROI(dealer_p1, dealer_btn_path + "dealer");
    //int active_players = tablesVector[table_num] -> get_number_active_players();
    
    table -> set_active_players();
    table -> set_player_cards();
    table -> set_board_cards();
    table -> set_player_stacks();
    table -> set_pot_size();


    //std::string game_phase = tablesVector[table_num] -> get_game_phase();
    //tablesVector[table_num] -> list_board_cards();
    //tablesVector[table_num] -> exp +=1;
    //cout << tablesVector[table_num] -> exp << endl;
}
       //sleep(1);


int main(int argc, char* argv[]) {
	using namespace std;
	using namespace cv;
    using namespace cv::text;

    char *name;
	char poker_window_str_identifier[] = "No Limit Hold'em";
	Display* disp = XOpenDisplay(NULL);
	std::list<Window> poker_windows_id_list = getPokerWindowIDs(disp, poker_window_str_identifier);
	std::vector<PokerTable*> tablesVector;

	//Set maximum number of playes at the table. Important for ROI's
	const int max_seats = 9;

	for (Window win : poker_windows_id_list) {
		//namedWindow("WindowTitle", WINDOW_AUTOSIZE); 
		//imshow("Display window", window_img);
		//waitKey(0);
		//draw_rectangle(window_img);
		name = getWindowName(disp, win);
		PokerTable table_1 (name, max_seats, win);
		table_1.get_table_name();
		tablesVector.push_back(new PokerTable(name, max_seats, win));
  	}

  	getOpenTables(tablesVector);

    int x = 2;
    namedWindow("Display window", WINDOW_AUTOSIZE);

    std::string path = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/p_cards/";
    std::string heart_path = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/suit/s.png";
    boost::filesystem::path dir_nums = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/num/";
    std::string  all_nums = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/num_investig/all_ROI/";
    std::string  dealer_btn_path = "/home/behzad/Documents/cPlusPlus_projects/poker/cardImages/cropped_btn/";
    std::string table_imgs_path = "/home/behzad/Documents/cPlusPlus_projects/poker/tableImages/save_";
    
    shadowEnumCopy();
    cout << "end" << endl;
    while (x < 2)
    {
        for (unsigned int table_num = 0; table_num < tablesVector.size(); ++table_num)
        {
            //standard_run(tablesVector[table_num], disp);
        }
        x++;
        cout << getMemoryValue()/pow(10,6) << endl;
    }
}




    /*
    while (x < 200)
    {
        for (unsigned int table_num = 0; table_num < tablesVector.size(); ++table_num)
        {
            //Wait set the current dealer_button position

            if (tablesVector[table_num] -> first_run == true) {
                tablesVector[table_num] -> get_table_image(disp);
                tablesVector[table_num] -> set_dealer_btn_pos();
                tablesVector[table_num] -> first_run = false;
            }

            //Wait for button to move before starting run
            if (tablesVector[table_num] -> first_hand == true) {
                tablesVector[table_num] -> get_table_image(disp);

                if (tablesVector[table_num] -> check_for_new_hand()) {
                    cout << "starting first recording..." << endl;
                    tablesVector[table_num] -> first_hand = false;
                }
                continue;
            }

            //tablesVector[table_num] -> save_table_img(); 

            tablesVector[table_num] -> get_table_image(disp);

            if (tablesVector[table_num] -> check_for_new_hand()) {  //Resets player variables if there is a new hand. Think about adding in set_player_names here? Also this is when we want to write the information to sql?
                cout << "submitting table report" << endl;
                tablesVector[table_num] -> table_report();
                tablesVector[table_num] -> new_hand();
                tablesVector[table_num] -> set_player_names();
            }

            //tablesVector[table_num] -> save_table_img(table_imgs_path);
            //tablesVector[table_num] -> save_ROI(dealer_p1, dealer_btn_path + "dealer");
            //int active_players = tablesVector[table_num] -> get_number_active_players();
            
            tablesVector[table_num] -> set_active_players();
            tablesVector[table_num] -> set_player_cards();
            tablesVector[table_num] -> set_board_cards();
            tablesVector[table_num] -> set_player_stacks();
            tablesVector[table_num] -> set_pot_size();
            //std::string game_phase = tablesVector[table_num] -> get_game_phase();
            //tablesVector[table_num] -> list_board_cards();
            //tablesVector[table_num] -> exp +=1;
            //cout << tablesVector[table_num] -> exp << endl;
            x++;
        }
       //sleep(1);
    }

}
*/
//
