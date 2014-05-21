#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

//  レジスタブロックの物理アドレス
#define PERI_BASE     0x20000000
#define GPIO_BASE     (PERI_BASE + 0x200000)
#define BLOCK_SIZE    4096
//  gpio[n]: GPIO 関連レジスタ (volatile＝必ず実メモリにアクセスさせる)
static volatile unsigned int *gpio;
//  gpio_init: GPIO 初期化（最初に１度だけ呼び出すこと）
void gpio_init ()
{
  int fd;
  void *gpio_map;
  //  /dev/mem（物理メモリデバイス）を開く（sudo が必要）
  fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd == -1) {
    printf("error: cannot open /dev/mem (gpio_setup)\n");
    exit(-1);
  }
  //  mmap で GPIO（物理メモリ）を gpio_map（仮想メモリ）に対応づける
  gpio_map = mmap(NULL, BLOCK_SIZE,
		  PROT_READ | PROT_WRITE, MAP_SHARED,
		  fd, GPIO_BASE );
  if ((int) gpio_map == -1) {
    printf("error: cannot map /dev/mem on the memory (gpio_setup)\n");
    exit(-1);
  }
  //  mmap 後は不要な fd をクローズ
  close(fd);
  //  gpio[index]: 整数 uint32 の配列としてレジスタへのアクセスを確立
  gpio = (unsigned int *) gpio_map;
}
//  ピン機能（BCM2835）
#define GPIO_INPUT    0x0       //  入力
#define GPIO_OUTPUT   0x1       //  出力
#define GPIO_ALT0     0x4
#define GPIO_ALT1     0x5
#define GPIO_ALT2     0x6
#define GPIO_ALT3     0x7
#define GPIO_ALT4     0x3
#define GPIO_ALT5     0x2
//  gpio_configure: ピン機能を設定する（ピンを使用する前に必ず設定）
//      pin : (P1) 2,3,4,7,8,9,10,11,14,15,17,18,22,23,24,25,27
//            (P5) 28,29,30,31
//      mode: GPIO_INPUT, _OUTPUT, _ALT0, _ALT1, _ALT2, _ALT3, _ALT4, _ALT5
void gpio_configure (int pin, int mode)
{
  //  ピン番号チェック
  if (pin < 0 || pin > 31) {
    printf("error: pin number out of range (gpio_configure)\n");
    exit(-1);
  }
  //  レジスタ番号（index）と３ビットマスクを生成
  int index = pin / 10;
  unsigned int mask = ~(0x7 << ((pin % 10) * 3));
  //  GPFSEL0/1 の該当する FSEL (3bit) のみを書き換え
  gpio[index] = (gpio[index] & mask) | ((mode & 0x7) << ((pin % 10) * 3));
}
//  gpio_set/clear: ピンをセット (3.3V)，クリア (0V)
void gpio_set (int pin)
{
  //  ピン番号チェック（スピードを追求するなら省略してもよい）
  if (pin < 0 || pin > 31) {
    printf("error: pin number out of range (gpio_set)\n");
    exit(-1);
  }
  //  ピンに１を出力（3.3V 出力）
  gpio[7] = 0x1 << pin;   //  GPSET0
}
void gpio_clear (int pin)
{
  //  ピン番号チェック（スピードを追求するなら省略してもよい）
  if (pin < 0 || pin > 31) {
    printf("error: pin number out of range (gpio_clear)\n");
    exit(-1);
  }
  //  ピンに０を出力（0V 出力）
  gpio[10] = 0x1 << pin;  //  GPCLR0
}

int gpio_read (int pin)
{
  //  ピン番号チェック（スピードを追求するなら省略してもよい）
  if (pin < 0 || pin > 31) {
    printf("error: pin number out of range (gpio_read)\n");
    exit(-1);
  }
  //  ピンの電圧を返す（入力/出力を問わず 3.3V なら 1，0V なら 0）
  return (gpio[13] & (0x1 << pin)) != 0;  //  GPLEV0
}

#define OPM_D0 2
#define OPM_D1 3
#define OPM_D2 4
#define OPM_D3 7
#define OPM_D4 8
#define OPM_D5 9
#define OPM_D6 10
#define OPM_D7 11

#define OPM_IRQ 14
#define OPM_IC 15
#define OPM_A0 17
#define OPM_WR 18
#define OPM_RD 22
#define OPM_CS 23
#define LVL_OE 24

void set_data_io(int io)
{
  gpio_configure(OPM_D0, io);
  gpio_configure(OPM_D1, io);
  gpio_configure(OPM_D2, io);
  gpio_configure(OPM_D3, io);
  gpio_configure(OPM_D4, io);
  gpio_configure(OPM_D5, io);
  gpio_configure(OPM_D6, io);
  gpio_configure(OPM_D7, io);
}

