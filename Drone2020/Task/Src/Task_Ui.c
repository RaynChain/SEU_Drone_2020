#include "Task_UI.h"
#include "Task_Judge.h"
#include "Task_StateMachine.h"

#define TASK_UI_INTERVAL (500)
#define bullet_remaining_bar_length   500 / bullet_max * (uint32_t)ext_bullet_remaining.bullet_remaining_num

//需要一个数组记录当前有多少图形要画！！！！！ 3.15晚上

void Task_Ui(void *parameters)
{
    InitPeripheral_UI();
    TickType_t xLastWakeUpTime;
    xLastWakeUpTime = xTaskGetTickCount();
    while (1)
    {
			FrameUpdata();
			vTaskDelayUntil(&xLastWakeUpTime, TASK_UI_INTERVAL);
    }
}

/* --------------------------   变量定义    ----------------------------------*/
ext_student_interactive_header_data_t header_data;
/*
typedef __packed struct 
{   
  uint16_t data_cmd_id;    
  uint16_t sender_ID;    
  uint16_t receiver_ID; 
}ext_student_interactive_header_data_t;
*/
robot_interactive_data_t robot_interactive_data;
/*
typedef __packed struct 
{ 
  uint8_t* data;
} robot_interactive_data_t;//交互数据
*/

graphic_data_struct_t graphic_data[GRAPHIC_NUM];
ext_student_interactive_header_data_t header_data;//交互数据接收信息
robot_interactive_data_t robot_interactive_data;//交互数据
ext_client_custom_character_t custom_character;//用户绘制字符

uint8_t* shoot_status_chars;//UI发送射击状态用

static uint8_t send_seq; //包序号

/* --------------------------   函数定义    ----------------------------------*/
/**
 * @brief  数据帧内容更新
 * @note   需要画什么，怎么画在这里面更新
 * @retval 0 不成功，此时不发送 1：更新成功
 */
