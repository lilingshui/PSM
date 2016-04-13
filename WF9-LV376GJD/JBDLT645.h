#ifndef	_JBDLT645_H
#define _JBDLT645_H

#include "jblib.h"

U8 ir_read_data(P_U8 bdz,U16 id,P_U8 ir_data,U16 delaytime);
U8 ir_receive_data(P_U8 irdata);
U8 ir_read_data_07(P_U8 bdz,P_U8 id,P_U8 ir_data,U16 delaytime);
U8 ir_receive_data_07(P_U8 irdata);

/**************************/
void bcdtochar(P_U8 databcd,U8 len,P_U8 datachar)
{
    U8 i ;
    for(i=0;i<len;i++)
    {
        *(datachar++)=(*databcd>>4)+'0' ;
        *(datachar++)=(*databcd&0x0f)+'0' ;
        databcd++;
    }
    *datachar='\0' ;
}

void rotate(P_U8 dataline,U8 len)
{
    U8 i,data_temp ;
    if(len%2==0x00)
    {
        //如果为偶数个的时候那么就交换len/2次
        for(i=0;i<len/2;i++)
        {
            data_temp=*(dataline+i);
            *(dataline+i)=*(dataline+len-1-i);
            *(dataline+len-1-i)=data_temp ;
        }
    }
    else 
    {
        //如果为奇数个的时候中间的元素不动
        for(i=0;i<(len-1)/2;i++)
        {
            data_temp=*(dataline+i);
            *(dataline+i)=*(dataline+len-1-i);
            *(dataline+len-1-i)=data_temp ;
        }
    }
}

void bdztobcd(P_U8 bdz,P_U8 bdzbcd,U8 mode)
{
    U8 i,len,bdz_array[13];
    P_U8 bdz_p=bdz_array+11 ;
    if(mode==0)for(i=0;i<12;i++)bdz_array[i]=':' ;
    if(mode==1)for(i=0;i<12;i++)bdz_array[i]='0' ;
    bdz_array[12]='\0' ;
    for(i=0;i<12;i++)if(!(bdz[i]=='A'||bdz[i]=='a'))break ;
    if(i==12)
    {
        for(i=0;i<6;i++)bdzbcd[i]=0xaa ;
        return ;
    }
    for(i=0;i<12;i++)if(bdz[i]!='9')break ;
    if(i==12)
    {
        for(i=0;i<6;i++)bdzbcd[i]=0x99 ;
        return ;
    }
    for(len=0;*(bdz+len)!='\0';len++);
    //执行空语句来判断共有多少个字节
    if(!len)
    {
        for(i=0;i<6;i++)bdzbcd[i]=0xaa ;
        return ;
    }
    len--;
    if(len%2)
    {
        for(;len>0;len--)*(bdz_p--)=*(bdz+len);
    }
    else 
    {
        for(;len>1;len--)*(bdz_p--)=*(bdz+len);
    }
    *(bdz_p--)=*(bdz+len);
    bdz=bdz_array ;
    for(i=0;i<6;i++)
    {
        *(bdzbcd+5-i)=((*bdz-'0')<<4)+(*(bdz+1)-'0');
        bdz=bdz+2 ;
    }
}

/**************************/
U8 chartobcd(P_U8 datachar,P_U8 databcd)
{
    U8 i,len ;
    P_U8 datachar_p=datachar ;
    for(i=0;*(datachar_p++)!='\0';i++);
    if(i%2==0x00)
    {
        for(len=0;*datachar!='\0';len++)
        {
            *(databcd++)=((*datachar-'0')<<4)+(*(datachar+1)-'0');
            datachar=datachar+2 ;
        }
    }
    else 
    {
        *(databcd++)=*(datachar++)&0x0f ;
        for(len=1;*datachar!='\0';len++)
        {
            *(databcd++)=((*datachar-'0')<<4)+(*(datachar+1)-'0');
            datachar=datachar+2 ;
        }
    }
    return(len);
}

