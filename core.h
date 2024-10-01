#pragma once

#include <windows.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

// Определения стилей блоков

#define StlStandart WS_VISIBLE | WS_CHILD
#define StlStaticC StlStandart | ES_CENTER
#define StlStaticL StlStandart | ES_LEFT
#define StlStaticR StlStandart | ES_RIGHT
#define StlEdit StlStandart | ES_RIGHT
#define StlNumber StlStandart | ES_NUMBER

// Определения размеров блоков

#define Height_ 17            // Стандартная высота
#define Width_chapter 575     // Ширина разделов
#define Width_names 325       // Ширина наименований
#define Width_currency 55     // Ширина валюты
#define Width_value 65        // Ширина полей значений
#define Width_month 50        // Ширина поля месяца
#define Width_month_value 80  // Ширина поля месяц значение
#define Width_year 30         // Ширина поля год
#define Width_year_value 35   // Ширина поля год значение
#define Width_button_open 70  // Ширина кнопки открыть
#define Width_button_save 80  // Ширина кнопки сохранить

// Определения позиций

#define X_POS_STANDART 5  // Стартовая позиция окон по X
#define Y_POS_STANDART 5  // Стартовая позиция окон по Y

#define X_POS_MONTH_STATIC 5  // Стартовая позиция месяца наименование
#define X_POS_MONTH_EDIT 60  // Стартовая позиция месяца значение
#define X_POS_YEAR_STATIC 145  // Стартовая позиция года наименование
#define X_POS_YEAR_EDIT 180  // Стартовая позиция года значение

#define X_POS_VALUE 335  // Стартовая позиция значений
#define X_POS_CURRENCY 405  // Стартовая позиция валюты

#define X_POS_BUTTON_OPEN 230  // Стартовая позиция кнопки открыть
#define X_POS_BUTTON_SAVE 5  // Стартовая позиция кнопки сохранить
#define Y_POS_BUTTON_OPEN 5  // Стартовая позиция кнопки открыть
#define Y_POS_BUTTON_SAVE 545  // Стартовая позиция кнопки сохранить

// Определение значений меню и виджетов

#define MenuFileNew 1000    // Файл -> Новый
#define MenuFileOpen 1001   // Файл -> Открыть
#define MenuFileCraft 1002  // Файл -> Черновик
#define MenuFileSave 1003   // Файл -> Сохранить
#define MenuFileClose 1004  // Файл -> Закрыть

#define ButtonOpen 2000  // Кнопка "Открыть"
#define ButtonSave 2001  // Кнопка "Сохранить"

// Внешние функции

LRESULT CALLBACK MainProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWndClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst,
                     HICON Icon, LPCSTR Name, WNDPROC Procedure);

// Класс

struct filedata {
  std::wstring month;
  std::vector<std::wstring> value;
  std::wstring total;
};

class Browser {
 private:
  bool is_new_ = true;
  bool is_open_ = false;
  bool is_button_ = true;
  // map<year, map<id(month), month - vector(value)>>
  std::map<std::wstring, std::map<int, filedata>> in_file_;

  std::vector<HWND> block_value_;
  std::vector<std::wstring> value_str_;
  std::wstring block_value_type_;
  std::vector<double> value_;

  HWND block_total_;
  std::wstring total_value_ = L"00.00";
  double total_;

  HWND block_year_;
  std::wstring year_;

  HWND block_month_;
  std::wstring month_;
  const std::vector<std::wstring> string_month_low_ = {
      L"январь", L"февраль", L"март",     L"апрель",  L"май",    L"июнь",
      L"июль",   L"август",  L"сентябрь", L"октябрь", L"ноябрь", L"декабрь"};

  std::wstring date_type_ = L"edit";

  LPCWSTR currency_ = L"Рублей";

  std::wstring edit_ = L"edit";
  std::wstring static_ = L"static";
  std::wstring empty_ = L"";
  std::wstring zero_ = L"00.00";
  std::wstring path_ = L"src/";

  std::vector<HWND> static_name_;
  const std::vector<LPCWSTR> types_of_services_ = {
      L"Начисление за жилищные услуги",          // 0
      L"1.Взнос на капитальный ремонт",          // 1
      L"2.Водоотведение ОДН",                    // 2
      L"3.Горячее В/С (НОСИТЕЛЬ) ОДН",           // 3
      L"4.Горячее В/С (ЭНЕРГИЯ) ОДН",            // 4
      L"5.Запирающее устройство(домофон)",       // 5
      L"6.Содержание жилого помещения",          // 6
      L"7.Холодное В/С (НОСИТЕЛЬ) ОДН",          // 7
      L"8.Электроснабжение ОДН",                 // 8
      L"Начисления за коммунальные услуги",      // 9
      L"1.Водоотведение",                        // 10
      L"2.Газоснабжение",                        // 11
      L"3.Горячее В/С (НОСИТЕЛЬ)",               // 12
      L"4.Горячее В/С (ЭНЕРГИЯ)",                // 13
      L"5.Обращение с ТКО",                      // 14
      L"6.Отопление",                            // 15
      L"7.Холодное В/С",                         // 16
      L"8.Электричество однотарифный ПУ (Д1)",   // 17
      L"Начисления за иные услуги",              // 18
      L"1.ТО ВКГО",                              // 19
      L"2.ТО ВКГО за прошлый год",               // 20
      L"Иные услуги не включенные в квитанцию",  // 21
      L"1.Интернет",                             // 22
      L"2.Телефон (мобильная связь)",            // 23
      L"3.Подписки (онлайн)",                    // 24
      L"Итого:"                                  // 25
  };

  HWND ButtonSave_;

  void AddMenu(HWND hWnd);
  void AddMenuFile(HMENU root, HMENU sub);
  void AddDate(HWND hWnd);
  void AddDateVal(HWND hWnd);
  void AddStaticWidget(HWND hWnd);
  void AddWidgetValue(HWND hWnd);
  void AddButton(HWND hWnd);
  void SaveYear();
  void SaveMonth();
  void SaveValue();
  void SaveStruct(HWND hWnd);
  void DoubleToString(filedata& fd);
  int IdMonth();
  std::wstring ToLower(std::wstring str);
  std::vector<std::wstring> SplitString(std::wstring str);
  double WstringToDouble(std::wstring str);
  bool IsNumber(std::wstring str);

 public:
  void PreProcess();
  void MenuFileNewTap(HWND hWnd);
  void MenuFileOpenTap(HWND hWnd);
  void MenuFileCloseTap(HWND hWnd);
  void ButtonOpenTap(HWND hWnd);
  void Create(HWND hWnd);
  void SaveInFile(HWND hWnd);
  void ReadInFile();

  void StartWindow(HWND hWnd);
  void UpdateWindow(HWND hWnd);
  void IsButton(bool s);
  void UpdateValueType(bool is_new);
  void UpdateDate(HWND hWnd);
  void UpdateDateType(bool is_open);
  void DestrWidget();
  void DestrDate();
  void SaveTemporary(HWND hWnd);
};

/*Create fron SandmanFrost(S.Barskiy)*/