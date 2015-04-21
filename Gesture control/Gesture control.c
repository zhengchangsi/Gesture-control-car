#include  <REG52.H>	
#include  <depend.h>
#include  <math.h>   
#include  <stdio.h>  	
#include  <INTRINS.H>
#define uint unsigned int
#define uchar unsigned char      
#define	SlaveAddress   0xA6	  
                             
#define JudgeP_M  0x8000  
#define Left_cmp  0x006e   
#define Right_cmp 0x006e
#define Go_cmp    0x006e
#define Back_cmp  0x006e
#define Go    0xaa		  
#define Back  0x55
#define Left  0xa0
#define Right 0x0a
#define Stop  0x00

sbit	  SCL=P1^0;      
sbit 	  SDA=P1^1;		  
Byte BUF[8];                        
Byte Sbuf[8]; 
uint Wbuf[2];    	
 
void Init_ADXL345(void);
void  Multiple_Read_ADXL345();
void Delay5us();
void Delay5ms();
void Delay20ms();
void ADXL345_Start();
void ADXL345_Stop();
void ADXL345_SendACK(bit ack);
bit  ADXL345_RecvACK();
void ADXL345_SendByte(Byte dat);
Byte ADXL345_RecvByte();
void ADXL345_ReadPage();
void ADXL345_WritePage();
void send();
void uart();
void Data_Convert();
void Data_Process();
void direction_judge();



void main()
{ 
	void Delay20ms();	                   		                   
	uart();	
	Init_ADXL345();                 
	while(1)                         
	{ 
		Multiple_Read_ADXL345();       	
		Data_Convert();
        Data_Process();
		direction_judge();
		Delay20ms();                       
	}
}

