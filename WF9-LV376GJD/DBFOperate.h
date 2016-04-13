#ifndef _DBFOperate_H
#define _DBFOperate_H

#include "jblib.h"
#include "Global.h"
#include "DataConvert.h"


/******************************************************************************/
/*                             读取DBF参数项函数                              */ 
/*  功能:读取数据库内参数项                                                   */
/*  参数:param——参数项信息, paramtype——参数类型, filename——数据库名           */
/*  返回:参数个数                                                             */
/******************************************************************************/
int GetParamItemFromDBF(Stru_Param *param, U8 paramtype, U8 *filename)
{
  int r = 0;
  int count = 0;
  int paramcount = 0;
  U8 tmpPType = 0;
  U8 temp[NUM_255] = {0};
  
  use(filename);
  count = libsumr();
   
  for (r = 0; r < count; r++)
  {       
    libset(r, FD_PARAMTYPE);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    tmpPType = StrToInt(temp, HEX);
    if ((tmpPType != ((paramtype >> 4) & 0x0F)) && (tmpPType != (paramtype & 0x0F))) 
    {
      continue;
    }

    if (paramcount > MAX_PARAMITEM_NUM)
    {
      break;
    }
    
    param->Type = tmpPType;
            
    libset(r, FD_PARAMID);
    memset(temp, 0, sizeof(temp));
    libread(temp);    
    param->Param_ID =  StrToInt(temp, HEX);              
    
    libset(r, FD_PARAMAFN);
    memset(temp, 0, sizeof(temp));
    libread(temp);   
    param->AFN =  StrToInt(temp, HEX);         
    
    libset(r, FD_PARAMPN);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    param->Pn =  StrToInt(temp, DEC);     
    
    libset(r, FD_PARAMFN);
    memset(temp, 0, sizeof(temp));
    libread(temp);  
    param->Fn =  StrToInt(temp, DEC);  
    
    libset(r, FD_PARAMNAME);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    memset(param->Param_Name, 0, sizeof(param->Param_Name));  
    sprintf(param->Param_Name, "%s", temp); 
    
    param->Rn = r;
          
    param++;
    paramcount++;
  }             
  
  use("");
  return paramcount;
}


/******************************************************************************/
/*                             读取DBF参数项内容函数                          */ 
/*  功能:读取数据库内参数项内容                                               */
/*  参数:param——参数项信息, pItem——参数子项内容, filename——数据库名           */
/*  返回:参数子项个数                                                         */
/******************************************************************************/
int GetParamValueFromDBF(Stru_Param *param, Stru_ParamItem *pItem, U8 *filename)
{
  
  int i = 0;
  int r = 0;
  int count = 0;
  int paramcount = 0;
  int pos = 0;
  int len = 0;
  U8 temp[NUM_255] = {0};
  U8 sLen[NUM_10] = {0};
  
  use(filename);
  count = libsumr();

	for (r = 0; r < count; r++)
	{
    libset(r, FD_PARAMID);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (StrToInt(temp, HEX) == param->Param_ID)
    {
    	break;
    }  
	}
	if (r == count)
	{
		use("");
		return 0;
	}
	pItem->Param_ID = param->Param_ID;
      
  libset(r, FD_PITEMTYPE);
  memset(temp, 0, sizeof(temp));
  libread(temp);  
  if (temp[0] == '0' || temp[0] == '\0')
  {
		use("");
		return 0;
  }
  
  len = strlen(temp);
  paramcount = 0;
  for (i = 0; i < len; i++)
  {
  	if (temp[i] == SEPCHAR)
  	{  		
  		(pItem + paramcount)->Param_Item_Mask = temp[i - 1];
  		paramcount++;
  		continue;
  	}
  	if (i == len - 1 && temp[i] != SEPCHAR)
  	{
  		(pItem + paramcount)->Param_Item_Mask = temp[i];
  		paramcount++;
  	}
  } 
   
  paramcount = 0; 
  pos = 0;
  libset(r, FD_PITEMVALUE);
  memset(temp, 0, sizeof(temp));
  libread(temp);  
 
  len = strlen(temp);
  for (i = 0; i < len; i++)
  {
  	if (temp[i] == SEPCHAR)
  	{  		
  		memcpy((pItem + paramcount)->Param_Item_Value, temp + pos, i - pos);
  		paramcount++;
  		pos = i + 1;
  		continue;
  	}
  	if (i == len - 1 && temp[i] != SEPCHAR)
		{
			memcpy((pItem + paramcount)->Param_Item_Value, temp + pos, i - pos + 1);
		}
  }
  
  paramcount = 0; 
  pos = 0;
  libset(r, FD_PITEMLEN);
  memset(temp, 0, sizeof(temp));
  libread(temp);  
 
  len = strlen(temp);
  for (i = 0; i < len; i++)
  {
  	if (temp[i] == SEPCHAR)
  	{  		
  	  memset(sLen, 0, sizeof(sLen));
  		memcpy(sLen, temp + pos, i - pos);
  		(pItem + paramcount)->Param_Item_Clen = (U8)(StrToInt(sLen, DEC));
  		paramcount++;
  		pos = i + 1;
  		continue;
  	}
  	if (i == len - 1 && temp[i] != SEPCHAR)
		{
			memset(sLen, 0, sizeof(sLen));
  		memcpy(sLen, temp + pos, i - pos + 1);
  		(pItem + paramcount)->Param_Item_Clen = (U8)(StrToInt(sLen, DEC));
		}
  }
  
  paramcount = 0; 
  pos = 0;
  libset(r, FD_PARAMVALUE);
  memset(temp, 0, sizeof(temp));
  libread(temp);  
 
  len = strlen(temp);
  for (i = 0; i < len; i++)
  {
  	if (temp[i] == SEPCHAR)
  	{  		
  		memcpy((pItem + paramcount)->Param_Value, temp + pos, i - pos);
  		paramcount++;
  		pos = i + 1;
  	}
  	if (i == len - 1 && temp[i] != SEPCHAR)
		{
			memcpy((pItem + paramcount)->Param_Value, temp + pos, i - pos + 1);
  		paramcount++;
		}
  }
  
  paramcount = 0; 
  pos = 0;
  libset(r, FD_PARAMITEM);
  memset(temp, 0, sizeof(temp));
  libread(temp);  
 
  len = strlen(temp);
  for (i = 0; i < len; i++)
  {
  	if (temp[i] == SEPCHAR)
  	{
  		(pItem + paramcount)->Param_Item_No = paramcount;   		
  		memcpy((pItem + paramcount)->Param_Item_Name, temp + pos, i - pos);
  		paramcount++;
  		pos = i + 1;
  	}
  	if (i == len - 1 && temp[i] != SEPCHAR)
		{
			(pItem + paramcount)->Param_Item_No = paramcount; 
			memcpy((pItem + paramcount)->Param_Item_Name, temp + pos, i - pos + 1);
  		paramcount++;
		}
  }           
  
  use("");
  param->Param_Num = paramcount;
  return paramcount;
}


