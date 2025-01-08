#include "InstructionPage.h"
#include "PageUtils.h"
#include "AnsiHelper.h"
#include "Utf8Symbols.h"
#include "IOHelper.h"

#define SET_COLOR_RED ESC_SEQ "31m"
#define SET_COLOR_BRIGHT_RED ESC_SEQ "91m"

#define RESET_COLOR ESC_SEQ "0m"

typedef struct {
    int terminalWidth;
    int terminalHeight;
    int pageNumber;
} InstructionPageData; 

void DrawUIPageOne(InstructionPageData* data);
void DrawUIPageTwo(InstructionPageData* data);
void DrawUIPageThree(InstructionPageData* data);
void DrawUI(InstructionPageData* data);
void OnInstructionPageResize(int width, int height, void* data);

void PageEnter_Instruction()
{
    DisableAlternativeBuffer();

    HideCursor();

    InstructionPageData* data = malloc(sizeof(InstructionPageData));
    data->pageNumber = 1;

    GetTerminalSize(&data->terminalWidth, &data->terminalHeight);
    DrawUI(data);

    SetResizeHandler(OnInstructionPageResize, data);

    while(true){
        KeyInputType key = HandleInteractions(true);
        switch (key)
        {
            case KEY_ENTER:
                break;
                
            case KEY_ARROW_LEFT: {
                data->pageNumber--;
                if(data->pageNumber < 1) data->pageNumber = 1;
                DrawUI(data);
                break;
            }
                
            case KEY_ARROW_RIGHT: {
                data->pageNumber++;
                if(data->pageNumber > 3) data->pageNumber = 3;
                DrawUI(data);
                break;
            }

            case KEY_ESCAPE: 
                UnsetResizeHandler();
                free(data);
                ClearScreenManual();
                EnableAlternativeBuffer();
                return;
            
            default:
                break;
        }
    }
}

void DrawUIPageOne(InstructionPageData* data) {
    //ClearScreen();
    
    int tmpY = 2;
    const int widthWithoutBorders = data->terminalWidth - 4;
    int tmp;

    PRINT_SINGLE_TOP_BORDER(data->terminalWidth);

    printf("  Instrukcja Obsługi:\n\n");
    PrintGenericBorderEdges(0, data->terminalWidth, 2, SINGLE_VERTICAL_LINE, false);
    PrintGenericBorderEdges(0, data->terminalWidth, 3, SINGLE_VERTICAL_LINE, false);

    //DrawBorderEdges(terminalWidth, &tmpY);
    //GetCursorPosition(&tmp, &spliterY1);
    PRINT_SINGLE_TJUNCTION_BORDER(data->terminalWidth);

    char* text =  "W naszej aplikacji będziesz miał możliwość sprawdzenia się w popularnym teleturnieju jakim są Milionerzy!" 
        " W tej aplikacji będziesz w stanie sprawdzić swoją wiedzę w najróżniejszych dziedzinach, bądź swoich znajomych dzięki możliwości dodawania "
        "własnych pytań! Czytaj uważnie pytania i niech nie zwiedzie ciebie pewność siebie. Powodzenia!";
    int width = widthWithoutBorders - 6;
    
    printf("  Opis: ");
    PrintWrappedLine(text, width, 7, false);

    GetCursorPosition(&tmp, &tmpY);
    for (int i = 0; i < GetWrappedLineCount(text, width); i++)
    {
        PrintGenericBorderEdges(0, data->terminalWidth, tmpY - i, SINGLE_VERTICAL_LINE, false);
    }
    SetCursorPosition(tmp, tmpY);

    text = "Podczas rozgrywki jako gracz zmierzysz się z 10 różnymi pytaniami z szerokiej puli pytań posilając się kołami ratunkowymi. "
        "Na każde pytanie istnieje tylko 1 prawidłowa odpowiedź! Wybierając odpowiedź, bądź koło ratunkowe podświetli się ono na inny kolor oczekując na "
        "potwierdzenie (ponowne wciśnięcie danego klawisza)."
        "Wraz z kolejnymi pytaniami kwota pieniężna o którą grasz będzie wzrastać ( im dalej tym większa szansa na znaczną wygraną ). ";
    width = widthWithoutBorders - 8;

    printf("\n\n  Zasady: ");
    PrintWrappedLine(text,
        width, 9, false);
        
    printf("\n\n");
    GetCursorPosition(&tmp, &tmpY);
    for (int i = 0; i < GetWrappedLineCount(text, width) + 3; i++)
    {
        PrintGenericBorderEdges(0, data->terminalWidth, tmpY - i, SINGLE_VERTICAL_LINE, false);
    }
    SetCursorPosition(tmp, tmpY);

    PRINT_SINGLE_TJUNCTION_BORDER(data->terminalWidth);

    text = "Przed rozpoczęciem rozgrywki zapoznaj się z ustawieniem kodowania znaków w zakładce \"Ustawienia\" dostępnej w Menu!";
    width = widthWithoutBorders;

    SetColor(COLOR_FG_RED);
    printf(CSR_MOVE_RIGHT(2));
    PrintWrappedLine("UWAGA!", widthWithoutBorders, 2, true);
    printf("\n" CSR_MOVE_RIGHT(2));
    PrintWrappedLine(text, width, 2, true);
    ResetColor();

    // .
    GetCursorPosition(&tmp, &tmpY);
    for (int i = 0; i < GetWrappedLineCount(text, width) + 2; i++)
    {
        PrintGenericBorderEdges(0, data->terminalWidth, tmpY - i, SINGLE_VERTICAL_LINE, false);
    }
    SetCursorPosition(tmp, tmpY);

    printf("\n");

    PRINT_SINGLE_BOTTOM_BORDER(data->terminalWidth);
    
    PrintWrappedLine("Strona: 1/3", data->terminalWidth, 0, true);
}

