#include "gesture.hpp"

double gesture::get_avg_fps()//帧率
{
    return avg_fps;
}

cv::Rect gesture::get_fist()//获取拳头
{
    return fist[0];
}

cv::Rect gesture::get_palm()//获取手掌
{
    return palm[0];
}

cv::Rect gesture::get_please()
{
	return please[0];
}

cv::Rect gesture::get_hole()
{
	return hole[0];
}

cv::Rect gesture::get_four()
{
	return four[0];
}

void gesture::detect(cv::Mat& img)//手势检测
{
    int64 start = cv::getTickCount();

#pragma omp parallel sections
	{
#pragma omp section
		{
			s = cv::getTickCount();//开始识别时间
			Palm.detectMultiScale(img, palm, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(60, 60), cv::Size(80, 80));//手掌检测用时
			time_palm += (cv::getTickCount() - s) / cv::getTickFrequency();//识别用时
}
#pragma omp section
{
		
		
			s = cv::getTickCount();
			Fist.detectMultiScale(img, fist, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(40, 40), cv::Size(45, 45));
			time_fist += (cv::getTickCount() - s) / cv::getTickFrequency();
		}

#pragma omp section
		{
		
		
			s = cv::getTickCount();
			Please.detectMultiScale(img, please, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(60, 60), cv::Size(90, 90));
			time_please += (cv::getTickCount() - s) / cv::getTickFrequency();
		}

#pragma omp section
		{
			s = cv::getTickCount();
			Hole.detectMultiScale(img, hole, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(50, 50), cv::Size(60, 60));
			time_hole += (cv::getTickCount() - s) / cv::getTickFrequency();
}
#pragma omp section
{
		

		
			s = cv::getTickCount();
			Four.detectMultiScale(img, four, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(60, 60), cv::Size(80, 80));
			time_four += (cv::getTickCount() - s) / cv::getTickFrequency();
		}
	}

        if(is_fist())
                add(G_FIST, &(fist[0]));
        if(is_palm())
                add(G_PALM, &(palm[0]));
	if (is_please())
		add(G_PLEASE, &(please[0]));
	if (is_hole())
		add(G_HOLE, &(hole[0]));
	if (is_four())
		add(G_FOUR, &(four[0]));

    frame_cnt++;
    time_past += (cv::getTickCount() - start) / cv::getTickFrequency();//处理总用时
    if(time_past >= T)
    {
        avg_fps = (double)frame_cnt / time_past;//平均帧率
        printf("average fps:%3.2f, time_palm:%f ms, time_fist:%f ms in %d second\n", avg_fps, time_palm/frame_cnt*1000, time_fist/frame_cnt*1000, T);//显示处理时间
		printf("average fps:%3.2f, time_please:%f ms,in %d second\n", avg_fps, time_please / frame_cnt * 1000, T);//显示处理时间
		printf("average fps:%3.2f, time_hole:%f ms, time_four:%f ms in %d second\n", avg_fps, time_hole / frame_cnt * 1000, time_four / frame_cnt * 1000, T);//显示处理时间
		printf("\n");
		frame_cnt = 0;
        time_past = 0;
        time_palm = 0;
		time_fist = 0;
        time_please = 0;
		time_hole = 0;
		time_four = 0;
    }
}

bool gesture::is_fist()
{
    return fist.size() == 1;
}

bool gesture::is_palm()
{
    return palm.size() == 1;
}

bool gesture::is_please()
{
	return please.size() == 1;
}

bool gesture::is_hole()
{
	return hole.size() == 1;
}

bool gesture::is_four()
{
	return four.size() == 1;
}

bool gesture::is_select_start()
{
    return G_FIRST_CORD == m_state_cord;
}


cv::Point* gesture::get_select_lt()
{
    return &(e_gesture.point_lt);
}

cv::Point* gesture::get_select_rb()
{
    return &(e_gesture.point_rb);
}

cv::Point gesture::find_gesture(G_TYPE dst_type)
{
    list<gesture_rec>::reverse_iterator it;//反向迭代器
  
    return {0,0};
}

bool gesture::anti_shaking_state(G_TYPE s)//防抖
{
    if(l_gesture.size()<m_ANTI_SHAKING_CNT)
        return false;
    list<gesture_rec>::reverse_iterator it;
    for(unsigned int i=0;i<m_ANTI_SHAKING_CNT;i++)
    {
        it = l_gesture.rbegin();
        if(it->type != s)
            return false;
        it++;
    }
    return true;
}

bool gesture::is_select_confirmed()//确认选择
{
    double cur_time = get_cur_time();
    if(G_SELECT_CONFIRMING == m_state_cord && cur_time - m_time_confirming > m_TIME_CONFIRMING_JUDGE)//识别时间
        return 1;
    return 0;
}

