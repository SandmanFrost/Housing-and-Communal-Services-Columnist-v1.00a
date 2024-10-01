#include "core.h"

WNDCLASS NewWndClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst,
                     HICON Icon, LPCSTR Name, WNDPROC Procedure) {
  WNDCLASS nwc = {0};
  nwc.hbrBackground = BGColor;
  nwc.hCursor = Cursor;
  nwc.hInstance = hInst;
  nwc.hIcon = Icon;
  nwc.lpszClassName = Name;
  nwc.lpfnWndProc = Procedure;
  return nwc;
}

// public

void Browser::PreProcess() { std::filesystem::create_directory(L"data src\\"); }

void Browser::MenuFileNewTap(HWND hWnd) {
  IsButton(false);
  UpdateValueType(true);
  UpdateDateType(false);
  UpdateDate(hWnd);
  DestrWidget();
}

void Browser::MenuFileOpenTap(HWND hWnd) {
  ReadInFile();
  IsButton(false);
  UpdateValueType(false);
  UpdateDateType(false);
  UpdateDate(hWnd);
  DestrWidget();
}

void Browser::MenuFileCloseTap(HWND hWnd) {
  UpdateDateType(false);
  UpdateDate(hWnd);
  DestrWidget();
}

void Browser::ButtonOpenTap(HWND hWnd) {
  IsButton(true);
  UpdateValueType(false);
  UpdateDateType(true);
  UpdateDate(hWnd);
  DestrWidget();
  UpdateWindow(hWnd);
}

void Browser::Create(HWND hWnd) {
  UpdateValueType(true);
  UpdateDateType(false);
  StartWindow(hWnd);
}

void Browser::StartWindow(HWND hWnd) {
  AddMenu(hWnd);
  AddDate(hWnd);
}

void Browser::UpdateWindow(HWND hWnd) {
  UpdateDate(hWnd);
  DestrWidget();
  AddStaticWidget(hWnd);
  AddWidgetValue(hWnd);
  if (is_new_) AddButton(hWnd);
}

void Browser::IsButton(bool s) { is_button_ = s; }

void Browser::UpdateValueType(bool is_new) {
  if (!is_button_) is_new_ = is_new;
  if (is_new_) {
    block_value_type_ = edit_;
    value_str_.clear();
    value_str_.resize(21, empty_);
  }
  if (!is_new_) {
    block_value_type_ = static_;
    value_str_.clear();
    value_str_.resize(21, zero_);
  }
}

void Browser::UpdateDate(HWND hWnd) {
  DestrDate();
  AddDateVal(hWnd);
}

void Browser::UpdateDateType(bool is_open) {
  is_open_ = is_open;
  if (is_open_ && is_new_) {
    date_type_ = static_;
  } else {
    date_type_ = edit_;
    year_.clear();
    month_.clear();
  }
}

void Browser::DestrWidget() {
  for (HWND h : static_name_) {
    DestroyWindow(h);
  }
  static_name_.clear();
  for (HWND h : block_value_) {
    DestroyWindow(h);
  }
  block_value_.clear();
  DestroyWindow(block_total_);
  block_total_ = NULL;
  DestroyWindow(ButtonSave_);
  ButtonSave_ = NULL;
}

void Browser::DestrDate() {
  wchar_t s1[20] = L"";
  LPWSTR str1 = s1;
  int sl1 = GetWindowTextLengthW(block_year_) + 1;
  sl1 = GetWindowTextW(block_year_, str1, sl1);
  std::wstring value1(str1);
  year_ = value1;
  DestroyWindow(block_year_);
  block_year_ = NULL;
  wchar_t s2[20] = L"";
  LPWSTR str2 = s2;
  int sl2 = GetWindowTextLengthW(block_month_) + 1;
  sl2 = GetWindowTextW(block_month_, str2, sl2);
  std::wstring value2(str2);
  month_ = value2;
  DestroyWindow(block_month_);
  block_month_ = NULL;
}

void Browser::SaveTemporary(HWND hWnd) { SaveStruct(hWnd); }

void Browser::SaveInFile(HWND hWnd) {
  for (auto [year, value] : in_file_) {
    std::wstring buff = L"data src/" + year;
    const wchar_t* path = buff.c_str();
    std::wofstream input(path, std::ios::binary);
    if (!input) {
      MessageBoxW(hWnd, L"Сохранить в файл не удалось", L"Ошибка", MB_OK);
    } else {
      for (auto [id, struc] : value) {
        double d = 0.0;
        input << id << L" " << struc.month << L" ";
        for (std::wstring str : struc.value) {
          d += WstringToDouble(str);
          input << str << L" ";
        }
        input << d;
        input << std::endl;
      }
      input.close();
    }
  }
}

