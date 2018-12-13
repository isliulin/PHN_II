#include "nokia_5110.h"
#include "english_6x8_pixel.h"
#include "write_chinese_string_pixel.h"
#include "move_chinese_string_pixel.h"
//#include "stm32f10x_lib.h"

void LCD_init(void);
void LCD_clear(void);
void LCD_write_english_string(unsigned char X,unsigned char Y,char *s);
void LCD_write_chinese_string(unsigned char X, unsigned char Y,
                              unsigned char ch_with,unsigned char num,
                              unsigned char line,unsigned char row);
void chinese_string(unsigned char X, unsigned char Y, unsigned char T);
void LCD_write_char(unsigned char c);
void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                        unsigned char Pix_x,unsigned char Pix_y);
void LCD_write_byte(unsigned char dat, unsigned char dc);
void delay_1u(void);

/*-----------------------------------------------------------------------
LCD_init          : 3310LCD��ʼ��

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 
-----------------------------------------------------------------------*/

void delay_1u(void)                 //1us��ʱ����
{
    unsigned int i;
    for(i=0;i<1000;i++);
}

void delay_1m(void)                 //1ms��ʱ����
{
    unsigned int i;
    for (i=0;i<1140;i++);
}

void delay_nms(unsigned int n)       //N ms��ʱ����
{
    unsigned int i=0;
    for (i=0;i<n;i++)
        delay_1m();
}