void write_data(unsigned char d)
{
  if (d & 0x01) {
    gpio_set(OPM_D0);
  } else {
    gpio_clear(OPM_D0);
  }
  if (d & 0x02) {
    gpio_set(OPM_D1);
  } else {
    gpio_clear(OPM_D1);
  }
  if (d & 0x04) {
    gpio_set(OPM_D2);
  } else {
    gpio_clear(OPM_D2);
  }
  if (d & 0x08) {
    gpio_set(OPM_D3);
  } else {
    gpio_clear(OPM_D3);
  }
  if (d & 0x10) {
    gpio_set(OPM_D4);
  } else {
    gpio_clear(OPM_D4);
  }
  if (d & 0x20) {
    gpio_set(OPM_D5);
  } else {
    gpio_clear(OPM_D5);
  }
  if (d & 0x40) {
    gpio_set(OPM_D6);
  } else {
    gpio_clear(OPM_D6);
  }
  if (d & 0x80) {
    gpio_set(OPM_D7);
  } else {
    gpio_clear(OPM_D7);
  }
}

unsigned char read_data(void)
{
  unsigned char d = 0;

  if (gpio_read(OPM_D0)) {
    d |= 0x01;
  }
  if (gpio_read(OPM_D1)) {
    d |= 0x02;
  }
  if (gpio_read(OPM_D2)) {
    d |= 0x04;
  }
  if (gpio_read(OPM_D3)) {
    d |= 0x08;
  }
  if (gpio_read(OPM_D4)) {
    d |= 0x10;
  }
  if (gpio_read(OPM_D5)) {
    d |= 0x20;
  }
  if (gpio_read(OPM_D6)) {
    d |= 0x40;
  }
  if (gpio_read(OPM_D7)) {
    d |= 0x80;
  }

  return d;
}

unsigned char read_mem(unsigned int *p)
{
  unsigned char d = 0;

  if (*p & (1 << 2)) {
    d |= 0x01;
  }
  if (*p & (1 << 3)) {
    d |= 0x02;
  }
  if (*p & (1 << 4)) {
    d |= 0x04;
  }
  if (*p & (1 << 7)) {
    d |= 0x08;
  }
  if (*p & (1 << 8)) {
    d |= 0x10;
  }
  if (*p & (1 << 9)) {
    d |= 0x20;
  }
  if (*p & (1 << 10)) {
    d |= 0x40;
  }
  if (*p & (1 << 11)) {
    d |= 0x80;
  }

  return d;
}

void main ()
{
  int i, sock, fd, rsize, retval;
  struct sockaddr_in sa;
  char buf[2];

  gpio_init();

  set_data_io(GPIO_OUTPUT);
  write_data(0x0);

  gpio_configure(OPM_IC, GPIO_OUTPUT);
  gpio_configure(OPM_A0, GPIO_OUTPUT);
  gpio_configure(OPM_WR, GPIO_OUTPUT);
  gpio_configure(OPM_RD, GPIO_OUTPUT);
  gpio_configure(OPM_CS, GPIO_OUTPUT);

  gpio_set(OPM_IC);
  gpio_set(OPM_CS);
  gpio_set(OPM_WR);
  gpio_set(OPM_RD);

  printf("start\n");

  usleep(100000);                 //  0.1秒待ち
  gpio_clear(OPM_IC);
  usleep(100000);                 //  0.1秒待ち
  gpio_set(OPM_IC);
  usleep(100000);                 //  0.1秒待ち

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock ==  -1) {
    printf("socket error\n");
    exit(1);
  }
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(2151);
  sa.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sock, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) == -1) {
    printf("bind error\n");
    close(sock);
    exit(1);
  }

  if (listen(sock, 128) == -1) {
    printf("listen error\n");
    close(sock);
    exit(1);
  }

  printf("listened\n");

  fd = accept(sock, NULL, NULL);
  if (fd == -1) {
    printf("accept error\n");
    close(sock);
    exit(1);
  }

  printf("accepted\n");

  while (1) {
    memset(buf, 0, sizeof(buf));
    rsize = recv(fd, buf, sizeof(buf), 0);
    if (rsize > 0) {
      //printf("%d 0x%x\n", buf[0], buf[1]);

      write_data(buf[1]);
      usleep(1);

      if (buf[0] == 1) {
	gpio_clear(OPM_A0);
      } else {
	gpio_set(OPM_A0);
      }

      gpio_clear(OPM_CS);
      usleep(1);
      gpio_clear(OPM_WR);
      usleep(2);
      gpio_set(OPM_WR);
      usleep(1);
      gpio_set(OPM_CS);
      usleep(1);
      usleep(256);
    }
  }

  close(fd);
  close(sock);
  exit(0);
}