uint8_t FrameUpdata(void)
{
	static uint8_t FirstDraw = 1;//判断是初始化UI界面还是更新UI界面
	uint8_t* uname;//uint8_t数组，必须三字节
	
	if(FirstDraw == 1)
	{
		uname = (unsigned char*)"apv";//准心竖直部分aim_point_vertical
		draw_line(1, 960, 540, 960, 405, uname, 12, 6, COLOR_GREEN);//竖直长度100，宽度5，图层6，绿色，有待调整
		uname = (unsigned char*)"aph";//准心水平部分aim_point_horizontal
		draw_line(1, 855, 405, 1065, 405, uname, 12, 6, COLOR_GREEN);//水平长度60，宽度5，图层6，绿色，有待调整
		
		uname = (unsigned char*)"epb";//能量点背景energy_point_background
		draw_rect(1, 890, 918, 1030, 848, uname, 12, 5, COLOR_BLACK);//图层5，黑色，矩形是实心的吗？有待调整。
		uname = (unsigned char*)"epv";//能量点数值energy_point_value
		write_chars(1, 890, 918, 1030, 848, uname, 1, 10, 7, COLOR_WHITE, energy_point_c, 2);//线宽1，字体10，图层7， 白色，有待调整
		
		uname = (unsigned char*)"atb";//攻击时间背景attack_time_background
		draw_rect(1, 1240, 517, 1380, 447, uname, 12, 5, COLOR_BLACK);//有待调整。
		uname = (unsigned char*)"atv";//攻击时间数值attack_time_value
		write_chars(1, 1240, 517, 1380, 447, uname, 1, 10, 7, COLOR_WHITE, &aerial_robot_energy.attack_time,1);//有待调整
		
		uname = (unsigned char*)"brb";//剩余子弹数目背景bullet_remaining_num_background
		draw_rect(1, 1198, 297, 1338, 227, uname, 12, 5, COLOR_BLACK);//有待调整。
		uname = (unsigned char*)"brv";//剩余子弹数目数值bullet_remaining_num_value
		write_chars(1, 1198, 297, 1338, 227, uname, 1, 10, 7, COLOR_WHITE, bullet_remaining_num_c, 2);
		
		/*uname = (unsigned char*)"lsg";//外观灯状态背景led_status_background
		draw_rect(582, 297, 722, 227, uname, 12, 5, COLOR_BLACK);//有待调整。
		uname = (unsigned char*)"lsv";//外观灯状态数值led_status_value
		write_chars(1, 582, 297, 722, 227, uname, 1, 10, 7, COLOR_WHITE, ??? , 2);*/
		
		/*uname = (unsigned char*)"asb";//自瞄状态背景auto_status_background
		draw_rect(1198, 337, 1338, 367, uname, 12, 5, COLOR_BLACK);//有待调整。
		uname = (unsigned char*)"asv";//自瞄状态数值auto_status_value
		write_chars(1, 1198, 337, 1338, 367, uname, 1, 10, 7, COLOR_WHITE, ???, 2);*/
		
		send_graphic();//七个图形已满，调用一次send_graphic()
		
		uname = (unsigned char*)"ssb";//射击状态背景shoot_status_background
		draw_rect(1, 1198, 477, 1338, 507, uname, 12, 5, COLOR_BLACK);//有待调整。
		Get_Shoot_Status();
		uname = (unsigned char*)"ssv";//射击状态数值shoot_status_value
		write_chars(1, 1198, 477, 1338, 507, uname, 1, 10, 7, COLOR_WHITE, shoot_status_chars, 12);
		
		uname = (unsigned char*)"brb";//剩余子弹数目进度条bullet_remaining_num_bar，最大x长度610-1110
		draw_line(1, 610, 1273, 610, 1263, uname, 10, 7, COLOR_CYAN);//有待调整
    uname = (unsigned char*)"bsb";//发射子弹数目进度条bullet_shot_num_bar
		draw_line(1, 1110, 1273, 1110, 1263, uname, 10, 7, COLOR_YELLOW);
		
		send_graphic();//三个图形，调用一次send_graphic()
		
		FirstDraw = 0;
	}
	
	uname = (unsigned char*)"epv";//更新能量点数值
	write_chars(2, 890, 918, 1030, 848, uname, 1, 10, 7, COLOR_WHITE, energy_point_c, 2);//有待调整
	
	uname = (unsigned char*)"atv";//更新攻击时间数值
	write_chars(2, 1240, 517, 1380, 447, uname, 1, 10, 7, COLOR_WHITE, &aerial_robot_energy.attack_time,1);//有待调整
	
	uname = (unsigned char*)"brv";//更新剩余子弹数目数值
	write_chars(2, 1198, 297, 1338, 227, uname, 1, 10, 7, COLOR_WHITE, bullet_remaining_num_c, 2);//有待调整
	
	/*uname = (unsigned char*)"lsv";//更新外观灯状态数值
	write_chars(2, 582, 297, 722, 227, uname, 1, 10, 7, COLOR_WHITE, ??? , 2);*/
	
	/*uname = (unsigned char*)"asv";//更新自瞄状态数值
	write_chars(2, 1198, 337, 1338, 367, uname, 1, 10, 7, COLOR_WHITE, ???, 2);*/
	
	Get_Shoot_Status();//更新射击状态数值
	uname = (unsigned char*)"ssv";
	write_chars(2, 1198, 477, 1338, 507, uname, 1, 10, 7, COLOR_WHITE, shoot_status_chars, 12);
	
	uname = (unsigned char*)"brb";//更新剩余子弹数目进度条bullet_remaining_num_bar
	draw_line(1, 610, 1273, 610 + bullet_remaining_bar_length, 1263, uname, 10, 7, COLOR_CYAN);//有待调整
	uname = (unsigned char*)"bsb";//更新发射子弹数目进度条bullet_shot_num_bar
	draw_line(1, 1110 - bullet_remaining_bar_length, 1273, 1110, 1263, uname, 10, 7, COLOR_YELLOW);
	
	send_graphic();//两个图形，调用一次send_graphic()
	
    if(header_data.sender_ID == 0 || header_data.receiver_ID == 0)
    {
        if(InitPeripheral_UI() == 0)
        {
            return 0;
        }
    }


    return 1;
}

/**
 * @brief 初始化UI
 * @note
 * @retval
 */
