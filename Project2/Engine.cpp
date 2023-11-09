﻿#include "framework.h"
#include "Engine.h"


Engine::Engine(Common* pcommon) : m_pDirect2dFactory(NULL), m_pRenderTarget(NULL), m_pWhiteBrush(NULL)
{
    //snake = new Snake();
    //food = new Food();
    //food->Reset(snake , isFoodOnBorderChecked );
    common = pcommon;
    playing = false;

    WindScore = 0;
    Score = 15000;
}

Engine::~Engine()
{
    SafeRelease(&m_pDirect2dFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pWhiteBrush);
}

HRESULT Engine::InitializeD2D(HWND m_hwnd)
{
    // 初始化 Direct2D
    D2D1_SIZE_U size = D2D1::SizeU(SCREEN_WIDTH, SCREEN_HEIGHT);
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
    m_pDirect2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(m_hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
        &m_pRenderTarget
    );

    // Initialize text writing factory and format
    DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(m_pDWriteFactory),
        reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
    );

    m_pDWriteFactory->CreateTextFormat(
        L"Verdana",
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        20,
        L"", //locale
        &m_pTextFormat
    );

    //m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); //置中
    m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING); //靠右

    m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        &m_pWhiteBrush
    );

    IWICImagingFactory* pIWICFactory = NULL;
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pIWICFactory);
    int errorcode = 0;

    // 將讀取所有圖檔
    std::wstring path = common->currentPath.wstring() + L"\\Images\\Apple.png";
    Common::LoadBitmapFromFile(m_pRenderTarget, pIWICFactory, path, 0, 0, &AppleBitmap,m_hwnd, errorcode);
    if (errorcode != 0) { //TODO::error處理
        ;
    }
    path = common->currentPath.wstring() + L"\\Images\\Bar.png";
    Common::LoadBitmapFromFile(m_pRenderTarget, pIWICFactory, path, 0, 0, &BarBitmap, m_hwnd, errorcode);
    if (errorcode != 0) {
        ;
    }
    path = common->currentPath.wstring() + L"\\Images\\Bell.png";
    Common::LoadBitmapFromFile(m_pRenderTarget, pIWICFactory, path, 0, 0, &BellBitmap, m_hwnd, errorcode);
    if (errorcode != 0) {
        ;
    }
    path = common->currentPath.wstring() + L"\\Images\\Lemon.png";
    Common::LoadBitmapFromFile(m_pRenderTarget, pIWICFactory, path, 0, 0, &LemonBitmap, m_hwnd, errorcode);
    if (errorcode != 0) {
        ;
    }
    path = common->currentPath.wstring() + L"\\Images\\Orange.png";
    Common::LoadBitmapFromFile(m_pRenderTarget, pIWICFactory, path, 0, 0, &OrangeBitmap, m_hwnd, errorcode);
    if (errorcode != 0) {
        ;
    }
    path = common->currentPath.wstring() + L"\\Images\\Seven.png";
    Common::LoadBitmapFromFile(m_pRenderTarget, pIWICFactory, path, 0, 0, &SevenBitmap, m_hwnd, errorcode);
    if (errorcode != 0) {
        ;
    }
    path = common->currentPath.wstring() + L"\\Images\\Star.png";
    Common::LoadBitmapFromFile(m_pRenderTarget, pIWICFactory, path, 0, 0, &StarBitmap, m_hwnd, errorcode);
    if (errorcode != 0) {
        ;
    }
    path = common->currentPath.wstring() + L"\\Images\\Watermelon.png";
    Common::LoadBitmapFromFile(m_pRenderTarget, pIWICFactory, path, 0, 0, &WatermelonBitmap, m_hwnd, errorcode);
    if (errorcode != 0) {
        ;
    }
    path = common->currentPath.wstring() + L"\\Images\\Background.png";
    Common::LoadBitmapFromFile(m_pRenderTarget, pIWICFactory, path, 0, 0, &BackgroundBitmap, m_hwnd, errorcode);
    if (errorcode != 0) {
        ;
    }
    path = common->currentPath.wstring() + L"\\Images\\Mid_Background.png";
    Common::LoadBitmapFromFile(m_pRenderTarget, pIWICFactory, path, 0, 0, &Mid_BackgroundBitmap, m_hwnd, errorcode);
    if (errorcode != 0) {
        ;
    }
    // init map
    Bet_call_map.clear();
    for (int i = 0; i < CELL_TOTAL; ++i) {
        Bet_call_map.insert({ i, 0 });
    }

    // 讀取倍率表
    std::string path1 = common->currentPath.string() + "/Images/倍率表.json";
    std::ifstream inFile(path1);
    if (!inFile.is_open()) {
        //std::cerr << "無法打開 JSON 文件" << std::endl;
        //OutputDebugString(L"JSON開啟成功\n");
        MessageBox(m_hwnd, L"倍率表讀取失敗", L"錯誤", MB_OK);
        return 1;
    }
    //else
        //OutputDebugString(L"JSON開啟成功\n");
    json Data;
    inFile >> Data;
    inFile.close();
    BettingTable = Data;

    // 初始化燈亮時間

    for (int i = 0; i < 8; ++i) {
        Bet_Light_map[i] = 0;
    }

    for (int i = 0; i < 24; ++i) {
        Game_Light_map[i] = 0;
    }

    for (int i = 0; i < 2; ++i) {
        Compare_Light_map[i] = 0;
    }
    
    pIWICFactory->Release();
    return S_OK;
}

