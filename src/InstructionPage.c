#include "InstructionPage.h"
#include "AnsiHelper.h"
#include "Utf8Symbols.h"
#include "IOHelper.h"
#include "TextHelper.h"
#include "Popup.h"

#define SET_COLOR_RED ESC_SEQ "38;2;139;0;0m"
#define SET_COLOR_BRIGHT_RED ESC_SEQ "38;2;255;36;0m"

#define RESET_COLOR ESC_SEQ "39;40m"

#define TERMINAL_MIN_WIDTH 70
#define TERMINAL_MIN_HEIGHT 26

#define MIN(x, y) ((x) < (y) ? (x) : (y))

typedef struct {
    int terminalWidth;
    int terminalHeight;
    int pageNumber;
} InstructionPageData;

static void DrawUI(InstructionPageData* data);
static void OnResize(void* data);

extern int LatestTerminalWidth, LatestTerminalHeight;

void PageEnter_Instruction()
{
    HideCursor();

    InstructionPageData data;
    data.terminalWidth = LatestTerminalWidth;
    data.terminalHeight = LatestTerminalHeight;
    data.pageNumber = 1;

    DrawUI(&data);

    SetResizeHandler(OnResize, &data);

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

static void PrintBorderEdges(InstructionPageData* data, int lineCount) {
    printf("\r");
    if(lineCount > -1) printf(CSR_MOVE_UP(lineCount - 1));
    for (int i = 0; i < lineCount; i++)
    {
        PrintGenericBorderEdgesHere(0, data->terminalWidth, SINGLE_VERTICAL_LINE);
        printf("\r" CSR_MOVE_DOWN_1);
    }
}

static void DrawUIPageOne(InstructionPageData* data) {
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

    printf(CSR_MOVE_UP_1);
    PrintWrappedLine(SINGLE_BREAK_LEFT "Strona: 1/3" SINGLE_BREAK_RIGHT, data->terminalWidth, 0, true);
}

static void DrawUIPageTwo(InstructionPageData* data) {
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

    printf("\n  *) [   Enter   ] - ");
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

    printf("\n  *) [ Shift + R ] - ");
    lineCount += PrintWrappedLine("Odśwież wyświetlany obraz konsoli",
        width, offset, false);

    printf("\n");
    lineCount += 1;

    PrintBorderEdges(data, lineCount);

    PRINT_SINGLE_BOTTOM_BORDER(data->terminalWidth);

    printf(CSR_MOVE_UP_1);
    PrintWrappedLine(SINGLE_BREAK_LEFT "Strona: 2/3" SINGLE_BREAK_RIGHT, data->terminalWidth, 0, true);
}

static void DrawUIPageThree(InstructionPageData* data) {
    const int widthWithoutBorders = data->terminalWidth - 4;

    PRINT_SINGLE_TOP_BORDER(data->terminalWidth);

    printf("  Zakończenie Gry: ");
    PrintGenericBorderEdges(0, data->terminalWidth, 3, SINGLE_VERTICAL_LINE, false);

    const int offset2 = 27;

    int width = widthWithoutBorders- offset2 + 1;
    int lineCount = 2;

    printf("\n  *) [ Wielka wygrana! ] - ");
    lineCount += PrintWrappedLine("Pokonując wszelkie przeszkody osiągasz swój cel wygrywając główną nagrodę teleturnieju.",
        width, offset2, false);

    printf("\n  *) [    Rezygnacja   ] - ");
    lineCount += PrintWrappedLine("Nie znając poprawnej odpowiedzi oraz decydując się poddać, kończysz swoje podejście w teleturnieju wygrywając ostatnią kwotę o którą w pytaniu dobrze odpowiedziałeś. "
        SET_COLOR_BRIGHT_RED "Jeśli jednak zrezygnujesz już na 1 pytaniu ukończysz teleturniej bez żadnej nagrody!",
        width, offset2, false);
    ResetColor();

    printf("\n  *) [  Zła odpowiedź  ] - ");
    lineCount += PrintWrappedLine("Popełniając błąd, zaznaczając błędną odpowiedź, kończysz swoje podejście w tym teleturnieju. Zdobywasz jednak nagrodę w postaci ostatniego \"punktu kontrolnego\" na który odpowiedziałeś poprawnie. Punkty te są umiejscowione odpowiednio na 1, 3 i 6 pytaniu. "
        SET_COLOR_BRIGHT_RED "Jeśli pomylisz się już na 1 pytaniu ukończysz teleturniej bez żadnej nagrody!",
        width, offset2, false);
    ResetColor();

    printf("\n");
    lineCount += 1;

    PrintBorderEdges(data, lineCount);

    PRINT_SINGLE_BOTTOM_BORDER(data->terminalWidth);

    printf(CSR_MOVE_UP_1);
    PrintWrappedLine(SINGLE_BREAK_LEFT "Strona: 3/3" SINGLE_BREAK_RIGHT, data->terminalWidth, 0, true);
}

static void DrawUI_BottomInstructions(InstructionPageData* data) {
    char* text = "[ Esc ] Wciśnij aby powrócić do głównego Menu.\n[ ← ] Poprzednia strona / Następna strona [ → ]";
    int lineCount = GetWrappedLineCount(text, data->terminalWidth);
    SetCursorPosition(0, data->terminalHeight - lineCount + 1);
    PrintWrappedLine(text, data->terminalWidth, 0, true);
}

static void OnResize(void* data) {
    InstructionPageData* pageData = (InstructionPageData*)data;
    pageData->terminalWidth = LatestTerminalWidth;
    pageData->terminalHeight = LatestTerminalHeight;
    DrawUI(pageData);
}

static void DrawUI(InstructionPageData* data){
    char buffer[256];
    while(data->terminalWidth < TERMINAL_MIN_WIDTH || data->terminalHeight < TERMINAL_MIN_HEIGHT) {
        UnsetResizeHandler();

        sprintf(buffer, ERRMSG_TOO_SMALL_TERMINAL(TERMINAL_MIN_WIDTH, TERMINAL_MIN_HEIGHT));
        ShowAlertPopupWithTitleKeys(ERRMSG_ERROR_POPUP_TITLE, buffer, MIN(data->terminalWidth, 60), RESIZE_EVENT);

        data->terminalWidth = LatestTerminalWidth;
        data->terminalHeight = LatestTerminalHeight;

        SetResizeHandler(OnResize, data);
    }

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

    DrawUI_BottomInstructions(data);
}