uint8_t InitPeripheral_UI(void)
{
    //这个封装我没试过，可能有问题，开学了试试
    /*
    UiFrame.Frame.frame_header.Frame.SOF = 0XA5;
    UiFrame.Frame.frame_header.Frame.seq = 0;
    UiFrame.Frame.frame_header.Frame.data_length.Frame = 0;
		*/
		/*
		|SOF1|data_length2|seq1|CRC81|
		|frame_header-----5----------|cmd_id 2|data n|
		|UiFrame_t(UiFrameBuffer[128])-----------|
	  */
    UiFrame.UiFrameBuffer[0] = 0XA5;
	  UiFrame.Frame.frame_header.Frame.seq = 0;//包序号初始化
    UiFrame.Frame.frame_header.Frame.data_length.Frame = 0;//数据长度初始化
	  UiFrame.Frame.frame_header.Frame.CRC8 = 0;//CRC校验初始化
    UiFrame.UiFrameBuffer[5] = 0X01;
    UiFrame.UiFrameBuffer[6] = 0X03;//cmd_id设置为0x0301机器人间交互
	  memset(UiFrame.Frame.data, 0, 121);//数据初始化

    header_data.sender_ID = ext_game_robot_state.robot_id;//发送方id

    switch (WhichTeam())//接收方id
    {
    case 0:
        /* code */
        return 0;

    case 1: // robotID 1~7  clientID 0x0101~0x0107
        header_data.receiver_ID = (ext_game_robot_state.robot_id | 0x0100);
        break;
    case 2: // robotID 101~107  clientID 0x0165~0x016A
        header_data.receiver_ID = ext_game_robot_state.robot_id + 256;
        break;
    default:
        return 0;

    }

    return 1;
}


/**
 * @brief  画直线
 * @note   调用该函数画直线(我现在没检查坐标，因为我不知道多少是越界，用的时候自己注意一下)
 * @param  operate: 需要进行的操作类型，1-增加，2-修改
 * @param  start_x: 直线起始x坐标
 * @param  start_y: 直线起始y坐标
 * @param  end_x: 直线结束x坐标
 * @param  end_y: 直线结束y坐标
 * @param  name:  图形名字(必须3个uint8_t，不够记得补零，实在懒得写判断了....)
 * @param  width: 线宽
 * @param  layer: 图层
 * @param  color: 颜色
 * @retval 1:配置正确  0:所有graphic已经堆满  2:坐标错误 3:图层错误 4:颜色错误 
 */
uint8_t draw_line(uint32_t operate, uint32_t start_x,uint32_t start_y,uint32_t end_x,uint32_t end_y,uint8_t name[],uint32_t width,uint8_t layer,uint8_t color)
{
    uint8_t index;
    index = check_empty_graphic();

    if(index >= GRAPHIC_NUM)
    {
        return 0;
    }

    graphic_data[index].graphic_name[0] = name[0];
    graphic_data[index].graphic_name[1] = name[1];
    graphic_data[index].graphic_name[2] = name[2];

		switch (operate)
		{
			case 1:
			{
				graphic_data[index].operate_tpye = OPERATE_ADD;
				break;
			}
			case 2:
			{
				graphic_data[index].operate_tpye = OPERATE_CHANGE;
				break;
			}
		}
		
    graphic_data[index].width = width;
    graphic_data[index].graphic_tpye = GRAPHIC_LINE;
    
    graphic_data[index].start_x = start_x;
    graphic_data[index].start_y = start_y;
    graphic_data[index].end_x = end_x;
    graphic_data[index].end_y = end_y;

    if(layer > LAYER_NUM)
    {
        return 3;
    }
    else
    {
        graphic_data[index].layer = layer;
    }
        
    if(color >= COLOR_NUM)
    {
        return 4;
    }
    else
    {
        graphic_data[index].color = color;
    }
		return 1;
}

/**
 * @brief  画矩形
 * @note   调用该函数画矩形(没检查坐标)
 * @param  operate: 需要进行的操作类型，1-增加，2-修改
 * @param  start_x: 起始x坐标
 * @param  start_y: 起始y坐标
 * @param  end_x: 结束x坐标
 * @param  end_y: 结束y坐标
 * @param  name:  图形名字(必须3个uint8_t，不够记得补零，实在懒得写判断了....)
 * @param  width: 线宽
 * @param  layer: 图层
 * @param  color: 颜色
 * @retval 1:配置正确  0:所有graphic已经堆满  2:坐标错误 3:图层错误 4:颜色错误 
 */
