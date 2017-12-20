#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <omp.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <cstdlib>
#include <list>

using namespace std;

#define G_Q_SIZE 100

enum G_TYPE//
{
	G_PALM,
	G_FIST,
	G_PLEASE,
	G_HOLE,
	G_FOUR
};

enum STATE_G
{
	S_NONE,
	S_PALM,
	S_PLEASE,
	S_HOLE,
	S_FOUR,
    S_CLICK,
};

enum STATE_CORD
{
    G_NONE,
    G_FIRST_CORD,
    G_SELECT_CONFIRMING,
};

struct gesture_rec
{
    G_TYPE type;
    double time;
    cv::Rect rect;
    bool is_click;
};

struct gesture_event
{
    cv::Point point_lt;
    cv::Point point_rb;
    cv::Rect rect_select;
};

class gesture
{
public:
    list<gesture_rec> l_gesture;
    gesture_event e_gesture;
    int64 m_tick_s;
    STATE_G m_state_g;
    double m_CLICK_JUDGE;
    STATE_CORD m_state_cord;
    bool is_click;
    double m_time_confirming;
    double m_TIME_CONFIRMING_JUDGE;
    unsigned int m_ANTI_SHAKING_CNT;
    std::vector<cv::Rect> palm;//
    std::vector<cv::Rect> fist;//
	std::vector<cv::Rect> please;//
	std::vector<cv::Rect> hole;//
	std::vector<cv::Rect> four;//

    cv::CascadeClassifier Fist, Palm,Please,Hole,Four;//
	double time_past = 0;
    int frame_cnt = 0;
    int T=5;
    double avg_fps=0;
    int64 s;
    double time_palm=0, time_fist=0, time_please = 0, time_hole = 0, time_four= 0;//

    gesture()//
    {
        Fist.load("fist.xml");
        Palm.load("rpalm.xml");
		Please.load("please.xml");
		Hole.load("hole.xml");
		Four.load("four.xml");
        m_tick_s = cv::getTickCount();//计时
        m_state_g = S_NONE;
        m_CLICK_JUDGE = 0.5;//判断值
        m_state_cord = G_NONE;
        m_TIME_CONFIRMING_JUDGE = 3;//判断用时
        m_ANTI_SHAKING_CNT = 3;//防抖
    }

    ~gesture(){}

    void add(G_TYPE type, cv::Rect* rect);
    STATE_G update_state_g();
    void update_state_cord();
    double get_cur_time();
    bool anti_shaking_state(G_TYPE s);
    cv::Point find_gesture(G_TYPE dst_type);

    void detect(cv::Mat& img);
    bool is_select_confirmed();
    bool is_select_start();
    void clear_select();
    bool is_fist();//
    bool is_palm();//
	bool is_please();//
	bool is_hole();//
	bool is_four();//
    cv::Rect get_fist();//
    cv::Rect get_palm();//
	cv::Rect get_please();//
	cv::Rect get_hole();//
	cv::Rect get_four();//
    cv::Point* get_select_lt();
    cv::Point* get_select_rb();
    double get_avg_fps();
};