void Browser::ReadInFile() {
  for (int i = 0; i < 3000; ++i) {
    filedata value;
    std::wstring str;
    int id;
    std::wstring year = std::to_wstring(i);
    std::wstring buff = L"data src\\" + year;
    const wchar_t* path = buff.c_str();
    std::wifstream output(path);
    if (output) {
      while (std::getline(output, str)) {
        std::vector<std::wstring> out = SplitString(str);
        int i = 1;
        for (std::wstring s : out) {
          if (i == 1) {
            id = std::stoi(s);
            ++i;
            continue;
          }
          if (i == 2) {
            value.month = s;
            ++i;
            continue;
          }
          value.value.push_back(s);
          value.total = s;
          ++i;
        }
        in_file_[year].insert({id, value});
        value.value.clear();
      }
      output.close();
    } else {
      PreProcess();
    }
  }
}

// private

void Browser::AddMenu(HWND hWnd) {
  HMENU root = CreateMenu();
  HMENU sub_file = CreateMenu();

  AddMenuFile(root, sub_file);
  SetMenu(hWnd, root);
}

void Browser::AddMenuFile(HMENU root, HMENU sub) {
  AppendMenuW(sub, MF_STRING, MenuFileNew, L"Новый");
  AppendMenuW(sub, MF_STRING, MenuFileOpen, L"Просмотр");
  // AppendMenuW(sub, MF_STRING, MenuFileCraft, L"Черновик");
  AppendMenu(sub, MF_SEPARATOR, 0, NULL);
  AppendMenuW(sub, MF_STRING, MenuFileSave, L"Сохранить");
  AppendMenuW(sub, MF_STRING, MenuFileClose, L"Закрыть");

  AppendMenuW(root, MF_POPUP, (UINT_PTR)sub, L"Файл");
}

void Browser::AddDate(HWND hWnd) {
  CreateWindowW(L"static", L"Месяц:", StlStandart, X_POS_MONTH_STATIC,
                Y_POS_STANDART, Width_month, Height_, hWnd, NULL, NULL, NULL);
  CreateWindowW(L"static", L"Год:", StlStandart, X_POS_YEAR_STATIC,
                Y_POS_STANDART, Width_year, Height_, hWnd, NULL, NULL, NULL);
  CreateWindowW(L"button", L"Открыть", StlStandart, X_POS_BUTTON_OPEN,
                Y_POS_BUTTON_OPEN, Width_button_open, Height_, hWnd,
                (HMENU)ButtonOpen, NULL, NULL);
  AddDateVal(hWnd);
}

void Browser::AddDateVal(HWND hWnd) {
  std::wstring t = date_type_;
  LPCWSTR type = t.c_str();
  std::wstring m = month_;
  LPCWSTR month = m.c_str();
  block_month_ =
      CreateWindowW(type, month, StlStandart, X_POS_MONTH_EDIT, Y_POS_STANDART,
                    Width_month_value, Height_, hWnd, NULL, NULL, NULL);
  std::wstring y = year_;
  LPCWSTR year = y.c_str();
  block_year_ =
      CreateWindowW(type, year, StlNumber, X_POS_YEAR_EDIT, Y_POS_STANDART,
                    Width_year_value, Height_, hWnd, NULL, NULL, NULL);
}

