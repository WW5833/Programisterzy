#include "QuestionEditPage.h"

#include "AnsiHelper.h"
#include "PageUtils.h"

bool PageEnter_QuestionEdit(Question *question) // Returns whether to save the question
{
    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);

    ClearScreen();

    printf("Edytuj pytanie");
    printf("\n[*] Zapisz i wyjdź");
    printf("\n[ ] Wyjdź bez zapisywania");
    printf("\n[ ] Usuń pytanie");

    printf("\n    Id pytania: %d", question->Id);
    printf("\n[ ] Treść pytania: ");
    PrintWrappedLine(question->Content, terminalWidth - 19, 19, false);
    printf("\n[ ] Odpowiedź Poprawna: ");
    PrintWrappedLine(question->Answer[0], terminalWidth - 24, 24, false);
    printf("\n[ ] Odpowiedź B:        ");
    PrintWrappedLine(question->Answer[1], terminalWidth - 24, 24, false);
    printf("\n[ ] Odpowiedź C:        ");
    PrintWrappedLine(question->Answer[2], terminalWidth - 24, 24, false);
    printf("\n[ ] Odpowiedź D:        ");
    PrintWrappedLine(question->Answer[3], terminalWidth - 24, 24, false);

    printf("\n[ ] Podpowiedź przyjaciela: ");
    if(question->Help != NULL) PrintWrappedLine(question->Help, terminalWidth - 28, 28, false);

    WaitForEnter();

    return false;
}