void Engine::KeyUp(WPARAM wParam)
{

}

void Engine::Reset()
{
    // This method reset the game, given that the game was won or lost
    if (!playing)
    {
        //snake->Reset();
        //food->Reset(snake, isFoodOnBorderChecked );
        //score = 5;
    }
}

void Engine::Logic(double elapsedTime)
{
    // This is the logic part of the engine.
    if (playing)
    {
        //更新現在時間
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() % 1000;
        currentTime = seconds * 1000 + milliseconds;

        updateLightStatus();
        if (starting) {
            for (int i = 0; i < 24; ++i) {
                SetLightStatus(Game_Light_call_map, currentTime + 100 * i, i, 1);
                SetLightStatus(Game_Light_call_map, currentTime + 100 * (i + 1), i, 0);
            }

            starting = 0;
        }
    }

    //    if (snake->CheckSelfCollision())
    //    {
    //        playing = false;
    //    }
    //    else if (food->GameWin)
    //    {
    //        playing = false;
    //        //TODO:寫入勝利畫面
    //    }

    //}
}
int Engine::Get_CellScore(int number) {
    switch (number)
    {
    case APPLE_NUMBER: {
        return 5;
    }
                     break;
    case BAR_NUMBER: {
        return 100;
    }
                   break;
    case BELL_NUMBER: {
        return 20;
    }
                    break;
    case LEMON_NUMBER: {
        return 15;
    }
                     break;
    case ORANGE_NUMBER: {
        return 10;
    }
                      break;
    case SEVEN_NUMBER: {
        return 40;
    }
                     break;
    case STAR_NUMBER: {
        return 30;
    }
                    break;
    case WATERMELOM_NUMBER: {
        return 20;
    }
                          break;

    default:
        break;
    }
}

// 投入金額 0為清空
void Engine::Bet_call(int number, int amount) {
    // 檢查是否存在該號碼的下注紀錄
    auto it = Bet_call_map.find(number);
    if (it != Bet_call_map.end()) {
        // 如果該號碼已存在，更新下注金額
        int cost = Get_CellScore(number);
        if (amount == 0) {
            AddScore(cost*it->second);
            it->second = 0;
        }
        else {
            if(CostScore(cost * amount))
                it->second += amount;
            else
                MessageBox(NULL, L"金額不足", L"錯誤", MB_OK);
        }
    }
    else {
        // 如果該號碼不存在，新增一筆下注紀錄
        MessageBox(NULL, L"號碼不存在", L"錯誤", MB_OK);
        //Bet_call_map[number] = amount;
    }
}