/******************************************************************************/
/*                             写入DBF参数值函数                              */ 
/*  功能:写入数据库参数值                                                     */
/*  参数:param——参数项信息, pItem——参数子项内容, filename——数据库名           */
/*  返回:成功返回1,失败返回0                                                  */
/******************************************************************************/
int SetParamValueToDBF(Stru_Param *param, Stru_ParamItem *pItem, U8* filename)
{
  int i = 0;
  int len = 0;
  char temp[NUM_255] = {0};
   
  if (param->Param_Num < 1 || param->Param_ID != pItem->Param_ID)
  {
  	return 0;
  }
  
 	memset(temp, 0, sizeof(temp));
  for (i = 0; i < param->Param_Num; i++)
  {
  	strcat(temp, (pItem + i)->Param_Value);
  	len = strlen(temp);
	  temp[len] = SEPCHAR;
	  temp[len + 1] = '\0'; 	 	
  }
    
  use(filename);
  
  libset(param->Rn, FD_PARAMVALUE);
  libwrit(temp);
  
  use("");
  
  return 1;
}


/******************************************************************************/
/*                             读取DBF参数项函数                              */ 
/*  功能:读取数据库内参数项                                                   */
/*  参数:param——参数项信息, paramtype——参数类型, filename——数据库名           */
/*  返回:参数个数                                                             */
/******************************************************************************/
int GetParamIDFromDBF(Stru_Param *param, U8 paramtype, U8 *filename)
{
  int r = 0;
  int count = 0;
  int paramcount = 0;
  U8 temp[NUM_255] = {0};
  
  use(filename);
  count = libsumr();
   
  if (paramtype)
  {
  	for (r = 0; r < count; r++)
	  {  
	    libset(r, FD_PARAMID);
	    memset(temp, 0, sizeof(temp));
	    libread(temp);    
	    if (param->Param_ID == StrToInt(temp, HEX))
	    {
	    	break;
	    }  	
	  }	  	
  }
  else
  {
  	for (r = 0; r < count; r++)
	  {    	    
      libset(r, FD_PARAMFN);
	    memset(temp, 0, sizeof(temp));
	    libread(temp);  
	    if (param->Fn != StrToInt(temp, DEC))
	    {
	    	continue;
	    } 
	    
	    libset(r, FD_PARAMAFN);
	    memset(temp, 0, sizeof(temp));
	    libread(temp);   
	    if (param->AFN == StrToInt(temp, HEX))
	    {
	    	break;
	    } 
	  }
	  
  }
  
  if (r >= count)
  {
  	use("");
  	return 0;
  }
     
  libset(r, FD_PARAMTYPE);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  param->Type = StrToInt(temp, HEX);
          
  libset(r, FD_PARAMID);
  memset(temp, 0, sizeof(temp));
  libread(temp);    
  param->Param_ID =  StrToInt(temp, HEX);              
  
  libset(r, FD_PARAMAFN);
  memset(temp, 0, sizeof(temp));
  libread(temp);   
  param->AFN =  StrToInt(temp, HEX);         
  
  libset(r, FD_PARAMPN);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  param->Pn =  StrToInt(temp, DEC);     
  
  libset(r, FD_PARAMFN);
  memset(temp, 0, sizeof(temp));
  libread(temp);  
  param->Fn =  StrToInt(temp, DEC);  
  
  libset(r, FD_PARAMNAME);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  memset(param->Param_Name, 0, sizeof(param->Param_Name));  
  sprintf(param->Param_Name, "%s", temp); 
  
  param->Rn = r;
                    
  use("");
  return 1;
}
#endif