uint8_t draw_rect(uint32_t operate, uint32_t start_x,uint32_t start_y,uint32_t end_x,uint32_t end_y,uint8_t name[],uint32_t width,uint8_t layer,uint8_t color)
{
    uint8_t index;
    index = check_empty_graphic();

    if(index >= GRAPHIC_NUM)
    {
        return 0;
    }

    graphic_data[index].graphic_name[0] = name[0];
    graphic_data[index].graphic_name[1] = name[1];
    graphic_data[index].graphic_name[2] = name[2];

		switch (operate)
		{
			case 1:
			{
				graphic_data[index].operate_tpye = OPERATE_ADD;
				break;
			}
			case 2:
			{
				graphic_data[index].operate_tpye = OPERATE_CHANGE;
				break;
			}
		}
		
    graphic_data[index].width = width;
    graphic_data[index].graphic_tpye = GRAPHIC_RECT;
    
    graphic_data[index].start_x = start_x;
    graphic_data[index].start_y = start_y;
    graphic_data[index].end_x = end_x;
    graphic_data[index].end_y = end_y;

    if(layer > LAYER_NUM)
    {
        return 3;
    }
    else
    {
        graphic_data[index].layer = layer;
    }
        
    if(color >= COLOR_NUM)
    {
        return 4;
    }
    else
    {
        graphic_data[index].color = color;
    }   
		return 1;
}

/**
 * @brief  通过这个函数来配置写字的帧，实现增加或修改的功能
 * @note   调用该函数写字(没检查坐标)
 * @param  operate: 需要进行的操作类型，1-增加，2-修改
 * @param  start_x: 字符起始x坐标
 * @param  start_y: 字符起始y坐标
 * @param  end_x: 字符结束x坐标
 * @param  end_y: 字符结束y坐标
 * @param  name:  图形名字(必须3个uint8_t，不够记得补零，实在懒得写判断了....)
 * @param  width: 线宽
 * @param  size: 字体大小
 * @param  layer: 图层
 * @param  color: 颜色
 * @param  contents: 内容
* @param   length: 长度
 * @retval 1:正常发送 2:坐标错误 3:图层错误 4:颜色错误 
 */
uint8_t write_chars(uint32_t operate, uint32_t start_x,uint32_t start_y,uint32_t end_x,uint32_t end_y,uint8_t name[],uint32_t width, uint32_t size, uint8_t layer,uint8_t color, uint8_t contents[], uint8_t length)
{
		custom_character.grapic_data_struct.graphic_name[0] = name[0];
		custom_character.grapic_data_struct.graphic_name[1] = name[1];
		custom_character.grapic_data_struct.graphic_name[2] = name[2];
		
	  switch (operate)
		{
			case 1:
			{
				custom_character.grapic_data_struct.operate_tpye = OPERATE_ADD;
				break;
			}
			case 2:
			{
				custom_character.grapic_data_struct.operate_tpye = OPERATE_CHANGE;
				break;
			}
		}

		custom_character.grapic_data_struct.graphic_tpye = GRAPHIC_CHAR;
		custom_character.grapic_data_struct.width = width;
		custom_character.grapic_data_struct.radius = size;
		
		custom_character.grapic_data_struct.start_x = start_x;
		custom_character.grapic_data_struct.start_y = start_y;
		custom_character.grapic_data_struct.end_x = end_x;
		custom_character.grapic_data_struct.end_y = end_y;

    if(layer > LAYER_NUM)
    {
        return 3;
    }
    else
    {
        custom_character.grapic_data_struct.layer = layer;
    }
        
    if(color >= COLOR_NUM)
    {
        return 4;
    }
    else
    {
        custom_character.grapic_data_struct.color = color;
    }

		header_data.data_cmd_id = 0x0110;
		
		load_chars(contents, length);
		UiFrame.UiFrameBuffer[1] = 36;

		send_seq++;
    UiFrame.UiFrameBuffer[3] = send_seq;

    memcpy(UiFrame.UiFrameBuffer+JUDGE_DATA_OFFSET, &header_data, sizeof(ext_student_interactive_header_data_t));
    memcpy(UiFrame.UiFrameBuffer+JUDGE_DATA_OFFSET + sizeof(ext_student_interactive_header_data_t), &custom_character, 45);
    Append_CRC_Check_Sum(UiFrame.UiFrameBuffer, UiFrame.UiFrameBuffer[1]);
    HAL_UART_Transmit_DMA(&REFEREE_HUART,UiFrame.UiFrameBuffer,(UiFrame.UiFrameBuffer[1]+9)); //头帧加校验共9字节
		
		return 1;
}

/**
 * @brief 发送图形
 * @note   
* @retval 1:发送成功,0:没有发送
 */