//---------------------------------------------------
/*-------------------------------
功    能：97接收电表数据
成功返回：帧长度
失败返回：0
--------------------------------*/
U8 ir_receive_data(P_U8 irdata)
{ 
    U8 len,len1 ;
    U8 i,ram,j,rt ;
    U8 flag=0,sum=0 ;
    flag=0 ;
    //      
    //      while(1)
    //      {
    //              if(!rport(&ram))
    //                      printf("%x,",ram);
    //              if(ram==0x16){ key(0);break;}
    //              
    //      }
    //      return;
    //接收第一个0x68
    for(i=0;i<8;i++)
    {
        //rt=rport(&ram);
        if(!rport(&ram)&&ram==0x68)
        {
            
            flag=1 ;
            break ;
        }
    }
    //接收不到0x68
    if(!flag)
    {
        //uartinit(1200,0x09,URS_LIRTX);
        return 0 ;
    }
    irdata[0]=ram ;
    //接收第一个0x68
    flag=0 ;
    //接收表地址
    for(i=1;i<7;i++)
    {
        if(!rport(&ram))
        irdata[i]=ram ;
    }
    //接收第二个0x68
    if(!rport(&ram))
    {
        if(ram==0x68)
        irdata[7]=ram ;
    }
    //失败
    else 
    {
        //uartinit(1200,0x09,URS_LIRTX);
        return 0 ;
    }
    //接收控制码
    if(!rport(&ram))
    {
        if(ram==0x81||ram==0xa1)
        irdata[8]=ram ;
    }
    //失败
    else 
    {
        //uartinit(1200,0x09,URS_LIRTX);
        return 0 ;
    }
    //接收数据长度
    if(!rport(&ram))
    {
        len1=irdata[9]=ram ;
    }
    //失败
    else 
    {
        //uartinit(1200,0x09,URS_LIRTX);
        return 0 ;
    }
    //接收剩余字节
    for(i=10;i<len1+10+2;i++)
    {
        if(!rport(&ram))
        irdata[i]=ram ;
        if(ram==0x16)break ;
    }
    irdata[len1+12]=0 ;
    //加结束符
    len=len1+12 ;
    //帧长度
    for(j=0;j<len-2;j++)
    sum+=irdata[j];
    //计算效验码
    //比较效验码    
    if(sum!=irdata[len-2])
    {
        //uartinit(1200,0x09,URS_LIRTX);
        return 0 ;
    }
    return len ;
}

int rrport(char *c)
{
	int i;
	for(i=0;i<5;i++)
	{
		if(!rport(c)) return 1;
	}
	if(i>=5)
		 return 0;
}
/*-------------------------------
功    能：07规约接收电表数据
成功返回：帧长度
失败返回：0
--------------------------------*/
U8 ir_receive_data_07(P_U8 irdata)
{
	U8 len,len1;
	U8 ram,j,rt;
	U8 sVal[20];
	U8 flag=0,sum=0;
	int i;
	flag=0;
	for(i=0;i<10;i++)               //接收第一个0x68
	{
		rt=rrport(&ram);
		Delay(10);
		if(rt&&ram==0x68) 
		{
			flag=1;
			break;
		}
	}
	if(!flag)                       //接收不到0x68
	{               
		//uart_init(0,1200,0x49);                 
		return 0;
	}
	irdata[0]=ram;                  //接收第一个0x68
	flag=0;
	for(i=1;i<7;i++)                //接收表地址
	{       
		if(rrport(&ram))      
			irdata[i]=ram;
		Delay(10);        
	}       
	if(rrport(&ram))              //接收第二个0x68
	{
		if(ram==0x68) 
			irdata[7]=ram;
	}
	else//失败
	{
	    // uart_init(0,1200,0x49); 
		return 0;
	}
	if(rrport(&ram))              //接收控制码
	{
		if(ram==0x91||ram==0xa1||ram==0x94) 
			irdata[8]=ram;
	}
	else//失败
	{
		// uart_init(0,1200,0x49); 
		return 0;
	}
	if(rrport(&ram))              //接收数据长度
	{
		len1=irdata[9]=ram;
	}
	else//失败
	{
		//uart_init(0,1200,0x49); 
		return 0;
	}
	for(i=10;i<len1+10+2;i++)       //接收剩余字节
	{
		if(rrport(&ram))      
			irdata[i]=ram;
		if(ram==0x16) break;
	}       
	irdata[len1+12]=0;              //加结束符
	len=len1+12;                    //帧长度
	for(j=0;j<len-2;j++) 
		sum+=irdata[j];         //计算效验码
	
	if(sum!=irdata[len-2])          //比较效验码    
	{
		// uart_init(0,1200,0x49);
		return 0;
	}
	return len;
}