HRESULT Engine::Draw()
{

    auto frameStart = std::chrono::steady_clock::now();

    // This is the drawing method of the engine.
    // It simply draws all the elements in the game using Direct2D
    //HRESULT hr;

    m_pRenderTarget->BeginDraw();


    m_pRenderTarget->Clear(D2D1::ColorF(176.0f/255.0f, 151.0f / 205, 95.0f / 255.0f,1.0f));

    //繪製圍牆
    //D2D1_RECT_F bound_rectangle = D2D1::RectF(1.0f, 1.0f, SCREEN_WIDTH - 3, SCREEN_HEIGHT - 3);
    ID2D1SolidColorBrush* pBlackBrush;
    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), &pBlackBrush);
    //m_pRenderTarget->DrawRectangle(&bound_rectangle, pBlackBrush, 7.0f);

    D2D1_RECT_F destinationRect = D2D1::RectF(
        1,
        1,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    m_pRenderTarget->DrawBitmap(BackgroundBitmap, destinationRect);



    // UI設計基準紅線 未來刪除
    D2D1_RECT_F rectangle4 = D2D1::RectF(SCREEN_WIDTH/2-1, 1.0f, SCREEN_WIDTH/2+1, SCREEN_HEIGHT - 3);
    ID2D1SolidColorBrush* pBrush;
    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pBrush);
    m_pRenderTarget->FillRectangle(&rectangle4, pBrush);

    // Draw score
    D2D1_RECT_F win_rectangle     = D2D1::RectF(SCREEN_WIDTH / 3  -120 , 70, SCREEN_WIDTH / 3  +120, 150);
    D2D1_RECT_F credits_rectangle = D2D1::RectF(SCREEN_WIDTH / 3*2-120 , 70, SCREEN_WIDTH / 3*2+120, 150);
    WCHAR scoreStr[64];
    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBrush);

    swprintf_s(scoreStr, L"BONUS/WIN 贏得分數 \n %d                                         ", WindScore);
    m_pRenderTarget->DrawText(
        scoreStr,
        48,
        m_pTextFormat,
        win_rectangle,
        pBrush
    );
    swprintf_s(scoreStr, L"CREDITS 目前總分數 \n   %d                                      ", Score);
    m_pRenderTarget->DrawText(
        scoreStr,
        48,
        m_pTextFormat,
        credits_rectangle,
        pBrush
    );

    // 遊戲區域
    Draw_Game(GRID_X, GRID_Y, GRID_WIDTH, GRID_HEIGHT);
    // 下注區域
    Draw_Bet(BET_X, BET_Y, BET_WIDTH, BET_HEIGHT);
    // 功能鍵
    Draw_Function(FUNCTION_X, FUNCTION_Y, FUNCTION_WIDTH, FUNCTION_HEIGHT);


    //m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &pBrush);
    //m_pRenderTarget->FillRectangle(&function_rectangle, pBrush);

    m_pRenderTarget->EndDraw();

    return S_OK;
}

void Engine::ClearDraw(HWND hWnd){
    m_pRenderTarget->BeginDraw();
    m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));  // 以白色清空背景
    m_pRenderTarget->EndDraw();
    InvalidateRect(hWnd, NULL, TRUE);
}