void Browser::AddStaticWidget(HWND hWnd) {
  LPCWSTR type = L"static";
  int y = 25, i = 0;
  static_name_.push_back(CreateWindowW(type, types_of_services_[i], StlStaticC,
                                       X_POS_STANDART, y, Width_chapter,
                                       Height_, hWnd, NULL, NULL, NULL));
  y += 20;
  ++i;
  for (; i < 9; ++i, y += 20) {
    static_name_.push_back(
        CreateWindowW(type, types_of_services_[i], StlStaticL, X_POS_STANDART,
                      y, Width_names, Height_, hWnd, NULL, NULL, NULL));
    static_name_.push_back(CreateWindowW(type, currency_, StlStaticL,
                                         X_POS_CURRENCY, y, Width_currency,
                                         Height_, hWnd, NULL, NULL, NULL));
  }
  static_name_.push_back(CreateWindowW(type, types_of_services_[i], StlStaticC,
                                       X_POS_STANDART, y, Width_chapter,
                                       Height_, hWnd, NULL, NULL, NULL));
  y += 20;
  ++i;
  for (; i < 18; ++i, y += 20) {
    static_name_.push_back(
        CreateWindowW(type, types_of_services_[i], StlStaticL, X_POS_STANDART,
                      y, Width_names, Height_, hWnd, NULL, NULL, NULL));
    static_name_.push_back(CreateWindowW(type, currency_, StlStaticL,
                                         X_POS_CURRENCY, y, Width_currency,
                                         Height_, hWnd, NULL, NULL, NULL));
  }
  static_name_.push_back(CreateWindowW(type, types_of_services_[i], StlStaticC,
                                       X_POS_STANDART, y, Width_chapter,
                                       Height_, hWnd, NULL, NULL, NULL));
  y += 20;
  ++i;
  for (; i < 21; ++i, y += 20) {
    static_name_.push_back(
        CreateWindowW(type, types_of_services_[i], StlStaticL, X_POS_STANDART,
                      y, Width_names, Height_, hWnd, NULL, NULL, NULL));
    static_name_.push_back(CreateWindowW(type, currency_, StlStaticL,
                                         X_POS_CURRENCY, y, Width_currency,
                                         Height_, hWnd, NULL, NULL, NULL));
  }
  static_name_.push_back(CreateWindowW(type, types_of_services_[i], StlStaticC,
                                       X_POS_STANDART, y, Width_chapter,
                                       Height_, hWnd, NULL, NULL, NULL));
  y += 20;
  ++i;
  for (; i < 25; ++i, y += 20) {
    static_name_.push_back(
        CreateWindowW(type, types_of_services_[i], StlStaticL, X_POS_STANDART,
                      y, Width_names, Height_, hWnd, NULL, NULL, NULL));
    static_name_.push_back(CreateWindowW(type, currency_, StlStaticL,
                                         X_POS_CURRENCY, y, Width_currency,
                                         Height_, hWnd, NULL, NULL, NULL));
  }
  static_name_.push_back(CreateWindowW(type, types_of_services_[i], StlStaticC,
                                       X_POS_STANDART, y, Width_chapter,
                                       Height_, hWnd, NULL, NULL, NULL));
  static_name_.push_back(CreateWindowW(type, currency_, StlStaticL,
                                       X_POS_CURRENCY, y, Width_currency,
                                       Height_, hWnd, NULL, NULL, NULL));
}

void Browser::AddWidgetValue(HWND hWnd) {
  total_value_ = L"00.00";
  if (!in_file_.empty() && !is_new_) {
    for (auto& [year, strukt] : in_file_) {
      if (year == year_) {
        for (auto& [id, str] : strukt) {
          if (IdMonth() == id) {
            value_str_.clear();
            value_str_ = str.value;
            total_value_ = str.total;
          }
        }
      }
    }
  }
  int y = 45, i = 0;
  LPWSTR type = const_cast<LPWSTR>(block_value_type_.c_str());
  for (; i < 8; ++i, y += 20) {
    LPWSTR stat = const_cast<LPWSTR>(value_str_[i].c_str());
    block_value_.push_back(CreateWindowW(type, stat, StlEdit, X_POS_VALUE, y,
                                         Width_value, Height_, hWnd, NULL, NULL,
                                         NULL));
  }
  y += 20;
  for (; i < 16; ++i, y += 20) {
    LPWSTR stat = const_cast<LPWSTR>(value_str_[i].c_str());
    block_value_.push_back(CreateWindowW(type, stat, StlEdit, X_POS_VALUE, y,
                                         Width_value, Height_, hWnd, NULL, NULL,
                                         NULL));
  }
  y += 20;
  for (; i < 18; ++i, y += 20) {
    LPWSTR stat = const_cast<LPWSTR>(value_str_[i].c_str());
    block_value_.push_back(CreateWindowW(type, stat, StlEdit, X_POS_VALUE, y,
                                         Width_value, Height_, hWnd, NULL, NULL,
                                         NULL));
  }
  y += 20;
  for (; i < 21; ++i, y += 20) {
    LPWSTR stat = const_cast<LPWSTR>(value_str_[i].c_str());
    block_value_.push_back(CreateWindowW(type, stat, StlEdit, X_POS_VALUE, y,
                                         Width_value, Height_, hWnd, NULL, NULL,
                                         NULL));
  }
  LPWSTR total = const_cast<LPWSTR>(total_value_.c_str());
  block_total_ = CreateWindowW(L"static", total, StlEdit, X_POS_VALUE, y,
                               Width_value, Height_, hWnd, NULL, NULL, NULL);
}

