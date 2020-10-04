#ifndef MY_USER_MESSAGE_H
#define MY_USER_MESSAGE_H

#define WM_MSG_UPDATE_LOG		WM_USER+800			//���´���LOG
#define WM_MSG_RUNNING_STAT		WM_USER+801			//ƴͼ����״̬
#define WM_MSG_SITEINFO			WM_USER+802			//�״�վ�б��ڽ�����Ϣ��ID			
#define WM_MSG_MOS				WM_USER+803			//�����̣߳�ʶ���Ƿ�������ID
#define WM_MSG_ID				WM_USER+804			//ͨ����ϢID
#define MYWM_ICONNOTIFY         WM_USER+805			//����ͼ����Ϣ

typedef enum enmu_RADAR_INFO_MSG
{
	RAD_UPDATE_DIR=1, //����·���ı�
	RAD_UPDATE_POS,
}radar_info_msg;

typedef enum enmu_RUNNING_STAT_MSG
{
	MOSAIC_STAT_RUN_REAL=1, //ʵʱ���ݴ���������
	MOSAIC_STAT_RUN_OLD,   //��ʷ���ݴ���
	MOSAIC_STAT_IDLE,  //δ��������
	MOSAIC_STAT_SYS_EXIT
}running_stat_msg;

typedef enum enmu_UPDATE_LOG_MSG
{
	LOG_STATUS=1, //״̬
	LOG_ERROR,    //�쳣	
	LOG_NONE	  //����ʾ
}update_log_msg;

#endif