/*******************************
延时函数
*****************************/
void Delay20ms()	
{
	unsigned char i, j, k;
	_nop_();
	_nop_();
	i = 1;
	j = 216;
	k = 35;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
										

void Delay5us()
{
    _nop_();_nop_();_nop_();_nop_();
    _nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
}


void Delay5ms()
{
    uint n = 560;

    while (n--);
}
/**************************************
起始信号
**************************************/
void ADXL345_Start()
{
    SDA = 1;                   
    SCL = 1;                   
    Delay5us();                
    SDA = 0;                    
    Delay5us();                 
    SCL = 0;                    
}
/**************************************
停止信号
**************************************/
void ADXL345_Stop()
{
    SDA = 0;                   
    SCL = 1;                   
    Delay5us();                 
    SDA = 1;                    
    Delay5us();                 
}
/**************************************
发送应答信号
**************************************/
void ADXL345_SendACK(bit ack)
{
    SDA = ack;                
    SCL = 1;                    
    Delay5us();             
    SCL = 0;                   
    Delay5us();                
}
/**************************************
接收应答信号
**************************************/
bit ADXL345_RecvACK()
{
    SCL = 1;                    
    Delay5us();               
    CY = SDA;                   
    SCL = 0;                   
    Delay5us();                
    return CY;
}
/**************************************
向IIC总线发送一个字节数据
**************************************/
void ADXL345_SendByte(Byte dat)
{
    Byte i;

    for (i=0; i<8; i++)         
    {
        dat <<= 1;              
        SDA = CY;               
        SCL = 1;               
        Delay5us();            
        SCL = 0;                
        Delay5us();             
    }
    ADXL345_RecvACK();
}
/**************************************
从IIC总线接收一个字节数据
**************************************/
Byte ADXL345_RecvByte()
{
    Byte i;
    Byte dat = 0;
    SDA = 1;                  
    for (i=0; i<8; i++)         
    {
        dat <<= 1;
        SCL = 1;               
        Delay5us();             
        dat |= SDA;                            
        SCL = 0;              
        Delay5us();            
    }
    return dat;
}
/**************************************
单字节写入
**************************************/
void Single_Write_ADXL345(Byte REG_Address,Byte REG_data)
{
    ADXL345_Start();                 
    ADXL345_SendByte(SlaveAddress);  
    ADXL345_SendByte(REG_Address);   
    ADXL345_SendByte(REG_data);     
    ADXL345_Stop();                   
}
/**************************************
单字节读取
**************************************/
Byte Single_Read_ADXL345(Byte REG_Address)
{  Byte REG_data;
    ADXL345_Start();                          
    ADXL345_SendByte(SlaveAddress);          
    ADXL345_SendByte(REG_Address);            	
    ADXL345_Start();                         
    ADXL345_SendByte(SlaveAddress+1);        
    REG_data=ADXL345_RecvByte();             
	ADXL345_SendACK(1);   
	ADXL345_Stop();                          
    return REG_data; 
}
/**************************************
连续读出ADXL345内部加速度数据
**************************************/
void Multiple_read_ADXL345(void)
{   Byte i;
    ADXL345_Start();                          
    ADXL345_SendByte(SlaveAddress);           
    ADXL345_SendByte(0x32);                   	
    ADXL345_Start();                         
    ADXL345_SendByte(SlaveAddress+1);         
	 for (i=0; i<6; i++)                      
    {
        BUF[i] = ADXL345_RecvByte();         
        if (i == 5)
        {
           ADXL345_SendACK(1);               
        }
        else
        {
          ADXL345_SendACK(0);               
       }
   }
    ADXL345_Stop();                          
    Delay5ms();
}
/**************************************
初始化ADXL345
**************************************/
void Init_ADXL345()
{
   Single_Write_ADXL345(0x31,0x0B);   
   Single_Write_ADXL345(0x2C,0x08);   
   Single_Write_ADXL345(0x2D,0x08);  
   Single_Write_ADXL345(0x2E,0x80);   
   Single_Write_ADXL345(0x1E,0x00);   
   Single_Write_ADXL345(0x1F,0x00);  
   Single_Write_ADXL345(0x20,0x05);  
}
void uart()
{
	SCON=0x50;
	TMOD=0x20;
	PCON=0x00;
	TH1=0xfd;
	TL1=0xfd;
	IE=0x90;
	TR1=1;
}

void send(unsigned char ch) 
{
	ES=0; 
	SBUF=ch; 
	while (TI==0); 
	TI=0 ;
	ES=1; 
}
void Data_Convert()					
{
	Wbuf[0]=BUF[1]<<8|BUF[0];
	Wbuf[1]=BUF[3]<<8|BUF[2];
	Wbuf[2]=BUF[5]<<8|BUF[4];
} 
void Data_Process()
{
	uint i=0;
	i=Wbuf[0]&JudgeP_M;	   
	if(i==0x8000)	          
	{
		Wbuf[0]=~Wbuf[0]+1;
		Wbuf[0]=Wbuf[0]&0x7fff;
		if(Wbuf[0]>Left_cmp)
		{
			Sbuf[0]=Left;
		}
		else
		{
			Sbuf[0]=Stop;
		}
	}
	else					  
	{
		if(Wbuf[0]>Right_cmp)
		{
		  Sbuf[0]=Right;
		}
		else
		{
		   Sbuf[0]=Stop;
		}
	}
	i=Wbuf[1]&JudgeP_M;		  
	if(i==0x8000)	           
	{
		Wbuf[1]=~Wbuf[1]+1;
		Wbuf[1]=Wbuf[1]&0x7fff;
		if(Wbuf[1]>Back_cmp)
		{
			Sbuf[1]=Back;
		}
		else
		{
			Sbuf[1]=Stop;
		}
	}
	else					 
	{
		if(Wbuf[1]>Go_cmp)
		{
		 	Sbuf[1]=Go;
		}
		else
		{
		 	Sbuf[1]=Stop;
		}
	}
}

void direction_judge()
{

	if(Sbuf[0]==Left|Sbuf[0]==Right)
	{
		send(Sbuf[0]);
	}
	else
	{
		send(Sbuf[1]);
	}
}