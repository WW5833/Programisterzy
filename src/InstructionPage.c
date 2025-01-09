#include "InstructionPage.h"
#include "PageUtils.h"
#include "AnsiHelper.h"
#include "Utf8Symbols.h"
#include "IOHelper.h"
#include "TextHelper.h"

#define SET_COLOR_RED ESC_SEQ "38;2;139;0;0m"
#define SET_COLOR_BRIGHT_RED ESC_SEQ "38;2;255;36;0m"

#define RESET_COLOR ESC_SEQ "39;40m"

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

extern int LatestTerminalWidth, LatestTerminalHeight;

void PageEnter_Instruction()
{
    HideCursor();

    InstructionPageData data;
    data.terminalWidth = LatestTerminalWidth;
    data.terminalHeight = LatestTerminalHeight;
    data.pageNumber = 1;

    DrawUI(&data);

    SetResizeHandler(OnInstructionPageResize, &data);

    while(true){
        KeyInputType key = HandleInteractions(true);
        switch (key)
        {
            case KEY_ENTER:
                break;
                
            case KEY_ARROW_LEFT: {
                data.pageNumber--;
                if(data.pageNumber < 1) data.pageNumber = 1;
                else DrawUI(&data);
                break;
            }
                
            case KEY_ARROW_RIGHT: {
                data.pageNumber++;
                if(data.pageNumber > 3) data.pageNumber = 3;
                else DrawUI(&data);
                break;
            }

            case KEY_ESCAPE: 
                UnsetResizeHandler();
                return;
            
            default:
                break;
        }
    }
}

void PrintBorderEdges(InstructionPageData* data, int lineCount) {
    printf("\r");
    if(lineCount > -1) printf(CSR_MOVE_UP(lineCount - 1));
    for (int i = 0; i < lineCount; i++)
    {
        PrintGenericBorderEdgesHere(0, data->terminalWidth, SINGLE_VERTICAL_LINE);
        printf("\r" CSR_MOVE_DOWN_1);
    }
}

void DrawUIPageOne(InstructionPageData* data) {
    const int widthWithoutBorders = data->terminalWidth - 4;

    PRINT_SINGLE_TOP_BORDER(data->terminalWidth);

    printf("  Instrukcja Obsługi:\n\n");
    PrintGenericBorderEdges(0, data->terminalWidth, 2, SINGLE_VERTICAL_LINE, false);
    PrintGenericBorderEdges(0, data->terminalWidth, 3, SINGLE_VERTICAL_LINE, false);

    PRINT_SINGLE_TJUNCTION_BORDER(data->terminalWidth);
    
    printf("  Opis: ");
    int lineCount = PrintWrappedLine("W naszej aplikacji będziesz miał możliwość sprawdzenia się w popularnym teleturnieju jakim są Milionerzy!" 
        " W tej aplikacji będziesz w stanie sprawdzić swoją wiedzę w najróżniejszych dziedzinach, bądź swoich znajomych dzięki możliwości dodawania "
        "własnych pytań! Czytaj uważnie pytania i niech nie zwiedzie ciebie pewność siebie. Powodzenia!",
        widthWithoutBorders - 6, 7, false);

    PrintBorderEdges(data, lineCount);

    printf("\n  Zasady: ");
    lineCount = PrintWrappedLine("Podczas rozgrywki jako gracz zmierzysz się z 10 różnymi pytaniami z szerokiej puli pytań posilając się kołami ratunkowymi. "
        "Na każde pytanie istnieje tylko 1 prawidłowa odpowiedź! Wybierając odpowiedź, bądź koło ratunkowe podświetli się ono na inny kolor oczekując na "
        "potwierdzenie (ponowne wciśnięcie danego klawisza)."
        "Wraz z kolejnymi pytaniami kwota pieniężna o którą grasz będzie wzrastać ( im dalej tym większa szansa na znaczną wygraną ). ",
        widthWithoutBorders - 8, 9, false) + 2;
        
    printf("\n");
    
    PrintBorderEdges(data, lineCount);

    PRINT_SINGLE_BOTTOM_BORDER(data->terminalWidth);
    
    PrintWrappedLine("Strona: 1/3", data->terminalWidth, 0, true);
}

