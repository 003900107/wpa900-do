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
  u32 RTCCount;              //用于获取RTC的当前计数
  static TimeStruct CurrentTime; //时间结构定义，从RTC计数获取时间
 
  RTCCount=     RTC_GetCounter(); //获取RTC的当前计数
  Time_Convert(RTCCount,&CurrentTime); //转换为具体时间

  //利用库函数将时间转换为格式化
  sprintf(s,"%0.2d:%0.2d:%0.2d",CurrentTime.Hour,CurrentTime.Minute,CurrentTime.Second);
  Shell_PutStr(s); /*在屏幕上显示*/
  Shell_PutStr("\n");  //换行，回到命令提示符
}