void DrawUIPageTwo(InstructionPageData* data) {
    int tmpY = 2;
    const int widthWithoutBorders = data->terminalWidth - 4;
    int tmp;

    PRINT_SINGLE_TOP_BORDER(data->terminalWidth);

    printf("  Jak grać:");
    PrintGenericBorderEdges(0, data->terminalWidth, 3, SINGLE_VERTICAL_LINE, false);

    const int offset = 21;

    int width = widthWithoutBorders- offset + 1;
    int lineCount = 2;

    printf("\n  *) [  Strzałki ] - ");
    char* text = "Korzystaj z nich aby nawigować się w Menu oraz podczas teleturnieju.";
    PrintWrappedLine(text, width, offset, false);
    lineCount += GetWrappedLineCount(text, width);

    printf("\n  *) [    Esc    ] - ");
    text = "Wróć do poprzedniego okna / Zakońć podejście w teleturnieju";
    PrintWrappedLine(text, width, offset, false);
    lineCount += GetWrappedLineCount(text, width);

    printf("\n  *) [   ENTER   ] - ");
    text = "Wybierz / Potwierdź daną opcję";
    PrintWrappedLine(text, width, offset, false);
    lineCount += GetWrappedLineCount(text, width);

    printf("\n  *) [     1     ] - ");
    text = "Wybierz / Potwierdź 1 koło ratunkowe (pytanie do publiczności - publiczność głosuje za poprawną odpowiedzią)";
    PrintWrappedLine(text, width, offset, false);
    lineCount += GetWrappedLineCount(text, width);    
    
    printf("\n  *) [     2     ] - ");
    text = "Wybierz / Potwierdź 2 koło ratunkowe (50/50 - ukazuje 2 z 4 odpowiedzi które są nieprawidłowe)";
    PrintWrappedLine(text, width, offset, false);
    lineCount += GetWrappedLineCount(text, width);    
    
    printf("\n  *) [     3     ] - ");
    text = "Wybierz / Potwierdź 3 koło ratunkowe (telefon do przyjaciela - twój przyjaciel sugeruje poprawną według niego odpowiedź)";
    PrintWrappedLine(text, width, offset, false);
    lineCount += GetWrappedLineCount(text, width);  
    
    printf("\n  *) [ SHIFT + R ] - ");
    text = "Odświerz wyświetlany obraz konsoli (działa jedynie podczas pytań teleturnieju!)";
    PrintWrappedLine(text, width, offset, false);
    lineCount += GetWrappedLineCount(text, width);  

    printf("\n\n");
    lineCount += 2;

    // .
    GetCursorPosition(&tmp, &tmpY);
    for (int i = 0; i < lineCount; i++)
    {
        PrintGenericBorderEdges(0, data->terminalWidth, tmpY - i, SINGLE_VERTICAL_LINE, false);
    }
    SetCursorPosition(tmp, tmpY);

    PRINT_SINGLE_BOTTOM_BORDER(data->terminalWidth);

    PrintWrappedLine("Strona: 2/3", data->terminalWidth, 0, true);
}