/*----------------------------------------------------------
DL/T645-1997 中华人民共和国电力工业部发布（国标规约）
读电表数据函数
参数1:电表通讯地址  参数2:标识编码  参数3:存放读回电表数据 参数4:发送字节延时（毫秒）
-----------------------------------------------------------*/
U8 ir_read_data(P_U8 bdz,U16 id,P_U8 ir_data,U16 Delaytime)
{
    U8 sendfram[14]= { 0 } ;
    //发送数据帧 长度14字节
    U8 addrbcd[6],i,ii,sum=0 ;
    U8 flag=0,ram,temp[13]= { 0 } ;
    U16 len,j,len1,len2,flag1=0 ;
    U8 recieve_data[200]= { 0 } ;
    //存放接收到的电表数据，最大200字节
    U8 recieve_data1[200]= { 0 } ;
    
    LOOP :
    
    //帧起始码
    //uartinit(1200,0x09,URS_LIRTX);
    //初始化抄表机红外口 波特率1200kbps 一位起始位、8位数据、一个偶效验、一个停止位
    sendfram[0]=0x68 ;
    if(Delaytime==20)
    {
        strcpy(temp,"000000000000");
        if(strlen(bdz)<12)
        {
            memcpy(&temp[12-strlen(bdz)],bdz,strlen(bdz));
            strcpy(bdz,temp);
        }
        //表地址不足12位 补零
        bdztobcd(bdz,addrbcd,1);
        //表地址转BCD码，不足12位补0
    }
    else 
    bdztobcd(bdz,addrbcd,0);
    //表地址转BCD码，不足12位补A
    for(i=1;i<=6;i++)
    sendfram[i]=addrbcd[i-1];
    //表地址
    sendfram[7]=0x68 ;
    //帧起始符
    sendfram[8]=0x01 ;
    //控制码
    sendfram[9]=0x02 ;
    //数据长度
    sendfram[10]=(id&0x00ff)+0x33 ;
    //标识编码低位 +0x33
    sendfram[11]=((id>>8)&0x00ff)+0x33 ;
    //标识编码高位 +0x33
    
    for(i=0;i<12;i++)
    sum+=sendfram[i];
    //各字节的累加和，不记超过256的溢出值
    sendfram[12]=sum ;
    //效验码
    sendfram[13]=0x16 ;
    //停止位
    Delay(200);
    sport(0xfe);
    Delay(10);
    //唤醒电表处于接收状态
    sport(0xfe);
    Delay(10);
    sport(0xfe);
    for(i=0;i<14;i++)
    {
        Delay(10);
        sport(sendfram[i]);
        //发送帧                          
    }
    // sport(0x16);
    sum=0 ;
    
    len1=ir_receive_data(recieve_data);
    //        for(i=0;i<len1;i++)
    //        {
    //               
    //                printf("%2x,",recieve_data[i]);  //发送帧                          
    //        }
    //        key(0);
    //uartinit(0,0x09,URS_LIRTX);
    if(len1<5)
    {
        //uartinit(0,0x09,URS_LIRTX);
        flag++;
        if(flag>1)
        return 0 ;
        else goto LOOP ;
    }
    for(i=0;i<len1;i++)
    {
        //判断接收数据帧的起始符
        if(recieve_data[i]==0x68)
        {
            //printf("0");key(0);
            break ;
            //printf("1");key(0);
        }
    }
    //判断第二个起始符
    if(recieve_data[7+i]!=0x68)
    {
        //printf("2");key(0);
        //uartinit(0,0x09,URS_LIRTX);
        flag++;
        if(flag>1)
        return 0 ;
        else goto LOOP ;
    }
    //控制码，无后续帧
    if(recieve_data[8+i]!=0x81)
    {
        //printf("3");key(0);
        //uartinit(0,0x09,URS_LIRTX);
        flag++;
        if(flag>1)
        return 0 ;
        else goto LOOP ;
    }
    len=recieve_data[9+i];
    //比较标识编码
    if(recieve_data[10+i]!=sendfram[10])
    {
        //printf("4");key(0);
        //uartinit(0,0x09,URS_LIRTX);
        flag++;
        if(flag>1)
        return 0 ;
        else goto LOOP ;
    }
    if(recieve_data[11+i]!=sendfram[11])
    {
        //printf("5");key(0);
        //uartinit(0,0x09,URS_LIRTX);
        flag++;
        if(flag>1)
        return 0 ;
        else goto LOOP ;
    }
    sum=0 ;
    for(j=0;j<len+10;j++)
    {
        sum+=recieve_data[i+j];
    }
    //比较校验和
    if(recieve_data[len+i+10]!=sum)
    {
        //printf("6");key(0);
        //uartinit(0,0x09,URS_LIRTX);
        flag++;
        if(flag>1)
        return 0 ;
        else goto LOOP ;
    }
    //        printf("-%d-%d-%d-%0x-",len,i,len+i+11,recieve_data[len+i+11]);
    //        key(0);
    //        if(recieve_data[len+i+11]!= 0x16)//比较停止位
    //        {
    //        	printf("-%d-%d-%d-%0x-",len,i,len+i+11,recieve_data[len+i+11]);
    //
    //        	printf("7-len:%d-%d-%02x",len,len+i+11,recieve_data[len+i+11]);
    //        	
    //                //uartinit(0,0x09,URS_LIRTX);
    //                flag++; 
    //                if(flag>1)
    //                return 0;
    //                else goto LOOP;
    //        }
    for(j=0;j<len;j++)
    recieve_data[i+10+j]-=0x33 ;
    //对接收数据减去0x33
    if((id&0x000f)==0x0f&&recieve_data[len1-3]==0xAA)
    {
        memcpy(recieve_data1,recieve_data+i+12,len-3);
        rotate(recieve_data1,len-3);
        // len2=len-3;
    }
    else 
    {
        memcpy(recieve_data1,recieve_data+i+12,len-2);
        rotate(recieve_data1,len-2);
        //len2=len-2;
    }
    if((id&0x000f)==0x0f&&recieve_data[len1-3]==0xAA)
    bcdtochar(recieve_data1,len-3,ir_data);
    //BCD码转换成CHAR
    else 
    bcdtochar(recieve_data1,len-2,ir_data);
    Delay(Delaytime);
    return 1 ;
}

