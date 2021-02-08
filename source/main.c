/*---------------------------------------------------------------------------------

	Basic template codefor starting a DS app

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
#include <gl2d.h>

#include "kimi_datta.h"

void initSubSprites(void);
//---------------------------------------------------------------------------------
int main(void) {
  //---------------------------------------------------------------------------------
  videoSetMode(MODE_5_3D);
  videoSetModeSub(MODE_5_2D);

  int frame = 0;
  int topShift = 0;
  int bottomShift = 0;

  glInit();

  initSubSprites();
  bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

  //-------------------------------------------------------
  //	 Setup gl
  //-------------------------------------------------------
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_ANTIALIAS);

  glClearColor(0, 0, 0, 31);
  glClearPolyID(63);
  glClearDepth(0x7FFF);

  glViewport(0, 0, 255, 191);
  vramSetBankA(VRAM_A_TEXTURE);

  int textureID;
  glGenTextures(1, & textureID);
  glBindTexture(0, textureID);
  glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_128, TEXTURE_SIZE_128, 0, TEXGEN_TEXCOORD, (u8 * ) kimi_dattaBitmap);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70, 256.0 / 128.0, 0.1, 100);

  gluLookAt(0.0, 0.0, 2.0, //camera possition 
    0.0, 0.0, 0.0, //look at
    0.0, 1.0, 0.0); //up	

  glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

  bool top = true;

  while (1) {
    top = !top;
	frame = (frame + 1) % 360;
	topShift = (topShift+1) % 200;
	bottomShift = (bottomShift+1) % 150;
    scanKeys();
    while (REG_DISPCAPCNT & DCAP_ENABLE);

    if (top) {
      lcdMainOnBottom();
      vramSetBankC(VRAM_C_LCD);
      vramSetBankD(VRAM_D_SUB_SPRITE);
      REG_DISPCAPCNT = DCAP_BANK(2) | DCAP_ENABLE | DCAP_SIZE(3);
    } else {
      lcdMainOnTop();
      vramSetBankD(VRAM_D_LCD);
      vramSetBankC(VRAM_C_SUB_BG);
      REG_DISPCAPCNT = DCAP_BANK(3) | DCAP_ENABLE | DCAP_SIZE(3);
    }
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0, 0, -2);
    glRotatef32i(degreesToAngle(0), inttof32(1), inttof32(1), inttof32(1));

    glMaterialf(GL_AMBIENT, RGB15(16, 16, 16));
    glMaterialf(GL_DIFFUSE, RGB15(16, 16, 16));
    glMaterialf(GL_SPECULAR, BIT(15) | RGB15(8, 8, 8));
    glMaterialf(GL_EMISSION, RGB15(16, 16, 16));
    if (top) {
      glRotateX(-40);
	  glRotateY(frame);
      glBegin(GL_QUAD);
      	glNormal(NORMAL_PACK(0, inttov10(-100), 0));

		GFX_TEX_COORD = (TEXTURE_PACK(0, inttot16(128)));
		glVertex3f(-10.0f, 0.0f+abs(100-topShift)*0.01f, -10.0f);

		GFX_TEX_COORD = (TEXTURE_PACK(inttot16(128), inttot16(128)));
		glVertex3f(10.0f, 0.0f+abs(100-topShift)*0.01f, -10.0f);

		GFX_TEX_COORD = (TEXTURE_PACK(inttot16(128), 0));
		glVertex3f(10.0f, 0.0f+abs(100-topShift)*0.01f, 10.0f);

		GFX_TEX_COORD = (TEXTURE_PACK(0, 0));
		glVertex3f(-10.0f, 0.0f+abs(100-topShift)*0.01f, 10.0f);
      glEnd();
    } else {
      glRotateX(40);
	  glRotateY(frame);
      glBegin(GL_QUADS);
		glColor3b(0, 0, 255); glVertex3f(-10.0f, -0.0f+abs(75-bottomShift)*0.01f, 10.0f);
		glColor3b(0, 255, 0); glVertex3f(10.0f, -0.0f+abs(75-bottomShift)*0.01f, 10.0f);
		glColor3b(255, 0, 0); glVertex3f(10.0f, -0.0f+abs(75-bottomShift)*0.01f, -10.0f);
		glColor3b(255, 255, 0); glVertex3f(-10.0f, -0.0f+abs(75-bottomShift)*0.01f, -10.0f);
      glEnd();
    }

    glPopMatrix(1);
    glFlush(0);
  }

  return 0;

}

void initSubSprites(void) {

  oamInit( & oamSub, SpriteMapping_Bmp_2D_256,false);

  int x = 0;
  int y = 0;

  int id = 0;

  //set up a 4x3 grid of 64x64 sprites to cover the screen
  for (y = 0; y < 3; y++)
    for (x = 0; x < 4; x++) {
      oamSub.oamMemory[id].attribute[0] = ATTR0_BMP | ATTR0_SQUARE | (64 * y);
      oamSub.oamMemory[id].attribute[1] = ATTR1_SIZE_64 | (64 * x);
      oamSub.oamMemory[id].attribute[2] = ATTR2_ALPHA(1) | (8 * 32 * y) | (8 * x);
      id++;
    }

  swiWaitForVBlank();

  oamUpdate( & oamSub);
}