void Engine::Draw_Cell(int number, D2D1_RECT_F originalRect) {

    D2D1_RECT_F Rect;
    // 將原本給的originalRect範圍縮小
    float shrinkAmount = 10.0f;
    Rect.left = originalRect.left + shrinkAmount;
    Rect.top = originalRect.top + shrinkAmount;
    Rect.right = originalRect.right - shrinkAmount;
    Rect.bottom = originalRect.bottom - shrinkAmount;


    switch (number)
    {
    case APPLE_NUMBER: {
        m_pRenderTarget->DrawBitmap(AppleBitmap, Rect);

        }
        break;
    case BAR_NUMBER: {
        m_pRenderTarget->DrawBitmap(BarBitmap, Rect);

    }
        break;
    case BELL_NUMBER: {
        m_pRenderTarget->DrawBitmap(BellBitmap, Rect);

    }
        break;
    case LEMON_NUMBER: {
        m_pRenderTarget->DrawBitmap(LemonBitmap, Rect);

    }
        break;
    case ORANGE_NUMBER: {
        m_pRenderTarget->DrawBitmap(OrangeBitmap, Rect);

    }
        break;
    case SEVEN_NUMBER: {
        m_pRenderTarget->DrawBitmap(SevenBitmap, Rect);

    }
        break;
    case STAR_NUMBER: {
        m_pRenderTarget->DrawBitmap(StarBitmap, Rect);

    }
        break;
    case WATERMELOM_NUMBER: {
        m_pRenderTarget->DrawBitmap(WatermelonBitmap, Rect);

    }
        break;
    default:
        break;
    }
}
void Engine::Draw_Game(int x, int y, int width, int height) {
    ID2D1SolidColorBrush* pBrush;

    D2D1_RECT_F game_rectangle = D2D1::RectF(x + width * 1, y + height * 1, x + width * 6, y + height * 6);
    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &pBrush);
    //m_pRenderTarget->FillRectangle(&game_rectangle, pBrush);
    ///正中間背景
    m_pRenderTarget->DrawBitmap(Mid_BackgroundBitmap, game_rectangle);
    // 比大小區域
    D2D1_RECT_F game_guess_rectangle = D2D1::RectF(x + width * 3, y + height * 4.5, x + width * 4, y + height * 5);
    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(55.0f / 255.0f, 22.0f / 205, 20.0f / 255.0f, 1.0f)), &pBrush);
    m_pRenderTarget->FillRectangle(&game_guess_rectangle, pBrush);
    WCHAR scoreStr[64];
    int guess = 0; //TODO 隨機數
    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightPink), &pBrush);
    m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); //置中
    swprintf_s(scoreStr, L"%d                                                  ", guess);
    m_pRenderTarget->DrawText(
        scoreStr,
        30,
        m_pTextFormat,
        game_guess_rectangle,
        pBrush
    );
    if (isLight(Compare_Light_map, 0)) { //TODO 判斷大小燈亮
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pBrush);
        m_pRenderTarget->FillEllipse(
            D2D1::Ellipse(D2D1::Point2F(x + width * 2.75, y + height * 4.75), LIGHT_SIZE, LIGHT_SIZE),
            pBrush);
    }
    else {
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkRed), &pBrush);
        m_pRenderTarget->FillEllipse(
            D2D1::Ellipse(D2D1::Point2F(x + width * 2.75, y + height * 4.75), LIGHT_SIZE, LIGHT_SIZE),
            pBrush);
    }
    if (isLight(Compare_Light_map, 1)) { //TODO 判斷大小燈亮
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pBrush);
        m_pRenderTarget->FillEllipse(
            D2D1::Ellipse(D2D1::Point2F(x + width * 4.25, y + height * 4.75), LIGHT_SIZE, LIGHT_SIZE),
            pBrush);
    }
    else {
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkRed), &pBrush);
        m_pRenderTarget->FillEllipse(
            D2D1::Ellipse(D2D1::Point2F(x + width * 4.25, y + height * 4.75), LIGHT_SIZE, LIGHT_SIZE),
            pBrush);
    }




    for (int j = 0; j < GAME_SIZE; ++j) { //遊戲區域分成7*7
        for (int i = 0; i < GAME_SIZE; ++i) {
            if ((i == 0 || i == GAME_SIZE-1) ||
                (j == 0 || j == GAME_SIZE-1)) {
                //判斷是否燈亮
                int light_number = -1;
                if (j == 0 && i == 0) {
                    // (0, 0) 處理邏輯
                    light_number = 0;
                }
                else if (j == 0 && i == 1) {
                    // (0, 1) 處理邏輯
                    light_number = 1;
                }
                else if (j == 0 && i == 2) {
                    // (0, 2) 處理邏輯
                    light_number = 2;
                }
                else if (j == 0 && i == 3) {
                    // (0, 3) 處理邏輯
                    light_number = 3;
                }
                else if (j == 0 && i == 4) {
                    // (0, 4) 處理邏輯
                    light_number = 4;
                }
                else if (j == 0 && i == 5) {
                    // (0, 5) 處理邏輯
                    light_number = 5;
                }
                else if (j == 0 && i == 6) {
                    // (0, 6) 處理邏輯
                    light_number = 6;
                }
                else if (j == 1 && i == 6) {
                    // (1, 6) 處理邏輯
                    light_number = 7;
                }
                else if (j == 2 && i == 6) {
                    // (2, 6) 處理邏輯
                    light_number = 8;
                }
                else if (j == 3 && i == 6) {
                    // (3, 6) 處理邏輯
                    light_number = 9;
                }
                else if (j == 4 && i == 6) {
                    // (4, 6) 處理邏輯
                    light_number = 10;
                }
                else if (j == 5 && i == 6) {
                    // (5, 6) 處理邏輯
                    light_number = 11;
                }
                else if (j == 6 && i == 6) {
                    // (6, 6) 處理邏輯
                    light_number = 12;
                }
                else if (j == 6 && i == 5) {
                    // (6, 5) 處理邏輯
                    light_number = 13;
                }
                else if (j == 6 && i == 4) {
                    // (6, 4) 處理邏輯
                    light_number = 14;
                }
                else if (j == 6 && i == 3) {
                    // (6, 3) 處理邏輯
                    light_number = 15;
                }
                else if (j == 6 && i == 2) {
                    // (6, 2) 處理邏輯
                    light_number = 16;
                }
                else if (j == 6 && i == 1) {
                    // (6, 1) 處理邏輯
                    light_number = 17;
                }
                else if (j == 6 && i == 0) {
                    // (6, 0) 處理邏輯
                    light_number = 18;
                }
                else if (j == 5 && i == 0) {
                    // (5, 0) 處理邏輯
                    light_number = 19;
                }
                else if (j == 4 && i == 0) {
                    // (4, 0) 處理邏輯
                    light_number = 20;
                }
                else if (j == 3 && i == 0) {
                    // (3, 0) 處理邏輯
                    light_number = 21;
                }
                else if (j == 2 && i == 0) {
                    // (2, 0) 處理邏輯
                    light_number = 22;
                }

                else if (j == 1 && i == 0) {
                    // (1, 0) 處理邏輯
                    light_number = 23;
                }
                else {
                    // 其他格子的處理邏輯
                    light_number = -1;
                }
                D2D1_RECT_F grid_rectangle = D2D1::RectF(x + width * i, y + height * j,
                    x + width * (i + 1), y + height * (j + 1));

                if (isLight(Game_Light_map, light_number)) {
                    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &pBrush);
                }
                else {
                    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrush);
                }
                m_pRenderTarget->FillRectangle(&grid_rectangle, pBrush);
                //邊界
                m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBrush);
                m_pRenderTarget->DrawRectangle(&grid_rectangle, pBrush, 2.0f);

                switch (light_number)
                {
                case 0: {
                    // (0, 0) 處理邏輯
                    Draw_Cell(ORANGE_NUMBER, grid_rectangle);
                    break;
                }
                case 1: {
                    // (0, 1) 處理邏輯
                    Draw_Cell(BELL_NUMBER, grid_rectangle);
                    break;
                }
                case 2: {
                    // (0, 2) 處理邏輯
                    Draw_Cell(BAR_NUMBER, grid_rectangle);
                    break;
                }
                case 3: {
                    // (0, 3) 處理邏輯
                    Draw_Cell(BAR_NUMBER, grid_rectangle);
                    break;
                }
                case 4: {
                    // (0, 4) 處理邏輯
                    Draw_Cell(APPLE_NUMBER, grid_rectangle);
                    break;
                }
                case 5: {
                    // (0, 5) 處理邏輯
                    Draw_Cell(APPLE_NUMBER, grid_rectangle);
                    break;
                }
                case 6: {
                    // (0, 6) 處理邏輯
                    Draw_Cell(LEMON_NUMBER, grid_rectangle);
                    break;
                }
                case 7: {
                    // (1, 6) 處理邏輯
                    Draw_Cell(WATERMELOM_NUMBER, grid_rectangle);
                    break;
                }
                case 8: {
                    // (2, 6) 處理邏輯
                    Draw_Cell(WATERMELOM_NUMBER, grid_rectangle);
                    break;
                }
                case 9: {
                    // (3, 6) 處理邏輯
                    WCHAR scoreStr[64];
                    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), &pBrush);
                    m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); //置中
                    swprintf_s(scoreStr, L"ONCE\nMORE                      ");
                    m_pRenderTarget->DrawText(
                        scoreStr,
                        30,
                        m_pTextFormat,
                        grid_rectangle,
                        pBrush
                    );
                    break;
                }
                case 10: {
                    // (4, 6) 處理邏輯
                    Draw_Cell(APPLE_NUMBER, grid_rectangle);
                    break;
                }
                case 11: {
                    // (5, 6) 處理邏輯
                    Draw_Cell(ORANGE_NUMBER, grid_rectangle);
                    break;
                }
                case 12: {
                    // (6, 6) 處理邏輯
                    Draw_Cell(ORANGE_NUMBER, grid_rectangle);
                    break;
                }
                case 13: {
                    // (6, 5) 處理邏輯
                    Draw_Cell(BELL_NUMBER, grid_rectangle);
                    break;
                }
                case 14: {
                    // (6, 4) 處理邏輯
                    Draw_Cell(SEVEN_NUMBER, grid_rectangle);
                    break;
                }
                case 15: {
                    // (6, 3) 處理邏輯
                    Draw_Cell(SEVEN_NUMBER, grid_rectangle);
                    break;
                }
                case 16: {
                    // (6, 2) 處理邏輯
                    Draw_Cell(APPLE_NUMBER, grid_rectangle);
                    break;
                }
                case 17: {
                    // (6, 1) 處理邏輯
                    Draw_Cell(LEMON_NUMBER, grid_rectangle);
                    break;
                }
                case 18: {
                    // (6, 0) 處理邏輯
                    Draw_Cell(LEMON_NUMBER, grid_rectangle);
                    break;
                }
                case 19: {
                    // (5, 0) 處理邏輯
                    Draw_Cell(STAR_NUMBER, grid_rectangle);
                    break;
                }
                case 20: {
                    // (4, 0) 處理邏輯
                    Draw_Cell(STAR_NUMBER, grid_rectangle);
                    break;
                }
                case 21: {
                    // (3, 0) 處理邏輯
                    WCHAR scoreStr[64];
                    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pBrush);
                    m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); //置中
                    swprintf_s(scoreStr, L"ONCE\nMORE                      ");
                    m_pRenderTarget->DrawText(
                        scoreStr,
                        30,
                        m_pTextFormat,
                        grid_rectangle,
                        pBrush
                    );
                    light_number = 21;
                    break;
                }
                case 22: {
                    // (2, 0) 處理邏輯
                    Draw_Cell(APPLE_NUMBER, grid_rectangle);
                    light_number = 22;
                    break;
                }
                case 23: {
                    // (1, 0) 處理邏輯
                    Draw_Cell(BELL_NUMBER, grid_rectangle);
                    light_number = 23;
                    break;
                }
                default:
                    // 其他格子的處理邏輯
                    light_number = -1;
                    break;
                }

                //製作燈號
                if (isLight(Game_Light_map, light_number)) {
                    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pBrush);
                }
                else {
                    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkRed), &pBrush);
                }
                m_pRenderTarget->FillEllipse(
                    D2D1::Ellipse(D2D1::Point2F(x + width * (i + 0.5), y + height * (j + 1) - 8), LIGHT_SIZE, LIGHT_SIZE),
                    pBrush);
            }
        }
    }
}
void Engine::Draw_Bet(int x, int y, int width, int height) {
    ID2D1SolidColorBrush* pBrush;

    D2D1_RECT_F bet_rectangle = D2D1::RectF(x, y, x + width * 8, y + 200);
    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &pBrush);
    m_pRenderTarget->FillRectangle(&bet_rectangle, pBrush);

    for (int i = 0; i < BET_SIZE; ++i) { //下注區域為8種選項
        //頂部分數
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGreen), &pBrush);
        D2D1_RECT_F grid_top_rectangle = D2D1::RectF(x + width * (i), y,
            x + width * (i + 1), y + 50);
        m_pRenderTarget->FillRectangle(&grid_top_rectangle, pBrush);
        grid_top_rectangle = D2D1::RectF(x + width * (i), y + 10,
            x + width * (i + 1), y + 50);
        WCHAR scoreStr[64];
        int m_score = Get_CellScore(i);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBrush);
        m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); //置中
        swprintf_s(scoreStr, L"%d                                                  ", m_score);
        m_pRenderTarget->DrawText(
            scoreStr,
            30, 
            m_pTextFormat,
            grid_top_rectangle,
            pBrush
        );

        //下注數
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(55.0f / 255.0f, 22.0f / 205, 20.0f / 255.0f, 1.0f)), &pBrush);
        D2D1_RECT_F grid_bottom_rectangle = D2D1::RectF(x + width * (i), y + 50,
            x + width * (i + 1), y + 124);
        m_pRenderTarget->FillRectangle(&grid_bottom_rectangle, pBrush);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightPink), &pBrush);
        int bet = Bet_call_map[i];
        swprintf_s(scoreStr, L"%d                                                  ", bet);
        m_pRenderTarget->DrawText(
            scoreStr,
            38,
            m_pTextFormat,
            grid_bottom_rectangle,
            pBrush
        );
        //圖片
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrush);
        D2D1_RECT_F grid_png_rectangle = D2D1::RectF(x + width * (i), y + 124,
            x + width * (i + 1), y + 200);
        m_pRenderTarget->FillRectangle(&grid_png_rectangle, pBrush);
        Draw_Cell(i, grid_png_rectangle);


        //分界線
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &pBrush);
        D2D1_RECT_F grid_rectangle = D2D1::RectF(x + width * (i + 1) - 1, y,
            x + width * (i + 1), y + 200);
        if (i != BET_SIZE - 1)
            m_pRenderTarget->FillRectangle(&grid_rectangle, pBrush);

        if (isLight(Bet_Light_map , i)) { //TODO 判斷是否旋轉到此號碼
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pBrush);
            m_pRenderTarget->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(x + width * (i + 0.5), y + 8), LIGHT_SIZE, LIGHT_SIZE),
                pBrush);
        }
        else {
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkRed), &pBrush);
            m_pRenderTarget->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(x + width * (i + 0.5), y + 8), LIGHT_SIZE, LIGHT_SIZE),
                pBrush);
        }
    }
}


