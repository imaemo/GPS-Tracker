
//牧码字模 0.88 测试版 
//免费软件 需要专业版与作者联系 老牧 email :mucode # 163.com 


//对齐方式:左上
//纵向取模 高位在下
//字体:宋体 大小12*12点阵 取模大小12*12点阵 
//字符数6 
// 1  2  3  4  5  6
// 测 试 显 示 函 数
const unsigned char FontLib[][24]=
{

{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

{0xF2,0x12,0x92,0x7E,0x92,0x12,0x92,0x7E,0x92,0x12,0xF2,0x00,
0x0F,0x01,0x00,0x00,0x02,0x01,0x00,0x00,0x00,0x09,0x0F,0x00},/*"两",0*/

{0x98,0xD4,0xB3,0x88,0x00,0x48,0x48,0xFF,0x24,0xA5,0x26,0x00,
0x04,0x04,0x02,0x02,0x08,0x08,0x04,0x03,0x05,0x08,0x0E,0x00},/*"线",1*/

{0x88,0xFF,0x48,0x00,0x4C,0xA4,0x95,0x86,0x94,0xA4,0x4C,0x00,
0x08,0x0F,0x00,0x00,0x08,0x08,0x08,0x0F,0x08,0x08,0x08,0x00},/*"控",2*/

{0x18,0xD6,0x54,0xFF,0x54,0xD4,0x10,0xFC,0x00,0x00,0xFF,0x00,
0x00,0x07,0x00,0x0F,0x04,0x07,0x00,0x01,0x08,0x08,0x0F,0x00},/*"制",3*/

{0x20,0x27,0xA5,0xA5,0x67,0x38,0x67,0xA5,0xAD,0x37,0x20,0x00,
0x01,0x0F,0x0A,0x0A,0x0E,0x00,0x0E,0x0A,0x0A,0x0F,0x01,0x00},/*"器",4*/

{0x38,0x00,0xFF,0x10,0x08,0x02,0x02,0x02,0xFE,0x02,0x02,0x00,
0x08,0x06,0x01,0x02,0x0C,0x00,0x08,0x08,0x0F,0x00,0x00,0x00},/*"灯",0*/

{0x22,0x44,0x10,0x08,0xF7,0x94,0x94,0xF4,0x04,0xFC,0x00,0x00,
0x04,0x02,0x01,0x00,0x07,0x08,0x08,0x08,0x09,0x09,0x0E,0x00},/*"泡",1*/

{0x82,0x82,0xBA,0xAA,0xAA,0xAB,0xAA,0xAA,0xBA,0x82,0x82,0x00,
0x09,0x08,0x06,0x02,0x02,0x02,0x02,0x06,0x08,0x08,0x0D,0x00},/*"亮",0*/

{0x82,0x72,0x02,0x02,0x82,0x7E,0x82,0x02,0x42,0x32,0x02,0x00,
0x08,0x08,0x04,0x02,0x01,0x00,0x01,0x02,0x04,0x08,0x08,0x00},/*"灭",1*/	

};