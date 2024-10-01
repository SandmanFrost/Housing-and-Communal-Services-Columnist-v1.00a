#include "core.h"

Browser core;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args,
                   int ncmdshow) {
  setlocale(0, "");
  core.ReadInFile();
  WNDCLASS MainClass =
      NewWndClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst,
                  LoadIcon(NULL, IDI_QUESTION), "MainWndClass", MainProc);
  if (!RegisterClass(&MainClass)) return -1;
  MSG MainMsg = {0};
  HWND MainhWnd = GetConsoleWindow();
  ShowWindow(MainhWnd, SW_HIDE);
  CreateWindowW(L"MainWndClass",
                L"Обозреватель по жилищно-коммунальному хозяйству",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE, 50, 50, 600, 700, NULL, NULL,
                NULL, NULL);
  while (GetMessage(&MainMsg, NULL, 0, 0)) {
    TranslateMessage(&MainMsg);
    DispatchMessage(&MainMsg);
  }
  return 0;
}

LRESULT CALLBACK MainProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
  switch (msg) {
    case WM_COMMAND:
      switch (wp) {
        case MenuFileNew:
          core.MenuFileNewTap(hWnd);
          break;
        case MenuFileOpen:
          core.MenuFileOpenTap(hWnd);
          break;
        case MenuFileSave:
          MessageBoxW(hWnd, L"Сохранено в файл.", L"Сохранение", MB_OK);
          core.SaveInFile(hWnd);
          break;
        case MenuFileClose:
          core.MenuFileCloseTap(hWnd);
          break;
        case ButtonOpen:
          core.ButtonOpenTap(hWnd);
          break;
        case ButtonSave:
          MessageBoxW(hWnd,
                      L"Сохранено в буфер.\nДля сохранения буфера в "
                      L"файл\nвоспользуйтесь меню: Файл -> сохранить.",
                      L"Сохранение", MB_OK);
          core.SaveTemporary(hWnd);
          break;
        default:
          break;
      }
      break;
    case WM_CREATE:
      core.Create(hWnd);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, msg, wp, lp);
  }
  return DefWindowProc(hWnd, msg, wp, lp);
}

/*Create fron SandmanFrost*/