void Engine::Draw_Function(int x, int y, int width, int height) {    
    D2D1_RECT_F function_rectangle = D2D1::RectF(x, y, x + width * 8, y + height); // 8為還原width
    ID2D1SolidColorBrush* pBrush;
    ID2D1SolidColorBrush* p_Pen_Brush;
    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &p_Pen_Brush);
    WCHAR scoreStr[64];
    m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); //置中

    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &pBrush);
    m_pRenderTarget->FillRectangle(&function_rectangle, pBrush);

    for (int i = 0; i < 8; ++i) {
        D2D1_RECT_F bet_rectangle = D2D1::RectF(x + width *i , y, x + width * (i + 1), y + height * 0.5);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBrush);
        m_pRenderTarget->DrawRectangle(&bet_rectangle, pBrush, 4.0f);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrush);
        m_pRenderTarget->FillRectangle(&bet_rectangle, pBrush);

        D2D1_RECT_F Add_1_rectangle  = D2D1::RectF(x + width * i        , y                , x + width * (i + 0.5), y + height * 0.25);
        D2D1_RECT_F Add_10_rectangle = D2D1::RectF(x + width * (i + 0.5), y                , x + width * (i + 1)  , y + height * 0.25);
        D2D1_RECT_F Clear_rectangle  = D2D1::RectF(x + width * i        , y + height * 0.25, x + width * (i + 1)  , y + height * 0.5);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBrush);
        m_pRenderTarget->DrawRectangle(&Add_1_rectangle , pBrush, 2.0f);
        m_pRenderTarget->DrawRectangle(&Add_10_rectangle, pBrush, 2.0f);

        swprintf_s(scoreStr, L"1                                                  ");
        m_pRenderTarget->DrawText(
            scoreStr,
            30,
            m_pTextFormat,
            Add_1_rectangle,
            p_Pen_Brush
        );
        swprintf_s(scoreStr, L"10                                                  ");
        m_pRenderTarget->DrawText(
            scoreStr,
            30,
            m_pTextFormat,
            Add_10_rectangle,
            p_Pen_Brush
        );
        swprintf_s(scoreStr, L"CLEAR                                                  ");
        m_pRenderTarget->DrawText(
            scoreStr,
            30,
            m_pTextFormat,
            Clear_rectangle,
            p_Pen_Brush
        );
    }

    

    // 分配功能鍵區域
    // TODO 這裡*的比例為人工計算,具體按鍵公式可在調整
    D2D1_RECT_F exit_rectangle  = D2D1::RectF(x + width * 0.225, y + height * BET_RATIO, x + width * 1.525, y + height);
    D2D1_RECT_F small_rectangle = D2D1::RectF(x + width * 1.725, y + height * BET_RATIO, x + width * 2.625, y + height);
    D2D1_RECT_F big_rectangle   = D2D1::RectF(x + width * 2.825, y + height * BET_RATIO, x + width * 3.725, y + height);
    D2D1_RECT_F score_rectangle = D2D1::RectF(x + width * 3.925, y + height * BET_RATIO, x + width * 5.225, y + height);
    D2D1_RECT_F auto_rectangle  = D2D1::RectF(x + width * 5.425, y + height * BET_RATIO, x + width * 6.325, y + height);
    D2D1_RECT_F start_rectangle = D2D1::RectF(x + width * 6.525, y + height * BET_RATIO, x + width * 7.825, y + height);


    //繪製綠色按鈕
    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &pBrush);
    m_pRenderTarget->FillRectangle(&exit_rectangle, pBrush);
    swprintf_s(scoreStr, L"離開遊戲\nEXIT                                                  ");
    m_pRenderTarget->DrawText(
        scoreStr,
        30,
        m_pTextFormat,
        exit_rectangle,
        p_Pen_Brush
    );
    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &pBrush);
    m_pRenderTarget->FillRectangle(&start_rectangle, pBrush);
    swprintf_s(scoreStr, L"開始\nSTART                                                  ");
    m_pRenderTarget->DrawText(
        scoreStr,
        30,
        m_pTextFormat,
        start_rectangle,
        p_Pen_Brush
    );

    //繪製黃色按鈕
    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &pBrush);
    m_pRenderTarget->FillRectangle(&small_rectangle, pBrush);
    swprintf_s(scoreStr, L"小\nSMALL                                                  ");
    m_pRenderTarget->DrawText(
        scoreStr,
        30,
        m_pTextFormat,
        small_rectangle,
        p_Pen_Brush
    );

    m_pRenderTarget->FillRectangle(&big_rectangle, pBrush);
    swprintf_s(scoreStr, L"大\nBIG                                                  ");
    m_pRenderTarget->DrawText(
        scoreStr,
        30,
        m_pTextFormat,
        big_rectangle,
        p_Pen_Brush
    );
    m_pRenderTarget->FillRectangle(&auto_rectangle, pBrush);
    swprintf_s(scoreStr, L"自動\nAUTO                                                  ");
    m_pRenderTarget->DrawText(
        scoreStr,
        30,
        m_pTextFormat,
        auto_rectangle,
        p_Pen_Brush
    );

    //繪製紅色按鈕
    m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pBrush);
    m_pRenderTarget->FillRectangle(&score_rectangle, pBrush);
    swprintf_s(scoreStr, L"得分\nSCORE                                                  ");
    m_pRenderTarget->DrawText(
        scoreStr,
        30,
        m_pTextFormat,
        score_rectangle,
        p_Pen_Brush
    );







}

