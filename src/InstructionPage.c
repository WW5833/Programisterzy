#include "InstructionPage.h"
#include "PageUtils.h"
#include "AnsiHelper.h"
#include "Utf8Symbols.h"
#include "IOHelper.h"

void DrawUI(int terminalWidth, int terminalHeight);
void OnInstructionPageResize(int width, int height, void* data);

void PageEnter_Instruction()
{
    DisableAlternativeBuffer();

    HideCursor();

    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);
    
    DrawUI(terminalWidth, terminalHeight);

    SetResizeHandler(OnInstructionPageResize, NULL);

    WaitForKeys(ESC);

    UnsetResizeHandler();
    
    ClearScreen();

    EnableAlternativeBuffer();
}

void DrawUI(int terminalWidth, int terminalHeight) {
    ClearScreen();

    const int widthWithoutBorders = terminalWidth - 4;

    int spliterY1, spliterY2, spliterY3; 
    int tmp;

    PRINT_SINGLE_TOP_BORDER(terminalWidth);

    printf("  Instrukcja Obsługi:\n\n");

    GetCursorPosition(&tmp, &spliterY1);
    //PRINT_TJUNCTION_BORDER(terminalWidth);
    
    printf("\n  Opis: ");
    PrintWrappedLine("W naszej aplikacji będziesz miał możliwość sprawdzenia się w popularnym teleturnieju jakim są Milionerzy!" 
        " W tej aplikacji będziesz w stanie sprawdzić swoją wiedzę w najróżniejszych dziedzinach, bądź swoich znajomych dzięki możliwości dodawania "
        "własnych pytań! Gra ta trochę różni się jednak od orginału, tak więc niech nie zwiedzie ciebie pewność siebie.", 
        widthWithoutBorders - 6, 7, false);

    printf("\n\n  Zasady: ");
    PrintWrappedLine("Podczas rozgrywki jako gracz zmierzysz się z 10 różnymi pytaniami z szerokiej puli pytań posilając się kołami ratunkowymi. "
        "Na każde pytanie istnieje tylko 1 prawidłowa odpowiedź! Wybierając odpowiedź, bądź koło ratunkowe podświetli się ono na inne kolor oczekując na "
        "potwierdzenie (ponowne wciśnię cie danego klawisza)."
        "Wraz z kolejnymi pytaniami kwota pieniężna o którą grasz będzie wzrastać ( im dalej tym większa szansa na znaczną wygraną ). "
        "Pomyłka skutkuje natomiast natychmiastową porażką oraz utratą swojej wygranej. Podczas chwili niepewności jako gracz możesz "
        "zawsze się wycofać. W takom przypadku wygrasz ostatnią \"bezpieczną\" nagrodę które znajdują się odpowiednio na 1, 3 oraz 6 pytaniu.", 
        widthWithoutBorders - 8, 9, false);
    printf("\n\n");
    
    GetCursorPosition(&tmp, &spliterY2);
    //PRINT_TJUNCTION_BORDER(terminalWidth);

    printf("\n  Jak grać: \n");

    SetColor(COLOR_FG_RED + COLOR_BRIGHT_MOD);
    PrintWrappedLine("UWAGA!", widthWithoutBorders, 2, true);
    printf("\n");
    PrintWrappedLine("Przed rozpoczęciem rozgrywki zapoznaj się z ustawieniem kodowania znaków w zakładce Ustawienia dostępnej w Menu!", widthWithoutBorders, 2, true);
    ResetColor();

    const int offset = 21;
    printf("\n");
    printf("\n  *) [  Strzałki ] - ");
    PrintWrappedLine("Korzystaj z nich aby nawigować się w Menu oraz podczas teleturnieju.", widthWithoutBorders - offset + 1, offset, false);
    printf("\n  *) [    Esc    ] - ");
    PrintWrappedLine("Wróć do poprzedniego okna / Zakońć podejście w teleturnieju", widthWithoutBorders - offset + 1, offset, false);
    printf("\n  *) [   ENTER   ] - ");
    PrintWrappedLine("Wybierz / Potwierdź daną opcję", widthWithoutBorders - offset + 1, offset, false);
    printf("\n  *) [     1     ] - ");
    PrintWrappedLine("Wybierz / Potwierdź 1 koło ratunkowe (pytanie do publiczności - publiczność głosuje za poprawną odpowiedzią)", widthWithoutBorders - offset + 1, offset, false);
    printf("\n  *) [     2     ] - ");
    PrintWrappedLine("Wybierz / Potwierdź 2 koło ratunkowe (50/50 - ukazuje 2 z 4 odpowiedzi które są nieprawidłowe)", widthWithoutBorders - offset + 1, offset, false);
    printf("\n  *) [     3     ] - ");
    PrintWrappedLine("Wybierz / Potwierdź 3 koło ratunkowe (telefon do przyjaciela - twój przyjaciel sugeruje poprawną według niego odpowiedź)", widthWithoutBorders - offset + 1, offset, false);
    printf("\n  *) [ SHIFT + R ] - ");
    PrintWrappedLine("Odświerz wyświetlany obraz konsoli (działa jedynie podczas pytań teleturnieju!) ", widthWithoutBorders - offset + 1, offset, false);

    printf("\n\n");

    GetCursorPosition(&tmp, &spliterY3);
    //PRINT_TJUNCTION_BORDER(terminalWidth);

    printf("\n  [Esc] - Wciśnij Escape aby powrócić do głównego Menu.\n");

    PRINT_SINGLE_BOTTOM_BORDER(terminalWidth);

    int x, y;
    GetCursorPosition(&x, &y);
    for (int i = 2; i < y - 1; i++)
    {
        PrintGenericBorderEdges(0, terminalWidth, i, SINGLE_VERTICAL_LINE, false);
    }
    
    SetCursorPosition(0, spliterY1);
    PRINT_SINGLE_TJUNCTION_BORDER(terminalWidth);
    SetCursorPosition(0, spliterY2);
    PRINT_SINGLE_TJUNCTION_BORDER(terminalWidth);
    SetCursorPosition(0, spliterY3);
    PRINT_SINGLE_TJUNCTION_BORDER(terminalWidth);
}

void OnInstructionPageResize(int width, int height, void* data) {
    DrawUI(width, height);
}