uint8_t send_graphic(void)
{
    uint8_t graphic_num;
    graphic_num = check_empty_graphic();
    uint8_t data_size;
    if(graphic_num == 0)
    {
        //全是空的不要发
        return 0;
    }
    else if(graphic_num == 1)
    {
        header_data.data_cmd_id = 0X0101;//发送一个图形的内容id
        UiFrame.UiFrameBuffer[1] = 21;//发送一个图形的长度
    }
    else if(graphic_num == 2)
    {
        header_data.data_cmd_id = 0X0102;
        UiFrame.UiFrameBuffer[1] = 36;
    }
    else if(graphic_num > 2 && graphic_num <= 5)
    {
        header_data.data_cmd_id = 0X0103;
        UiFrame.UiFrameBuffer[1] = 81;
    }
    else if(graphic_num > 5 && graphic_num <= 7)
    {
        header_data.data_cmd_id = 0X0104;
        UiFrame.UiFrameBuffer[1] = 111;
    }
    else
    {
        header_data.data_cmd_id = 0X0104;
        UiFrame.UiFrameBuffer[1] = 111;
    }
    send_seq++;
    UiFrame.UiFrameBuffer[3] = send_seq;
    data_size = 15 * graphic_num;

    memcpy(UiFrame.UiFrameBuffer+JUDGE_DATA_OFFSET, &header_data, sizeof(ext_student_interactive_header_data_t));
    memcpy(UiFrame.UiFrameBuffer+JUDGE_DATA_OFFSET + sizeof(ext_student_interactive_header_data_t), graphic_data, data_size);
    Append_CRC_Check_Sum(UiFrame.UiFrameBuffer, UiFrame.UiFrameBuffer[1]);
    HAL_UART_Transmit_DMA(&REFEREE_HUART,UiFrame.UiFrameBuffer,(UiFrame.UiFrameBuffer[1]+9)); //头帧加校验共9字节
    //相当于清空数据
    for(int i = 0; i < graphic_num; i++)
    {
        graphic_data[i].operate_tpye = OPERATE_NULL;
    }
		return 1;
}

/**
 * @brief  判断是否为红方队伍
 * @note   
 * @retval 0 未收到 1 红 2 蓝
 */
uint8_t WhichTeam(void)
{
    if(ext_game_robot_state.robot_id == 0)
    {
        return 0;
    }
    else if(ext_game_robot_state.robot_id < 10)
    {
        return 1;
    }
    else if(ext_game_robot_state.robot_id > 100)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  判断当前哪个graphic为被使用
 * @note   
 * @retval 当前第一个空的graphic
 */
uint8_t check_empty_graphic(void)
{
    for(int i = 0; i < GRAPHIC_NUM; i++)
    {
        if(graphic_data[i].operate_tpye == OPERATE_NULL)
        {
            return i;
        }
    }
    return GRAPHIC_NUM;
}

/**
 * @brief  获得当前射击状态
 * @note   不论状态，shoot_status_chars均赋值12字节的字符串，故字符串内不够12字节就补空格
 * @retval 
 */

uint8_t Get_Shoot_Status(void)
{
	switch (Shoot_Status_Value)
	{
		case STOP:
		{
			shoot_status_chars = (unsigned char*)"shoot:STOP  ";
			break;
		}
		case SINGLE_ROUND:
		{
			shoot_status_chars = (unsigned char*)"shoot:SIGNLE";
			break;
		}
		case MULTI_ROUND:
		{
			shoot_status_chars = (unsigned char*)"shoot:MULTI ";
			break;
		}
	}
	
	return 1;
}

/**
 * @brief  向custom_character中装入字符串，需要给定字符串长度（不需要计算'\0'）
 * @note   
 * @retval 正常录入 1
 */
uint8_t load_chars(uint8_t chars_to_send[], uint8_t length)
{
	uint8_t count = 0;

	for(count = 0; count < length; count++)
	{
		custom_character.data[count] = chars_to_send[count];
	}
	for(; count < 30; count++)//不够30字节的补0
	{
		custom_character.data[count] = 0;
	}
		
	return 1;
}

/**
 * @brief  向custom_character中装入字符串（字符串长于30则只录30个字符，不包括传入字符串的'\0'）
 * @note   
 * @retval 正常录入 1
 */
/*uint8_t load_chars(uint8_t chars_to_send[])
{
	uint8_t count = 0;
	uint8_t length = 0;
	for(count = 0; count < 30; count++)
	{
		if(chars_to_send[count] == '\0')
			break;
		custom_character.data[count] = chars_to_send[count];
	}
	for(; count < 30; count++)//不够30字节的补0
	{
		custom_character.data[count] = 0;
	}
		
	return 1;
}*/