void DrawUIPageThree(InstructionPageData* data) {
    int tmpY = 2;
    const int widthWithoutBorders = data->terminalWidth - 4;
    int tmp;

    PRINT_SINGLE_TOP_BORDER(data->terminalWidth);

    printf("  Zakończenie Gry: ");
    PrintGenericBorderEdges(0, data->terminalWidth, 3, SINGLE_VERTICAL_LINE, false);

    const int offset2 = 33;

    int width = widthWithoutBorders- offset2 + 1;
    int lineCount = 2;

    printf("\n  *) [     Wielka wygrana!   ] - ");
    char* text = "Pokonując wszelkie przeszkody osiągasz swój cel wygrywając główną nagrodę teleturnieju.";
    PrintWrappedLine(text, width, offset2, false);
    lineCount += GetWrappedLineCount(text, width);  

    printf("\n  *) [  Rezygnacja / wyjście ] - ");
    text = "Nie znając poprawnej odpowiedzi oraz decydujac się poddać, kończysz swoje podejście w teleturnieju wygrywając ostatnią kwotę o którą w pytaniu dobrze odpowiedziałeś. "
    SET_COLOR_BRIGHT_RED "Jeśli jednak zrezygnujesz już na 1 pytaniu ukończysz teleturniej bez żadniej nagrody!" RESET_COLOR;
    PrintWrappedLine(text, width, offset2, false);
    lineCount += GetWrappedLineCount(text, width);  

    printf("\n  *) [      Zła odpowiedź    ] - ");
    text = "Popełniając błąd, zaznaczając błędną odpowiedź, kończysz swoje podejście w tym teleturnieju. Zdobywasz jednak nagrodę w postaci ostatniego \"punktu kontrolnego\" na który odpowiedziałeś poprawnie. Punkty te są umiejscowione odpowiednio na 1, 3 i 6 pytaniu. "
    SET_COLOR_BRIGHT_RED "Jeśli pomylisz się już na 1 pytaniu ukończysz teleturniej bez żadniej nagrody!" RESET_COLOR;
    PrintWrappedLine(text, width, offset2, false);
    lineCount += GetWrappedLineCount(text, width);  

    printf("\n\n");
    lineCount += 2;

    // .
    GetCursorPosition(&tmp, &tmpY);
    for (int i = 0; i < lineCount; i++)
    {
        PrintGenericBorderEdges(0, data->terminalWidth, tmpY - i, SINGLE_VERTICAL_LINE, false);
    }
    SetCursorPosition(tmp, tmpY);

    PRINT_SINGLE_BOTTOM_BORDER(data->terminalWidth);

    PrintWrappedLine("Strona: 3/3", data->terminalWidth, 0, true);
}

void DrawBottomInstructions(InstructionPageData* data) {
    char* text = "[ <- ] - Zmień stronę / [ Esc ] - Wciśnij Escape aby powrócić do głównego Menu. / Zmień stronę - [ -> ]";
    int lineCount = GetWrappedLineCount(text, data->terminalWidth);
    SetCursorPosition(0, data->terminalHeight - lineCount + 1);
    PrintWrappedLine(text, data->terminalWidth, 0, true);
}

void OnInstructionPageResize(int width, int height, void* data) {
    InstructionPageData* pageData = (InstructionPageData*)data;
    pageData->terminalWidth = width;
    pageData->terminalHeight = height;
    DrawUI(pageData);
}

void DrawUI(InstructionPageData* data){
    ClearScreenManual();
    switch (data->pageNumber)
    {
        case 1:
            DrawUIPageOne(data);
            break;
        case 2:
            DrawUIPageTwo(data);
            break;
        case 3:
            DrawUIPageThree(data);
            break;
    }
    DrawBottomInstructions(data);
}
