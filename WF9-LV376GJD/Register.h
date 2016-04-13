#ifndef	_Register_H
#define _Register_H

#include "jblib.h"

char Register(void)
{
	int i = 0;
	int len = 0;
	MYFILE *fp;
	char temp[128] = {0};
	char serial[32] = {0};
	char sn[16] = {0};
	unsigned char key[16] = {0};
	
	AdmEnvParams p;
	
	getAdmParam((char *)(&p));
	
	sprintf(serial, "sn%s.dat", p.id);
	
	sprintf(key, "Wasion");
	len = strlen(key);
	
	if (strlen(p.id) < len)
	{
		MessageBox("获取ID失败!", "提示", MB_OK);
		return 0;
	}
	memcpy(sn, p.id + strlen(p.id) - len, len);
	for (i = 0; i < len; i++)
	{
		key[i] = sn[i] ^ key[i];
	}

	if (fileexits(serial) != FSNULL)
	{
		fp = fopen(serial, "r");		
			
		memset(temp, 0, sizeof(temp));
		if (fread(temp, len, fp) >= len)
		{
			for (i = 0; i < len; i++)
			{
				if (key[i] != temp[i])
				{
					break;
				}				
			}	
			
			if (i >= len)
			{
				fclose(fp);
				return 1;
			}				
		}
		
		fclose(fp);
	}
	
	memset(sn, 0, sizeof(sn));
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "长度：%d", 2 * len);
	
	while(1)
	{		
		if (InputBoxWS(temp, "请输入注册码", sn, 2*len) != ID_OK)
		{
			return 0;
		}	
		
		if (strlen(sn) != 2 * len)
		{
			MessageBox("长度错误!", "提示", MB_OK);
			continue;
		}
		
		
		break;		
	}
	
	StrToHexStr(sn, len, 0, 0, 0);

	for (i = 0; i < len; i++)
	{
		if (key[i] != sn[i])
		{
			break;
		}				
	}
	
	if (i >= len)
	{
		fp = fopen(serial, "war");
		fseek(fp, 0, SEEK_SET);
	  fwrite(sn, len, fp);
	  fclose(fp);
	  MessageBox("注册成功!", "提示", MB_OK);
	  return 1;
	}
	else
	{
		MessageBox("注册失败!", "提示", MB_OK);
		return 0;
	}
}

#endif