static void LcdPinmux()
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	RCC_APB2PeriphClockCmd( RCC_SCLK | RCC_SDIN | RCC_LCD_DC |
                           RCC_LCD_CE  | RCC_LCD_RST  , ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_SCLK;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(  GPIO_SCLK_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_SDIN;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(  GPIO_SDIN_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_LCD_DC;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(  GPIO_LCD_DC_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_LCD_CE;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(  GPIO_LCD_CE_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_LCD_RST;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(  GPIO_LCD_RST_PORT, &GPIO_InitStructure);
	
}

void LCD_init(void)
{
	
	LcdPinmux();
	
	
    // ����һ����LCD��λ�ĵ͵�ƽ����
    GPIO_ResetBits(GPIO_LCD_RST_PORT,GPIO_LCD_RST);//LCD_RST = 0;
    delay_1u();

    GPIO_SetBits(GPIO_LCD_RST_PORT,GPIO_LCD_RST);//LCD_RST = 1;
    
    // �ر�LCD
    GPIO_ResetBits(GPIO_LCD_CE_PORT,GPIO_LCD_CE);//LCD_CE = 0;
    delay_1u();
    // ʹ��LCD
    GPIO_SetBits(GPIO_LCD_CE_PORT,GPIO_LCD_CE);//LCD_CE = 1;
    delay_1u();

    LCD_write_byte(0x21, 0);	// ʹ����չ��������LCDģʽ
    LCD_write_byte(0xc8, 0);	// ����ƫ�õ�ѹ
    LCD_write_byte(0x06, 0);	// �¶�У��
    LCD_write_byte(0x13, 0);	// 1:48
    LCD_write_byte(0x20, 0);	// ʹ�û�������
    LCD_clear();	        // ����
    LCD_write_byte(0x0c, 0);	// �趨��ʾģʽ��������ʾ

    // �ر�LCD
    GPIO_ResetBits(GPIO_LCD_CE_PORT,GPIO_LCD_CE);//LCD_CE = 0;
}

/*-----------------------------------------------------------------------
LCD_clear         : LCD��������

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 
-----------------------------------------------------------------------*/
void LCD_clear(void)
{
    unsigned int i;

    LCD_write_byte(0x0c, 0);			
    LCD_write_byte(0x80, 0);			

    for (i=0; i<504; i++)
        LCD_write_byte(0, 1);
}

/*-----------------------------------------------------------------------
LCD_set_XY        : ����LCD���꺯��

���������X       ��0��83
          Y       ��0��5

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 
-----------------------------------------------------------------------*/
void LCD_set_XY(unsigned char X, unsigned char Y)
{
    LCD_write_byte(0x40 | Y, 0);		// column
    LCD_write_byte(0x80 | X, 0);          	// row
}

/*-----------------------------------------------------------------------
LCD_write_char    : ��ʾӢ���ַ�

���������c       ����ʾ���ַ���

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 
-----------------------------------------------------------------------*/
void LCD_write_char(unsigned char c)
{
    unsigned char line;

    c -= 32;

    for (line=0; line<6; line++)
        LCD_write_byte(font6x8[c][line], 1);
}

/*-----------------------------------------------------------------------
LCD_write_english_String  : Ӣ���ַ�����ʾ����

���������*s      ��Ӣ���ַ���ָ�룻
          X��Y    : ��ʾ�ַ�����λ��,x 0-83 ,y 0-5

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 		
-----------------------------------------------------------------------*/
void LCD_write_english_string(unsigned char X,unsigned char Y,char *s)
{
    LCD_set_XY(X,Y);
    while (*s) 
    {
        LCD_write_char(*s);
        s++;
    }
}
/*-----------------------------------------------------------------------
LCD_write_chinese_string: ��LCD����ʾ����

���������X��Y    ����ʾ���ֵ���ʼX��Y���ꣻ
          ch_with �����ֵ���Ŀ��
          num     ����ʾ���ֵĸ�����  
          line    �����ֵ��������е���ʼ����
          row     ��������ʾ���м��
��д����          ��2004-8-11 
����޸�����      ��2004-8-12 
���ԣ�
	LCD_write_chi(0,0,12,7,0,0);
	LCD_write_chi(0,2,12,7,0,0);
	LCD_write_chi(0,4,12,7,0,0);	
-----------------------------------------------------------------------*/                        
void LCD_write_chinese_string(unsigned char X, unsigned char Y, 
                              unsigned char ch_with,unsigned char num,
                              unsigned char line,unsigned char row)
{
    unsigned char i,n;
    
    LCD_set_XY(X,Y);                             //���ó�ʼλ��
    
    for (i=0;i<num;)
    {
      	for (n=0; n<ch_with*2; n++)              //дһ������
        {
      	    if (n==ch_with)                      //д���ֵ��°벿��
            {
      	        if (i==0) LCD_set_XY(X,Y+1);
      	        else
                    LCD_set_XY((X+(ch_with+row)*i),Y+1);
            }
      	    LCD_write_byte(write_chinese[line+i][n],1);
        }
      	i++;
      	LCD_set_XY((X+(ch_with+row)*i),Y);
    }
}

/*-----------------------------------------------------------------------
LCD_move_chinese_string: �����ƶ�

���������X��Y    ����ʾ���ֵ���ʼX��Y���ꣻ
          T       ���ƶ��ٶȣ�

��д����          ��2004-8-13 
����޸�����      ��2004-8-13 
-----------------------------------------------------------------------*/                        
void chinese_string (unsigned char X, unsigned char Y, unsigned char T)
{
    unsigned char i,n,j=0;
    unsigned char buffer_h[84]={0};
    unsigned char buffer_l[84]={0};

    for (i=0; i<MAX_MOVE_TEXT; i++)
    {
        buffer_h[83] = move_chinese_string[i/12][j];
        buffer_l[83] = move_chinese_string[i/12][j+12];
        j++;
        if (j==12) j=0;
        
        for (n=0; n<83; n++)
        {
            buffer_h[n]=buffer_h[n+1];
            buffer_l[n]=buffer_l[n+1];
        }
        
        LCD_set_XY(X,Y);
        for (n=0; n<83; n++)
        {
            LCD_write_byte(buffer_h[n],1);
        }
        
        LCD_set_XY(X,Y+1); 
        for (n=0; n<83; n++)
        {
            LCD_write_byte(buffer_l[n],1);
        }

        delay_nms(T);
    }
}

/*-----------------------------------------------------------------------
LCD_draw_map      : λͼ���ƺ���

���������X��Y    ��λͼ���Ƶ���ʼX��Y���ꣻ
          *map    ��λͼ�������ݣ�
          Pix_x   ��λͼ���أ�����
          Pix_y   ��λͼ���أ���

��д����          ��2004-8-13
����޸�����      ��2004-8-13 
-----------------------------------------------------------------------*/
void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                        unsigned char Pix_x,unsigned char Pix_y)
{
    unsigned int i,n;
    unsigned char row;
    
    if (Pix_y%8==0) row=Pix_y/8;      //����λͼ��ռ����
    else
        row=Pix_y/8+1;
    
    for (n=0;n<row;n++)
    {
      	LCD_set_XY(X,Y);
        for(i=0; i<Pix_x; i++)
        {
            LCD_write_byte(map[i+n*Pix_x], 1);
        }
        Y++;                         //����
    }
}

/*-----------------------------------------------------------------------
LCD_write_byte    : ʹ��SPI�ӿ�д���ݵ�LCD

���������data    ��д������ݣ�
          command ��д����/����ѡ��

��д����          ��2004-8-10 
����޸�����      ��2004-8-13 
-----------------------------------------------------------------------*/
void LCD_write_byte(unsigned char dat, unsigned char command)
{
    unsigned char i;
    GPIO_ResetBits(GPIO_LCD_CE_PORT,GPIO_LCD_CE);//LCD_CE = 0;
    
    if (command == 0)
        GPIO_ResetBits(GPIO_LCD_DC_PORT,GPIO_LCD_DC);//LCD_DC = 0;
    else
        GPIO_SetBits(GPIO_LCD_DC_PORT,GPIO_LCD_DC);//LCD_DC = 1;
    for(i=0;i<8;i++)
    {
        if(dat&0x80)
            GPIO_SetBits(GPIO_SDIN_PORT,GPIO_SDIN);//SDIN = 1;
        else
            GPIO_ResetBits(GPIO_SDIN_PORT,GPIO_SDIN);//SDIN = 0;
        GPIO_ResetBits(GPIO_SCLK_PORT,GPIO_SCLK);//SCLK = 0;
        dat = dat << 1;
        GPIO_SetBits(GPIO_SCLK_PORT,GPIO_SCLK);//SCLK = 1;
    }
    GPIO_SetBits(GPIO_LCD_CE_PORT,GPIO_LCD_CE);//LCD_CE = 1;
}