U8 Char2Bcd(P_U8 datachar,P_U8 databcd)
{
	U8 i,len;
    P_U8 datachar_p=datachar;
   
    for(len=0;*datachar!='\0';len++)
    {    	
    	if(*datachar>='a') *datachar=*datachar-0x20;    
    	if(*(datachar+1)>='a') *(datachar+1)=*(datachar+1)-0x20;
    	
		if(*datachar>='A'&&*(datachar+1)>='A')
			*(databcd++)=((*datachar-55)<<4)+(*(datachar+1)-55);
			
		if(*datachar<'A'&&*(datachar+1)>='A'&&*(datachar+1)<'a')	
			*(databcd++)=((*datachar-'0')<<4)+(*(datachar+1)-55);
		if(*datachar>='A'&&*(datachar+1)<'A')	
			*(databcd++)=((*datachar-55)<<4)+(*(datachar+1)-0x30);
		if(*datachar<'A'&&*(datachar+1)<'A')	
			*(databcd++)=((*datachar-0x30)<<4)+(*(datachar+1)-0x30);
	    datachar=datachar+2;
    }       
}
/*----------------------------------------------------------
DL/T645-2007 中华人民共和国电力工业部发布（国标规约）
读电表数据函数
参数1:电表通讯地址  参数2:标识编码  参数3:存放读回电表数据 参数4:发送字节延时（毫秒）
-----------------------------------------------------------*/
U8 ir_read_data_07(P_U8 bdz,P_U8 id,P_U8 ir_data,U16 delaytime)
{
	U8 gb_flag=0;
	U8 sendfram[16]={0}; //发送数据帧 长度16字节
	U8 addrbcd[6],idbcd[4],i,ii,sum=0;
	U8 flag=0,ram,temp[13]={0};
	U8 len,j,len1,len2,flag1=0;
	U8 recieve_data[200]={0};       //存放接收到的电表数据，最大200字节
	U8 recieve_data1[200]={0};   
	U8 datalen;
   
LOOP:	
	sendfram[0]=0x68;               //帧起始码
	//uartinit(1200,0x09,5);         //初始化抄表机红外口 波特率1200kbps 一位起始位、8位数据、一个偶效验、一个停止位
	Delay(200);
	strcpy(temp,"000000000000");
	if(strlen(bdz)<12)
	{
		memcpy(&temp[12-strlen(bdz)],bdz,strlen(bdz));
		strcpy(bdz,temp);
	}//表地址不足12位 补零
	bdztobcd(bdz,addrbcd,1);        //表地址转BCD码
	//
	//bdztobcd(bdz,addrbcd,0);        //表地址转BCD码，不足12位补A
	//
	Char2Bcd(id,idbcd);
	for(i=1;i<=6;i++)               
	sendfram[i]=addrbcd[i-1];       //表地址
	sendfram[7]=0x68;               //帧起始符
	sendfram[8]=0x11;               //控制码
	sendfram[9]=0x04;               //数据长度
	sendfram[10]=idbcd[3]+0x33;  //标识编码低位 +0x33
	sendfram[11]=idbcd[2]+0x33;//标识编码高位 +0x33
	sendfram[12]=idbcd[1]+0x33;  //标识编码低位 +0x33
	sendfram[13]=idbcd[0]+0x33;//标识编码高位 +0x33
	for(i=0;i<14;i++)
		sum+=sendfram[i];               //各字节的累加和，不记超过256的溢出值
	sendfram[14]=sum;               //效验码
	sendfram[15]=0x16;              //停止位

	sport(0xfe);Delay(10);    //唤醒电表处于接收状态
	sport(0xfe);Delay(10);
	sport(0xfe);
	for(i=0;i<16;i++)
	{
//		Delay(10); 
//		sport(sendfram[i]);  //发送帧                                         
		sbuf(sendfram, 16, 0);  //发送帧                                         
	}
	sum=0;
	len1=ir_receive_data_07(recieve_data);
	//uartinit(0,0x09,5);
	if(len1<5) 
	{
		//uartinit(0,0x09,5);
		flag++; 
		if(flag>1) return 0;
		else goto LOOP;
	}
	for(i=0;i<len1;i++)
	{
		if(recieve_data[i]==0x68)//判断接收数据帧的起始符
		break;
	}
	if(recieve_data[7+i]!= 0x68)    //判断第二个起始符
	{
		//uartinit(0,0x09,5);
		flag++; 
		if(flag>1) return 0;
		else goto LOOP;
	}
	if(recieve_data[8+i]!= 0x91)    //控制码，无后续帧
	{
		//uartinit(0,0x09,5);
		flag++; 
		if(flag>1) return 0;
		else goto LOOP;
	}
	len=recieve_data[9+i];
	if(recieve_data[10+i]!= sendfram[10])//比较标识编码
	{
		//uartinit(0,0x09,5);
		flag++; 
		if(flag>1) return 0;
		else goto LOOP;
	}
	if(recieve_data[11+i]!= sendfram[11])
	{
		//uartinit(0,0x09,5);
		flag++; 
		if(flag>1) return 0;
		else goto LOOP;
	}
	if(recieve_data[12+i]!= sendfram[12])
	{
		//uartinit(0,0x09,5);
		flag++; 
		if(flag>1) return 0;
		else goto LOOP;
	}
	if(recieve_data[13+i]!= sendfram[13])
	{
		//uartinit(0,0x09,5);
		flag++; 
		if(flag>1) return 0;
		else goto LOOP;
	}
	sum=0;
	for(j=0;j<len+10;j++)
	{
		sum+=recieve_data[i+j];
	}
	if(recieve_data[len+i+10]!= sum)//比较校验和
	{
		//uartinit(0,0x09,5);
		flag++; 
		if(flag>1) return 0;
		else goto LOOP;
	}
	if(recieve_data[len+i+11]!= 0x16)//比较停止位
	{
		//uartinit(0,0x09,5);
		flag++; 
		if(flag>1) return 0;
		else goto LOOP;
	}
	for(j=0;j<len;j++)
		recieve_data[i+10+j]-=0x33;//对接收数据减去0x33
	
	if((id[4]&0x000f)==0x0f)           
	{
		memcpy(recieve_data1,recieve_data+i+14,len-5);
		rotate(recieve_data1,len-5);
		len2=len-5;
	}
	else
	{
		memcpy(recieve_data1,recieve_data+i+14,len-4);
		rotate(recieve_data1,len-4);
		len2=len-4;
	}
	if((id[4]&0x000f)==0x0f)
		bcdtochar(recieve_data1,len-5,ir_data);//BCD码转换成CHAR
	else
		bcdtochar(recieve_data1,len-4,ir_data);

	Delay(delaytime);
	return len2;
}
#endif
