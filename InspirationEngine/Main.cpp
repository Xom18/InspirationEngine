#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAP_SIZE 30
#define TILE_SIZE 4
#include <SDL\SDL.h>
#include <thread>
#include <mutex>
#include <vector>
#include "MacroDefine.h"
#include "Input.h"
#include "Display.h"
#include "Image.h"
#include "File.h"
#include "Core.h"

int main(int argc, char* argv[])
{
	SDL_Event		SDL_Event;		//â �̺�Ʈó��
	std::thread*	InputThread;	//Ű���� �Է� ������, ���� ������ ȭ �ϱ� ��
	cDisplay		Display("Inspiration", SCREEN_WIDTH, SCREEN_HEIGHT, 2);	//â ����
	
	cImage TileSet; // Ÿ�ϼ��� �ִ� �̹���
	cTile Tile;		// Ÿ��

	cInput Input;	//Ű���� �Է�
	Input.start();

	//Ÿ�� �̹����� �ҷ��ͼ� Ÿ�� ����
	TileSet.readBMP("../Release/file.bmp");
	Tile.imageToTile(&TileSet, 16);
	
	SDL_SetTextureBlendMode(Display.getLayer(1), SDL_BLENDMODE_BLEND);

	while (!Display.closed())
	{
		SDL_Rect Rect;//Ÿ�� ��� ��ġ
		Rect.h = 16;
		Rect.w = 16;
		Rect.x = 1 * 16;
		Rect.y = 1 * 16;

		//���̾� �ؽ��� ���� �׸���
		SDL_UpdateTexture(Display.getLayer(0), &Rect, Tile.getData(12), 16 * sizeof(Uint32));

		SDL_WaitEventTimeout(&SDL_Event, 30);
		
		Display.render();
	}
	SDL_Quit();
	return 0;
}