bool Engine::isLight(std::map<int, bool> map, int number) {
    auto it = map.find(number);
    if (it != map.end()) {
        return it->second;
    }
    // 如果找不到對應的鍵值，也返回 false
    return false;
}

void Engine::SetLightStatus(std::multimap<int, std::pair<int, bool>> &map, int time, int number, bool islight) {
    std::pair<int, bool> ppair;
    ppair.first = number;
    ppair.second = islight;
    map.insert(std::make_pair(time, ppair));
}


void  Engine::updateLightStatus() {
    for (auto it = Bet_Light_call_map.begin(); it != Bet_Light_call_map.end();) {
        if (it->first <= currentTime) {
            // 更新 Bet_Light_map
            Bet_Light_map[it->second.first] = it->second.second;
            // 刪除已更新的資訊
            it = Bet_Light_call_map.erase(it);
        }
        else {
            ++it;
        }
    }

    for (auto it = Game_Light_call_map.begin(); it != Game_Light_call_map.end();) {
        if (it->first <= currentTime) {
            // 更新 Bet_Light_map
            Game_Light_map[it->second.first] = it->second.second;

            //std::wstring numberStr = std::to_wstring(it->second.first);
            //MessageBox(NULL, numberStr.c_str(), L"測試", MB_OK);
            //numberStr = std::to_wstring(it->second.second);
            //MessageBox(NULL, numberStr.c_str(), L"測試", MB_OK);

            // 刪除已更新的資訊
            it = Game_Light_call_map.erase(it);
        }
        else {
            ++it;
        }
    }

    for (auto it = Compare_Light_call_map.begin(); it != Compare_Light_call_map.end();) {
        if (it->first <= currentTime) {
            // 更新 Bet_Light_map
            Compare_Light_map[it->second.first] = it->second.second;
            // 刪除已更新的資訊
            it = Compare_Light_call_map.erase(it);
        }
        else {
            ++it;
        }
    }
}