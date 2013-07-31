char* CmdStrTable[5]=\
{
"help",
"ps",
"ledon",
"ledoff",
"date"
};
  CmdTable[0].CmdFunc=CmdHelp;
  CmdTable[1].CmdFunc=CmdCls;
  CmdTable[2].CmdFunc=CmdLedOn;
  CmdTable[3].CmdFunc=CmdLedOff;
  CmdTable[4].CmdFunc=CmdTime;

void CmdTime(void)
{
  u32 RTCCount;              //���ڻ�ȡRTC�ĵ�ǰ����
  static TimeStruct CurrentTime; //ʱ��ṹ���壬��RTC������ȡʱ��
 
  RTCCount=     RTC_GetCounter(); //��ȡRTC�ĵ�ǰ����
  Time_Convert(RTCCount,&CurrentTime); //ת��Ϊ����ʱ��

  //���ÿ⺯����ʱ��ת��Ϊ��ʽ��
  sprintf(s,"%0.2d:%0.2d:%0.2d",CurrentTime.Hour,CurrentTime.Minute,CurrentTime.Second);
  Shell_PutStr(s); /*����Ļ����ʾ*/
  Shell_PutStr("\n");  //���У��ص�������ʾ��
}
