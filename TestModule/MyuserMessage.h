#ifndef MY_USER_MESSAGE_H
#define MY_USER_MESSAGE_H

#define WM_MSG_UPDATE_LOG		WM_USER+800			//更新窗口LOG
#define WM_MSG_RUNNING_STAT		WM_USER+801			//拼图运行状态
#define WM_MSG_SITEINFO			WM_USER+802			//雷达站列表窗口接收消息的ID			
#define WM_MSG_MOS				WM_USER+803			//组网线程，识别是否组网的ID
#define WM_MSG_ID				WM_USER+804			//通用消息ID
#define MYWM_ICONNOTIFY         WM_USER+805			//托盘图标消息

typedef enum enmu_RADAR_INFO_MSG
{
	RAD_UPDATE_DIR=1, //数据路径改变
	RAD_UPDATE_POS,
}radar_info_msg;

typedef enum enmu_RUNNING_STAT_MSG
{
	MOSAIC_STAT_RUN_REAL=1, //实时数据处理运行中
	MOSAIC_STAT_RUN_OLD,   //历史数据处理
	MOSAIC_STAT_IDLE,  //未处理数据
	MOSAIC_STAT_SYS_EXIT
}running_stat_msg;

typedef enum enmu_UPDATE_LOG_MSG
{
	LOG_STATUS=1, //状态
	LOG_ERROR,    //异常	
	LOG_NONE	  //不显示
}update_log_msg;

#endif