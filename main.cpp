#include <windows.h>
#include <string.h>
#include <time.h>
#pragma warning (disable: 4996)
const char windowClass[] = "win32app";
const char windowTitle[] = "Game BALL";

int  size_box = 60, x_box = 0, y_box = 0, dy = 0, d = 0;
int  size_ball = 20, x_ball = -size_ball, y_ball = -size_ball;

long score = 0;
bool field;
int R = rand() % 256, G = rand() % 256, B = rand() % 256;

int x = 1;
// Оконная процедура
long __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	GetClientRect(hWnd, &rect);

	HPEN pen, white_pen, system_pen;
	HBRUSH brush, system_brush;
	char buff[20];
	switch (message)
	{
	case WM_PAINT:
		// Всегда должна быть в начале
		hdc = BeginPaint(hWnd, &ps);
		white_pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		pen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		brush = CreateSolidBrush(RGB(R, G, B));
		system_pen = (HPEN)SelectObject(hdc, pen);

		// Прорисовка Счёта
		ltoa(score, buff, 20);
		TextOut(hdc, 0, 0, "Счёт: ", 6);
		TextOut(hdc, 40, 0, buff, strlen(buff));

		// Линия раздела
		MoveToEx(hdc, (rect.left + rect.right) / 2, 0, NULL);
		LineTo(hdc, (rect.left + rect.right) / 2, rect.bottom);
		SelectObject(hdc, system_pen);
		DeleteObject(pen);

		// Квадрат и шарик
		system_brush = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, rect.right - size_box, rect.top + dy, rect.right, rect.top + size_box + dy);
		Rectangle(hdc, x_ball - size_ball, y_ball - size_ball, x_ball + size_ball, y_ball + size_ball);

		// Подчистка мусора (чтобы не оставался след от движения коробок)
		SelectObject(hdc, white_pen);
		SelectObject(hdc, system_brush);
		Rectangle(hdc, x_ball - size_ball - 5, y_ball - size_ball, x_ball - size_ball, y_ball + size_ball);
		Rectangle(hdc, rect.right - size_box, rect.top + dy - 3, rect.right, rect.top + dy);
		Rectangle(hdc, rect.right - size_box, rect.top + dy + size_box, rect.right, rect.top + size_box + dy + 3);

		SelectObject(hdc, system_pen);
		DeleteObject(brush);
		DeleteObject(white_pen);

		// Конец рисования
		EndPaint(hWnd, &ps);
		break;

	case WM_LBUTTONDOWN:
		// Если щелкаем на правой половине поля -- игнор
		if (LOWORD(lParam) > (rect.left + rect.right) / 2) break;
		x_ball = LOWORD(lParam);
		y_ball = HIWORD(lParam);
		InvalidateRect(hWnd, NULL, true);
		break;

	case WM_RBUTTONDOWN:
		R = rand() % 256;
		G = rand() % 256;
		B = rand() % 256;
		InvalidateRect(hWnd, NULL, false);
		break;

	case WM_TIMER:
		// Поездка вверх, вниз
		if (dy >= (rect.bottom - size_box)) d = -3;
		if (dy == rect.top) d = 3;
		if (wParam == 1)
		{
			dy = dy + d;
			InvalidateRect(hWnd, NULL, false);
		}

		// Смещение вправо для кубика, который "бросаем"
		if (wParam == 2)
		{
			x_ball = x_ball + 5;
			InvalidateRect(hWnd, NULL, false);
		}

		// Каждую секунду проверяем, нет ли столкновнеия
		if (wParam == 666)
		{
			// Если координаты Х сторон пересеклись
			if (((x_ball + size_ball) >= (rect.right - size_box)) && ((x_ball - size_ball) <= (rect.right)))
			{
				// Проверка крайних точек сторон
				if (rect.top + dy < y_ball + size_ball)
				if (y_ball - size_ball <= (rect.top + dy + size_box))
				{
					score++;
					dy = 0;
					x_ball = -size_ball * 2;
					y_ball = -size_ball * 2;

					// Меняем цвет коробок
					R = rand() % 256;
					G = rand() % 256;
					B = rand() % 256;

					// Перерисовываем заний план поля только в том случае, если коробки столкнулись
					InvalidateRect(hWnd, NULL, true);
					break;
				}

				if ((y_ball - size_ball) < (rect.top + dy + size_box))
				if ((rect.top + dy) <= (y_ball + size_ball))
				{
					score++;
					dy = 0;
					x_ball = -size_ball * 2;
					y_ball = -size_ball * 2;

					// Меняем цвет коробок
					R = rand() % 256;
					G = rand() % 256;
					B = rand() % 256;

					// Перерисовываем заний план поля только в том случае, если коробки столкнулись
					InvalidateRect(hWnd, NULL, true);
					break;
				}
			}
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Точка входа
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(time(NULL));
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = windowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, "Can’t register window class!", "Win32 API Test", NULL);
		return 1;
	}

	HWND hWnd = CreateWindow(windowClass, windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 700, 600, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		MessageBox(NULL, "Can’t create window!", "Win32 API Test", NULL);
		return 1;
	}

	ShowWindow(hWnd, nCmdShow);

	// Создание таймера
	SetTimer(hWnd, 1, 10, NULL);
	SetTimer(hWnd, 2, 10, NULL);
	SetTimer(hWnd, 666, 10, NULL);
	UpdateWindow(hWnd);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	KillTimer(hWnd, 1);
	KillTimer(hWnd, 2);
	KillTimer(hWnd, 666);
	return msg.wParam;
}