void DrawUIPageTwo(InstructionPageData* data) {
    const int widthWithoutBorders = data->terminalWidth - 4;

    PRINT_SINGLE_TOP_BORDER(data->terminalWidth);

    printf("  Jak grać:");
    PrintGenericBorderEdges(0, data->terminalWidth, 3, SINGLE_VERTICAL_LINE, false);

    const int offset = 21;

    int width = widthWithoutBorders- offset + 1;
    int lineCount = 2;

    printf("\n  *) [  Strzałki ] - ");
    lineCount += PrintWrappedLine("Korzystaj z nich aby nawigować się w Menu oraz podczas teleturnieju.",
        width, offset, false);

    printf("\n  *) [    Esc    ] - ");
    lineCount += PrintWrappedLine("Wróć do poprzedniego okna / Zakończ podejście w teleturnieju",
        width, offset, false);

    printf("\n  *) [   ENTER   ] - ");
    lineCount += PrintWrappedLine("Wybierz / Potwierdź daną opcję",
        width, offset, false);

    printf("\n  *) [     1     ] - ");
    lineCount += PrintWrappedLine("Wybierz / Potwierdź 1 koło ratunkowe (pytanie do publiczności - publiczność głosuje za poprawną odpowiedzią)",
        width, offset, false);
    
    printf("\n  *) [     2     ] - ");
    lineCount += PrintWrappedLine("Wybierz / Potwierdź 2 koło ratunkowe (50/50 - zaznacza 2 z 4 odpowiedzi które są nieprawidłowe)",
        width, offset, false);
    
    printf("\n  *) [     3     ] - ");
    lineCount += PrintWrappedLine("Wybierz / Potwierdź 3 koło ratunkowe (telefon do przyjaciela - twój przyjaciel sugeruje poprawną według niego odpowiedź)",
        width, offset, false);
    
    printf("\n  *) [ SHIFT + R ] - ");
    lineCount += PrintWrappedLine("Odśwież wyświetlany obraz konsoli (działa jedynie podczas pytań teleturnieju!)",
        width, offset, false);

    printf("\n\n");
    lineCount += 2;

    PrintBorderEdges(data, lineCount);

    PRINT_SINGLE_BOTTOM_BORDER(data->terminalWidth);

    PrintWrappedLine("Strona: 2/3", data->terminalWidth, 0, true);
}

void DrawUIPageThree(InstructionPageData* data) {
    const int widthWithoutBorders = data->terminalWidth - 4;

    PRINT_SINGLE_TOP_BORDER(data->terminalWidth);

    printf("  Zakończenie Gry: ");
    PrintGenericBorderEdges(0, data->terminalWidth, 3, SINGLE_VERTICAL_LINE, false);

    const int offset2 = 33;

    int width = widthWithoutBorders- offset2 + 1;
    int lineCount = 2;

    printf("\n  *) [     Wielka wygrana!   ] - ");
    lineCount += PrintWrappedLine("Pokonując wszelkie przeszkody osiągasz swój cel wygrywając główną nagrodę teleturnieju.",
        width, offset2, false);

    printf("\n  *) [  Rezygnacja / wyjście ] - ");
    lineCount += PrintWrappedLine("Nie znając poprawnej odpowiedzi oraz decydując się poddać, kończysz swoje podejście w teleturnieju wygrywając ostatnią kwotę o którą w pytaniu dobrze odpowiedziałeś. "
        SET_COLOR_BRIGHT_RED "Jeśli jednak zrezygnujesz już na 1 pytaniu ukończysz teleturniej bez żadnej nagrody!" RESET_COLOR,
        width, offset2, false);

    printf("\n  *) [      Zła odpowiedź    ] - ");
    lineCount += PrintWrappedLine("Popełniając błąd, zaznaczając błędną odpowiedź, kończysz swoje podejście w tym teleturnieju. Zdobywasz jednak nagrodę w postaci ostatniego \"punktu kontrolnego\" na który odpowiedziałeś poprawnie. Punkty te są umiejscowione odpowiednio na 1, 3 i 6 pytaniu. "
        SET_COLOR_BRIGHT_RED "Jeśli pomylisz się już na 1 pytaniu ukończysz teleturniej bez żadnej nagrody!" RESET_COLOR,
        width, offset2, false);

    printf("\n\n");
    lineCount += 2;

    PrintBorderEdges(data, lineCount);

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