void gesture::clear_select()//清除选择
{
    m_state_cord = G_NONE;
}



double gesture::get_cur_time()//用时
{
    return (cv::getTickCount() - m_tick_s) / cv::getTickFrequency();
}

void gesture::add(G_TYPE type, cv::Rect* rect)//
{
    gesture_rec rec;
    rec.type = type;
    rec.time = get_cur_time();
    rec.rect = *rect;
    l_gesture.push_back(rec);
    if (l_gesture.size() > G_Q_SIZE)
    {
        l_gesture.pop_front();
    }
    update_state_g();
}

STATE_G gesture::update_state_g()//更新状态G
{
    G_TYPE type;
    double time=1000;
    list<gesture_rec>::reverse_iterator it = l_gesture.rbegin();
    type = it->type;

    if(l_gesture.size()>=2)
    {
        time = it->time;
        it++;
        time -= it->time;
    }

    if(S_NONE == m_state_g && G_PALM == type && anti_shaking_state(G_PALM))
    {
        m_state_g = S_PALM;
    }
	else if (S_NONE == m_state_g && G_PLEASE == type && anti_shaking_state(G_PLEASE))
	{
		m_state_g = S_PLEASE;
	}
	else if (S_NONE == m_state_g && G_HOLE == type && anti_shaking_state(G_HOLE))
	{
		m_state_g = S_HOLE;
	}
	else if (S_NONE == m_state_g && G_FOUR == type && anti_shaking_state(G_FOUR))
	{
		m_state_g = S_FOUR;
	}
    else if(S_PALM == m_state_g &&((G_FIST == type && anti_shaking_state(G_FIST))||( G_PLEASE == type && anti_shaking_state(G_PLEASE))||(G_HOLE == type && anti_shaking_state(G_HOLE))|| (G_FOUR == type && anti_shaking_state(G_FOUR))))
	{
        if(time < m_CLICK_JUDGE)
            m_state_g = S_CLICK;
        else
            m_state_g = S_NONE;        
    }
	else if (S_PLEASE == m_state_g && ((G_FIST == type && anti_shaking_state(G_FIST))|| (G_PALM == type && anti_shaking_state(G_PALM)) ||
		(G_HOLE == type && anti_shaking_state(G_HOLE)) || (G_FOUR == type && anti_shaking_state(G_FOUR))))
	{
		if (time < m_CLICK_JUDGE)
			m_state_g = S_CLICK;
		else
			m_state_g = S_NONE;
	}
	else if (S_HOLE == m_state_g && ((G_FIST == type && anti_shaking_state(G_FIST))|| (G_PALM == type && anti_shaking_state(G_PALM)) ||
		(G_PLEASE == type && anti_shaking_state(G_PLEASE)) || (G_FOUR == type && anti_shaking_state(G_FOUR))))
	{
		if (time < m_CLICK_JUDGE)
			m_state_g = S_CLICK;
		else
			m_state_g = S_NONE;
	}
	else if (S_FOUR == m_state_g && ((G_FIST == type && anti_shaking_state(G_FIST))|| (G_PALM == type && anti_shaking_state(G_PALM)) ||
		(G_HOLE == type && anti_shaking_state(G_HOLE)) || (G_PLEASE == type && anti_shaking_state(G_PLEASE))))
	{
		if (time < m_CLICK_JUDGE)
			m_state_g = S_CLICK;
		else
			m_state_g = S_NONE;
	}
    else if(S_CLICK == m_state_g)
        m_state_g = S_NONE;


    if(S_CLICK == m_state_g)
    {
        l_gesture.rbegin()->is_click = (m_state_g == S_CLICK);
    }

    update_state_cord();

    return m_state_g;
}

void gesture::update_state_cord()
{
    if(G_NONE == m_state_cord && S_CLICK == m_state_g)
    {
        m_state_cord = G_FIRST_CORD;
    }
    else if(G_FIRST_CORD == m_state_cord && S_CLICK == m_state_g)
    {
        e_gesture.rect_select.x = e_gesture.point_lt.x;
        e_gesture.rect_select.y = e_gesture.point_lt.y;
        e_gesture.rect_select.width = e_gesture.point_rb.x - e_gesture.point_lt.x;
        e_gesture.rect_select.height = e_gesture.point_rb.y - e_gesture.point_lt.y;
        m_state_cord = G_SELECT_CONFIRMING;
        m_time_confirming = l_gesture.rbegin()->time;
    }
    else if(G_SELECT_CONFIRMING == m_state_cord)
    {
    }
}