void Browser::AddButton(HWND hWnd) {
  ButtonSave_ = CreateWindowW(
      L"button", L"Записать", StlStandart, X_POS_BUTTON_SAVE, Y_POS_BUTTON_SAVE,
      Width_button_save, Height_, hWnd, (HMENU)ButtonSave, NULL, NULL);
}

void Browser::SaveYear() {
  wchar_t s[20] = L"";
  LPWSTR str = s;
  int save_l = GetWindowTextLengthW(block_year_) + 1;
  save_l = GetWindowTextW(block_year_, str, save_l);
  std::wstring year(str);
  year_ = year;
}

void Browser::SaveMonth() {
  wchar_t s[20] = L"";
  LPWSTR str = s;
  int save_l = GetWindowTextLengthW(block_month_) + 1;
  save_l = GetWindowTextW(block_month_, str, save_l);
  std::wstring month(str);
  month_ = month;
}

void Browser::SaveValue() {
  if (!value_str_.empty()) value_str_.clear();
  for (HWND h : block_value_) {
    wchar_t s[20] = L"";
    LPWSTR str = s;
    int save_lenth = GetWindowTextLengthW(h) + 1;
    save_lenth = GetWindowTextW(h, str, save_lenth);
    std::wstring value(str);
    if (value == L"") value = L"0,00";
    value_str_.push_back(value);
  }
}

void Browser::SaveStruct(HWND hWnd) {
  std::wstring year;
  int id;
  filedata fd;
  SaveMonth();
  id = IdMonth();
  if (id == 0) {
    MessageBoxW(hWnd, L"Неправильно указан месяц", L"Ошибка", MB_OK);
  } else {
    SaveYear();
    year = year_;
    fd.month = month_;
    for (auto [tyear, tid] : in_file_) {
      if (tyear == year) {
        for (auto [mo, va] : tid) {
          if (mo == id) {
            in_file_[tyear].erase(mo);
          }
        }
      }
    }
    SaveValue();
    DoubleToString(fd);
    value_.clear();
    in_file_[year].insert({id, fd});
    MessageBoxW(hWnd,
                L"Записано в буфер.\nДля сохранения буфера в "
                L"файл воспользуйтесь меню:\nФайл -> сохранить.",
                L"Запись в буфер", MB_OK);
  }
}

void Browser::DoubleToString(filedata& fd) {
  double ttl = 0.00;
  for (std::wstring str : value_str_) {
    fd.value.push_back(str);
    value_.push_back(WstringToDouble(str));
    ttl += WstringToDouble(str);
  }
  int t = ttl * 100;
  ttl = t / 100.00;
  std::wstring buf = std::to_wstring(ttl);
  int j = 2;
  bool pnt = false;
  for (wchar_t c : buf) {
    if (c == L',' || c == L'.') pnt = true;
    fd.total += c;
    if (j == 0) break;
    if (pnt) --j;
  }
}

int Browser::IdMonth() {
  int id = 1;
  std::wstring month = ToLower(month_);
  for (const std::wstring str : string_month_low_) {
    if (str == month) return id;
    ++id;
  }
  return 0;
}

std::wstring Browser::ToLower(std::wstring str) {
  std::wstring out;
  for (wchar_t c : str) {
    c = towlower(c);
    out += c;
  }
  return out;
}

std::vector<std::wstring> Browser::SplitString(std::wstring str) {
  std::vector<std::wstring> out;
  std::wstring word;
  for (wchar_t c : str) {
    if (c == L' ') {
      if (!word.empty()) {
        out.push_back(word);
        word.clear();
      }
    } else {
      word += c;
    }
  }
  if (!word.empty()) {
    out.push_back(word);
  }
  return out;
}

double Browser::WstringToDouble(std::wstring str) {
  std::wstring s = str;
  double d = 0.0;
  int i = 0;
  if (IsNumber(s)) {
    for (wchar_t c : str) {
      if (c == L',') s[i] = L'.';
      ++i;
    }
    std::wstringstream convStream;
    convStream << s;
    convStream >> d;
  }
  return d;
}

bool Browser::IsNumber(std::wstring str) {
  for (wchar_t c : str) {
    if (c == L'.' || c == ',') {
      continue;
    } else {
      if (L'0' <= c && c <= L'9') {
        continue;
      } else {
        return false;
      }
    }
  }
  return true;
}

/*Create fron SandmanFrost(S.Barskiy)*/